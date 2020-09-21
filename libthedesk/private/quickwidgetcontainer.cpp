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

#include <the-libs_global.h>
#include <QPainter>
#include <tvariantanimation.h>
#include <statemanager.h>
#include <barmanager.h>
#include "chunk.h"

struct QuickWidgetContainerPrivate {
    Chunk* parentChunk;
    int pointX = 0;

    tVariantAnimation* yAnim;
    tVariantAnimation* opacityAnim;

    BarManager::BarLockPtr barLock;
};

QuickWidgetContainer::QuickWidgetContainer(Chunk* parent) :
    QWidget(parent),
    ui(new Ui::QuickWidgetContainer) {
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_X11NetWmWindowTypeNotification);

    d = new QuickWidgetContainerPrivate();
    d->parentChunk = parent;

    d->yAnim = new tVariantAnimation(this);
    d->yAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->yAnim->setDuration(100);
    connect(d->yAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->move(this->x(), value.toInt());
    });

    d->opacityAnim = new tVariantAnimation(this);
    d->opacityAnim->setStartValue(0.0);
    d->opacityAnim->setEndValue(1.0);
    d->opacityAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->opacityAnim->setDuration(100);
    connect(d->opacityAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setWindowOpacity(value.toDouble());
    });
    connect(d->opacityAnim, &tVariantAnimation::finished, this, [ = ] {
        if (d->opacityAnim->direction() == tVariantAnimation::Backward) this->hide();
    });

    this->setWindowOpacity(0);
    this->setContentsMargins(1, SC_DPI(5) + 1, 1, 1);
}

QuickWidgetContainer::~QuickWidgetContainer() {
    delete ui;
    delete d;
}

void QuickWidgetContainer::showContainer() {
    ui->chunkContainerLayout->addWidget(d->parentChunk->quickWidget());
    calculatePosition();

    this->show();
    this->activateWindow();

    d->yAnim->setDirection(tVariantAnimation::Forward);
    d->opacityAnim->setDirection(tVariantAnimation::Forward);
    d->yAnim->start();
    d->opacityAnim->start();

    d->barLock = StateManager::barManager()->acquireLock();
}

void QuickWidgetContainer::hideContainer() {
    d->yAnim->setDirection(tVariantAnimation::Backward);
    d->opacityAnim->setDirection(tVariantAnimation::Backward);
    d->yAnim->start();
    d->opacityAnim->start();

    d->barLock->unlock();
}

void QuickWidgetContainer::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(this->palette().color(QPalette::WindowText));
    painter.setBrush(this->palette().color(QPalette::Window));

    QPolygon poly;
    int topEdge = SC_DPI(5);
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
    int xpoint = midpoint.x() - this->width() / 2;

    d->pointX = this->width() / 2;
    if (xpoint < SC_DPI(9)) {
        int oldX = xpoint;
        xpoint = SC_DPI(9);
        d->pointX -= oldX - xpoint;
    }

    d->yAnim->setStartValue(midpoint.y() - SC_DPI(50));
    d->yAnim->setEndValue(midpoint.y());

    this->move(xpoint, this->y());
}
