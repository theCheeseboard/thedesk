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
#include "notificationsdrawerwidget.h"
#include "ui_notificationsdrawerwidget.h"

#include <tvariantanimation.h>
#include <QIcon>
#include <QGraphicsOpacityEffect>
#include "notification.h"

struct NotificationsDrawerWidgetPrivate {
    tVariantAnimation* timeout;
    NotificationPtr n;

    QGraphicsOpacityEffect* effect;
};

NotificationsDrawerWidget::NotificationsDrawerWidget(NotificationPtr notification, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NotificationsDrawerWidget) {
    ui->setupUi(this);

    d = new NotificationsDrawerWidgetPrivate();
    d->n = notification;

    ui->summaryLabel->setText(notification->summary());
    ui->bodyLabel->setText(notification->body());
    connect(notification, &Notification::summaryChanged, ui->summaryLabel, &QLabel::setText);
    connect(notification, &Notification::bodyChanged, ui->bodyLabel, &QLabel::setText);

    d->timeout = new tVariantAnimation(this);
    d->timeout->setStartValue(1.0);
    d->timeout->setEndValue(0.0);
    d->timeout->setEasingCurve(QEasingCurve::Linear);
    d->timeout->setDuration(notification->timeout());
    connect(d->timeout, &tVariantAnimation::finished, this, &NotificationsDrawerWidget::animateDismiss);
    connect(notification, &Notification::timeoutChanged, this, [ = ](qint32 timeout) {
        if (timeout == 0) {
            d->timeout->setDuration(-1);
        } else {
            d->timeout->setDuration(timeout);
        }
    });

    ui->appIconLabel->setPixmap(QIcon::fromTheme(notification->application()->getProperty("Icon").toString()).pixmap(SC_DPI_T(QSize(16, 16), QSize)));
    ui->appNameLabel->setText(notification->application()->getProperty("Name").toString());
    connect(notification, &Notification::applicationChanged, this, [ = ] {
        ui->appIconLabel->setPixmap(QIcon::fromTheme(notification->application()->getProperty("Icon").toString()).pixmap(SC_DPI_T(QSize(16, 16), QSize)));
        ui->appNameLabel->setText(notification->application()->getProperty("Name").toString());
    });

    d->effect = new QGraphicsOpacityEffect(this);
    d->effect->setEnabled(false);
    this->setGraphicsEffect(d->effect);
}

NotificationsDrawerWidget::~NotificationsDrawerWidget() {
    delete ui;
    delete d;
}

void NotificationsDrawerWidget::show() {
    QWidget::show();

    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(-ui->mainFrame->width());
    anim->setEndValue(SC_DPI(9));
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->setDuration(250);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        ui->mainFrame->move(value.toInt(), SC_DPI(9));
    });
    connect(anim, &tVariantAnimation::finished, this, [ = ] {
        d->timeout->start();
    });
    anim->start();
}

void NotificationsDrawerWidget::animateDismiss() {
    tVariantAnimation* opacityAnim = new tVariantAnimation(this);
    opacityAnim->setStartValue(1.0);
    opacityAnim->setEndValue(0.0);
    opacityAnim->setEasingCurve(QEasingCurve::OutCubic);
    opacityAnim->setDuration(250);
    connect(opacityAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        d->effect->setOpacity(value.toDouble());
    });
    connect(opacityAnim, &tVariantAnimation::finished, this, [ = ] {
        opacityAnim->deleteLater();

        tVariantAnimation* anim = new tVariantAnimation(this);
        anim->setStartValue(this->height());
        anim->setEndValue(0);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->setDuration(250);
        connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
            this->setFixedHeight(value.toInt());
        });
        connect(anim, &tVariantAnimation::finished, this, [ = ] {
            anim->deleteLater();
            emit dismiss();
        });
        anim->start();
    });
    d->effect->setEnabled(true);
    opacityAnim->start();
}

QSize NotificationsDrawerWidget::sizeHint() const {
    QSize sizeHint = QWidget::sizeHint();
    sizeHint.setHeight(ui->mainFrame->sizeHint().height() + SC_DPI(9));
    return sizeHint;
}

void NotificationsDrawerWidget::resizeEvent(QResizeEvent* event) {
    ui->mainFrame->setFixedWidth(this->width() - SC_DPI(18));
    ui->mainFrame->setFixedHeight(ui->mainFrame->sizeHint().height());
}
