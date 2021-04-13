/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "screenarranger.h"

#include <QMouseEvent>
#include <QPainter>
#include <the-libs_global.h>

#include <Screens/screendaemon.h>
#include <Screens/systemscreen.h>

struct Snap {
    Snap() {
        valid = false;
    }

    Snap(double local, int global) {
        this->local = local;
        this->global = global;
        valid = true;
    }

    operator bool() const {
        return valid;
    }

    double local;
    int global;
    bool valid;
};

struct ScreenArrangerPrivate {
    QList<SystemScreen*> registeredScreens;

    SystemScreen* draggedScreen;
    QPointF initialDragPoint;
    QPointF dragOffset;

    Snap snapLeft, snapRight, snapTop, snapBottom;
};

ScreenArranger::ScreenArranger(QWidget* parent) : QWidget(parent) {
    d = new ScreenArrangerPrivate();

    connect(ScreenDaemon::instance(), &ScreenDaemon::screensUpdated, this, &ScreenArranger::updateScreens);
    updateScreens();
}

ScreenArranger::~ScreenArranger() {
    delete d;
}

QSize ScreenArranger::sizeHint() const {
    return SC_DPI_T(QSize(0, 300), QSize);
}

void ScreenArranger::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    painter.setFont(this->font());
    painter.setBrush(this->palette().color(QPalette::Window));
    painter.setPen(this->palette().color(QPalette::WindowText));
    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
        painter.save();

        QRectF rect = screenRect(screen);

        if (d->draggedScreen == screen) rect = draggedScreenRect(rect);

        painter.fillRect(rect, painter.brush());

        if (!screen->powered()) painter.setOpacity(0.5);
        painter.drawRect(rect);

        QString text = screen->displayName();
        painter.drawText(rect, Qt::AlignCenter, text);

        painter.restore();
    }
}

double ScreenArranger::scaling() {
    QRect rect = ScreenDaemon::instance()->screens().first()->geometry();
    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
        rect = rect.united(screen->geometry());
    }

    //TODO: better scaling
    return qMin((this->height() * 0.5) / rect.height(), (this->width() * 0.5) / rect.width());
}

QRectF ScreenArranger::totalScreen() {
    QRectF totalScreen = ScreenDaemon::instance()->screens().first()->geometry();
    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
        totalScreen = totalScreen.united(screen->geometry());
    }
    totalScreen = QRectF(totalScreen.topLeft() * scaling(), totalScreen.size() * scaling());
    totalScreen.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    return totalScreen;
}

QRectF ScreenArranger::screenRect(SystemScreen* screen) {
    QRect geometry = screen->geometry();
    return QRectF(geometry.topLeft() * scaling(), geometry.size() * scaling()).translated(totalScreen().topLeft());
}

QRectF ScreenArranger::draggedScreenRect(QRectF screenRect) {
    screenRect.translate(d->dragOffset);
    if (d->snapLeft) screenRect.moveLeft(d->snapLeft.local);
    if (d->snapRight) screenRect.moveRight(d->snapRight.local);
    if (d->snapTop) screenRect.moveTop(d->snapTop.local);
    if (d->snapBottom) screenRect.moveBottom(d->snapBottom.local);
    return screenRect;
}

void ScreenArranger::updateScreens() {
    for (SystemScreen* screen : d->registeredScreens) {
        screen->disconnect(this);
    }
    d->registeredScreens.clear();

    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
        connect(screen, &SystemScreen::geometryChanged, this, QOverload<>::of(&ScreenArranger::update));
        d->registeredScreens.append(screen);
    }
    this->update();
}

void ScreenArranger::mousePressEvent(QMouseEvent* event) {
    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
        if (screenRect(screen).contains(event->pos())) {
            d->draggedScreen = screen;
            d->initialDragPoint = event->pos();
            d->dragOffset = QPoint();
        }
    }
}

void ScreenArranger::mouseReleaseEvent(QMouseEvent* event) {
    if (d->draggedScreen) {
        QRectF newGeometry = draggedScreenRect(screenRect(d->draggedScreen)).translated(-totalScreen().topLeft());
        newGeometry = QRectF(newGeometry.topLeft() / scaling(), newGeometry.size() / scaling());

        if (d->snapLeft) newGeometry.moveLeft(d->snapLeft.global);
        if (d->snapRight) newGeometry.moveRight(d->snapRight.global);
        if (d->snapTop) newGeometry.moveTop(d->snapTop.global);
        if (d->snapBottom) newGeometry.moveBottom(d->snapBottom.global);

        d->draggedScreen->move(newGeometry.topLeft().toPoint());
    }
    d->draggedScreen = nullptr;
    this->update();
}

void ScreenArranger::mouseMoveEvent(QMouseEvent* event) {
    if (d->draggedScreen) {
        d->dragOffset = event->pos() - d->initialDragPoint;

        auto trySnap = [ = ](Snap * snap, double snapTo, int snapToGlobal, double currentValue) {
            if ((currentValue < snapTo + SC_DPI(5)) && (currentValue > snapTo - SC_DPI(5))) {
                *snap = Snap(snapTo, snapToGlobal);
            }
        };

        d->snapLeft = d->snapRight = d->snapTop = d->snapBottom = Snap();

        QRectF geometry = screenRect(d->draggedScreen).translated(d->dragOffset);
        for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
            //Attempt to snap to this screen

            QRectF snapTo = screenRect(screen);
            trySnap(&d->snapLeft, snapTo.right(), screen->geometry().right() + 1, geometry.left()); //Left edge to right edge
            trySnap(&d->snapRight, snapTo.left(), screen->geometry().left(), geometry.right()); //Right edge to left edge
            trySnap(&d->snapBottom, snapTo.bottom(), screen->geometry().bottom() + 1, geometry.bottom()); //Bottom edge to bottom edge
            trySnap(&d->snapTop, snapTo.top(), screen->geometry().top(), geometry.top()); //Top edge to top edge
            trySnap(&d->snapBottom, snapTo.top(), screen->geometry().top(), geometry.bottom()); //Bottom edge to top edge
            trySnap(&d->snapTop, snapTo.bottom(), screen->geometry().bottom() + 1, geometry.top()); //Top edge to bottom edge
        }

        this->update();
    }
}
