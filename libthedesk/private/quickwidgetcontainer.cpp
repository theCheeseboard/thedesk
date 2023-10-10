/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
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
#include "quickwidgetcontainer.h"
#include "ui_quickwidgetcontainer.h"

#include "chunk.h"
#include <QPainter>
#include <QScreen>
#include <QTimer>
#include <Wm/desktopwm.h>
#include <barmanager.h>
#include <libcontemporary_global.h>
#include <statemanager.h>
#include <tlogger.h>
#include <tvariantanimation.h>

struct QuickWidgetContainerPrivate {
        QWidget* parentChunk;
        QWidget* quickWidget;
        int pointX = 0;

        tVariantAnimation* yAnim;
        tVariantAnimation* opacityAnim;
        bool isShowing = false;

        BarManager::BarLockPtr barLock;
};

QuickWidgetContainer::QuickWidgetContainer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::QuickWidgetContainer) {
    ui->setupUi(this);

    //    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Popup);
    this->setWindowFlags(Qt::Popup);
    this->setAttribute(Qt::WA_TranslucentBackground);

    d = new QuickWidgetContainerPrivate();
    d->parentChunk = parent;

    d->yAnim = new tVariantAnimation(this);
    d->yAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->yAnim->setDuration(100);
    connect(d->yAnim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        this->move(this->x(), value.toInt());
    });

    d->opacityAnim = new tVariantAnimation(this);
    d->opacityAnim->setStartValue(0.0);
    d->opacityAnim->setEndValue(1.0);
    d->opacityAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->opacityAnim->setDuration(100);
    connect(d->opacityAnim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        this->setWindowOpacity(value.toDouble());
    });
    connect(d->opacityAnim, &tVariantAnimation::finished, this, [=] {
        if (d->opacityAnim->direction() == tVariantAnimation::Backward) this->hide();
    });

    this->setWindowOpacity(0);
    this->setContentsMargins(1, 6, 1, 1);
    ui->chunkContainerLayout->installEventFilter(this);
}

QuickWidgetContainer::~QuickWidgetContainer() {
    delete ui;
    delete d;
}

QSize QuickWidgetContainer::sizeHint() const {
    QSize sizeHint;
    Chunk* chunk = qobject_cast<Chunk*>(d->parentChunk);
    if (chunk) {
        sizeHint = chunk->quickWidget()->sizeHint();
    } else {
        sizeHint = d->quickWidget->sizeHint();
    }

    return sizeHint.grownBy(this->contentsMargins());
}

void QuickWidgetContainer::showContainer() {
    Chunk* chunk = qobject_cast<Chunk*>(d->parentChunk);
    if (chunk) {
        ui->chunkContainerLayout->addWidget(chunk->quickWidget());
        chunk->quickWidget()->installEventFilter(this);
    } else {
        ui->chunkContainerLayout->addWidget(d->quickWidget);
        d->quickWidget->installEventFilter(this);
    }
    calculatePosition();

    d->isShowing = true;

    this->show();
    DesktopWm::setSystemWindow(this, DesktopWm::SystemWindowTypeSkipTaskbarOnly);
    this->activateWindow();
    calculatePosition();

    d->yAnim->setDirection(tVariantAnimation::Forward);
    d->opacityAnim->setDirection(tVariantAnimation::Forward);
    d->yAnim->start();
    d->opacityAnim->start();

    d->barLock = StateManager::barManager()->acquireLock();
    emit showing();
}

void QuickWidgetContainer::hideContainer() {
    d->yAnim->setDirection(tVariantAnimation::Backward);
    d->opacityAnim->setDirection(tVariantAnimation::Backward);
    d->yAnim->start();
    d->opacityAnim->start();

    d->isShowing = false;

    d->barLock->unlock();
    emit hiding();
}

bool QuickWidgetContainer::isShowing() {
    return d->isShowing;
}

void QuickWidgetContainer::setQuickWidget(QWidget* widget) {
    d->quickWidget = widget;
}

void QuickWidgetContainer::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(this->palette().color(QPalette::WindowText));
    painter.setBrush(this->palette().color(QPalette::Window));

    QPolygon poly;
    int topEdge = 5;
    int leftPoint = qMax(0, d->pointX - 5);
    int rightPoint = qMin(this->width() - 1, d->pointX + 5);

    poly.append(QPoint(0, topEdge));
    poly.append(QPoint(leftPoint, topEdge));
    poly.append(QPoint(d->pointX, 0));
    poly.append(QPoint(rightPoint, topEdge));
    poly.append(QPoint(this->width() - 1, topEdge));
    poly.append(QPoint(this->width() - 1, this->height() - 1));
    poly.append(QPoint(0, this->height() - 1));
    poly.append(QPoint(0, topEdge));
    painter.drawPolygon(poly);
}

void QuickWidgetContainer::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);
    switch (event->type()) {
        case QEvent::ActivationChange:
            if (!this->isActiveWindow()) this->hideContainer();
            break;
        default:
            break;
    }
}

void QuickWidgetContainer::calculatePosition() {
    this->setFixedSize(this->sizeHint());

    QPoint midpoint = d->parentChunk->mapToGlobal(QPoint(d->parentChunk->width() / 2, d->parentChunk->height()));
    QRect screenGeometry = qApp->screenAt(midpoint)->geometry();

    QRect geom = this->geometry();
    geom.setSize(this->sizeHint());
    geom.moveLeft(midpoint.x() - geom.width() / 2);
    if (geom.left() < screenGeometry.left() + 9) geom.moveLeft(screenGeometry.left() + 9);
    if (geom.right() > screenGeometry.right() - 9) geom.moveRight(screenGeometry.right() - 9);
    this->setGeometry(geom);

    d->pointX = this->mapFromGlobal(midpoint).x();

    d->yAnim->setStartValue(midpoint.y() - 50);
    d->yAnim->setEndValue(midpoint.y());
}

bool QuickWidgetContainer::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Resize || event->type() == QEvent::LayoutRequest) {
        QTimer::singleShot(0, this, &QuickWidgetContainer::calculatePosition);
    }
    return false;
}

void QuickWidgetContainer::hideEvent(QHideEvent* event) {
    if (d->isShowing) this->hideContainer();
}
