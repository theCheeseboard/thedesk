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
#include "gateway.h"
#include "ui_gateway.h"

#include <Gestures/gesturedaemon.h>
#include <QApplication>
#include <QPainter>
#include <QScreen>
#include <Wm/desktopwm.h>
#include <tvariantanimation.h>

#include <tsettings.h>

#include <gatewaymanager.h>
#include <statemanager.h>

struct GatewayPrivate {
        Gateway* instance = nullptr;

        tVariantAnimation* width;

        GestureInteractionPtr lastGesture;
};

GatewayPrivate* Gateway::d = new GatewayPrivate();

Gateway::Gateway() :
    QDialog(nullptr),
    ui(new Ui::Gateway) {
    ui->setupUi(this);

    ui->gatewayContainer->move(0, 0);
    connect(ui->gatewayContainer, &MainGatewayWidget::closeGateway, this, [=] {
        this->close();
    });

    d->width = new tVariantAnimation();
    d->width->setDuration(500);
    connect(d->width, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        this->setFixedWidth(value.toInt());
        QScreen* screen = qApp->primaryScreen();
        QRect geometry;
        geometry.setHeight(this->height());
        geometry.setWidth(value.toInt());
        if (this->layoutDirection() == Qt::RightToLeft) {
            geometry.moveTopRight(screen->geometry().topRight());
        } else {
            geometry.moveTopLeft(screen->geometry().topLeft());
        }
        this->setGeometry(geometry);
    });
    connect(d->width, &tVariantAnimation::finished, this, [=] {
        if (d->lastGesture && d->lastGesture->isActive()) return;
        if (this->geometry().width() == 0) {
            QDialog::hide();
            ui->gatewayContainer->clearState();
        }
    });

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    DesktopWm::setSystemWindow(this, DesktopWm::SystemWindowTypeMenu);
    this->setFixedWidth(0);

    connect(GestureDaemon::instance(), &GestureDaemon::gestureBegin, this, [=](GestureInteractionPtr interaction) {
        if (interaction->isValidInteraction(GestureTypes::Swipe, this->layoutDirection() == Qt::RightToLeft ? GestureTypes::Left : GestureTypes::Right, 3)) {
            trackGatewayOpenGesture(interaction);
        } else if (interaction->isValidInteraction(GestureTypes::Swipe, this->layoutDirection() == Qt::RightToLeft ? GestureTypes::Right : GestureTypes::Left, 3)) {
            trackGatewayCloseGesture(interaction);
        }
    });

    ui->line->raise();
}

void Gateway::resizeEvent(QResizeEvent* event) {
    ui->gatewayContainer->setFixedSize(ui->gatewayContainer->sizeHint().width(), this->height());

    if (this->layoutDirection() == Qt::RightToLeft) {
        ui->gatewayContainer->move(1, 0);
        ui->line->setGeometry(0, 0, 1, this->height());
    } else {
        ui->gatewayContainer->move(this->geometry().width() - ui->gatewayContainer->width() - 1, 0);
        ui->line->setGeometry(this->geometry().width() - 1, 0, 1, this->height());
    }
    StateManager::gatewayManager()->setGatewayWidth(this->geometry().width());
}

void Gateway::changeEvent(QEvent* event) {
    QDialog::changeEvent(event);
    switch (event->type()) {
        case QEvent::ActivationChange:
            if (!this->isActiveWindow()) this->close();
            break;
        default:;
    }
}

void Gateway::trackGatewayOpenGesture(GestureInteractionPtr gesture) {
    if (this->isVisible()) return;

    // Capture this gesture!
    d->lastGesture = gesture;

    QScreen* screen = qApp->primaryScreen();
    this->setFixedHeight(screen->geometry().height());
    d->width->setEasingCurve(QEasingCurve::Linear);
    d->width->setCurrentTime(0);
    d->width->setStartValue(0);
    d->width->setEndValue(ui->gatewayContainer->sizeHint().width() + 1);

    connect(gesture.data(), &GestureInteraction::interactionUpdated, this, [=] {
        d->width->setCurrentTime(d->width->totalDuration() * gesture->percentage());
        d->width->valueChanged(d->width->currentValue());
    });
    connect(gesture.data(), &GestureInteraction::interactionEnded, this, [=] {
        if (gesture->extrapolatePercentage(100) > 0.3) {
            show();
        } else {
            close();
        }
    });

    QDialog::show();
}

void Gateway::trackGatewayCloseGesture(GestureInteractionPtr gesture) {
    if (!this->isVisible()) return;

    // Capture this gesture!
    d->lastGesture = gesture;

    QScreen* screen = qApp->primaryScreen();
    this->setFixedHeight(screen->geometry().height());
    d->width->setEasingCurve(QEasingCurve::Linear);
    d->width->setCurrentTime(0);
    d->width->setStartValue(ui->gatewayContainer->sizeHint().width() + 1);
    d->width->setEndValue(0);

    connect(gesture.data(), &GestureInteraction::interactionUpdated, this, [=] {
        d->width->setCurrentTime(d->width->totalDuration() * gesture->percentage());
        d->width->valueChanged(d->width->currentValue());
    });
    connect(gesture.data(), &GestureInteraction::interactionEnded, this, [=] {
        if (gesture->extrapolatePercentage(100) > 0.3) {
            close();
        } else {
            show();
        }
    });
}

Gateway::~Gateway() {
    delete ui;
}

Gateway* Gateway::instance() {
    if (!d->instance) d->instance = new Gateway();
    return d->instance;
}

void Gateway::show() {
    QScreen* screen = qApp->primaryScreen();
    this->setFixedHeight(screen->geometry().height());

    d->width->setEasingCurve(QEasingCurve::OutCubic);
    d->width->setStartValue(this->geometry().width());
    d->width->setEndValue(ui->gatewayContainer->sizeHint().width() + 1);
    d->width->stop();
    d->width->start();

    QDialog::show();
    QDialog::activateWindow();
    ui->gatewayContainer->setFocus();
}

void Gateway::close() {
    d->width->setEasingCurve(QEasingCurve::OutCubic);
    d->width->setStartValue(this->geometry().width());
    d->width->setEndValue(0);
    d->width->stop();
    d->width->start();
}
