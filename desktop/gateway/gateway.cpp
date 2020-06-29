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

#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <tvariantanimation.h>
#include <Wm/desktopwm.h>

#include <tsettings.h>

#include <statemanager.h>
#include <gatewaymanager.h>

struct GatewayPrivate {
    Gateway* instance = nullptr;

    tVariantAnimation* width;
    tSettings settings;
};

GatewayPrivate* Gateway::d = new GatewayPrivate();

Gateway::Gateway() :
    QDialog(nullptr),
    ui(new Ui::Gateway) {
    ui->setupUi(this);

    ui->gatewayContainer->move(0, 0);
    connect(ui->gatewayContainer, &MainGatewayWidget::closeGateway, this, [ = ] {
        this->close();
    });

    d->width = new tVariantAnimation();
    d->width->setEasingCurve(QEasingCurve::OutCubic);
    d->width->setDuration(500);
    connect(d->width, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setFixedWidth(value.toInt());
    });
    connect(d->width, &tVariantAnimation::finished, this, [ = ] {
        if (this->width() == 0) {
            QDialog::hide();
            ui->gatewayContainer->clearState();
        }
    });

    DesktopWm::setSystemWindow(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setFixedWidth(0);

    ui->line->raise();
}

void Gateway::resizeEvent(QResizeEvent* event) {
    ui->gatewayContainer->setFixedSize(ui->gatewayContainer->sizeHint().width(), this->height());
    ui->gatewayContainer->move(this->width() - ui->gatewayContainer->width() - 1, 0);
    ui->line->setGeometry(this->width() - 1, 0, 1, this->height());
    StateManager::gatewayManager()->setGatewayWidth(this->width());
}

void Gateway::changeEvent(QEvent* event) {
    QDialog::changeEvent(event);
    switch (event->type()) {
        case QEvent::ActivationChange:
            if (!this->isActiveWindow()) this->close();
            break;
        default:
            ;
    }
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
    this->move(screen->geometry().topLeft() - QPoint(0, 1));

    d->width->setStartValue(this->width());
    d->width->setEndValue(ui->gatewayContainer->sizeHint().width() + 1);
    d->width->stop();
    d->width->start();

    QDialog::show();
    QDialog::activateWindow();
}

void Gateway::close() {
    d->width->setStartValue(this->width());
    d->width->setEndValue(0);
    d->width->stop();
    d->width->start();
}
