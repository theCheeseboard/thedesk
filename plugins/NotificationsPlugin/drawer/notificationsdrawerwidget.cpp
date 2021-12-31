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
#include <QPushButton>
#include "notificationtracker.h"
#include "notification.h"

struct NotificationsDrawerWidgetPrivate {
    NotificationTracker* tracker;
    tVariantAnimation* timeout;
    tVariantAnimation* actionsWidgetHeight;
    NotificationPtr n;

    QList<QPushButton*> actions;

    QGraphicsOpacityEffect* effect;
    bool shouldTimeoutRun = false;
};

NotificationsDrawerWidget::NotificationsDrawerWidget(NotificationPtr notification, NotificationTracker* tracker, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NotificationsDrawerWidget) {
    ui->setupUi(this);

    d = new NotificationsDrawerWidgetPrivate();
    d->n = notification;
    d->tracker = tracker;

    ui->summaryLabel->setText(notification->summary());
    ui->bodyLabel->setText(notification->body());
    connect(notification, &Notification::summaryChanged, ui->summaryLabel, &QLabel::setText);
    connect(notification, &Notification::bodyChanged, ui->bodyLabel, &QLabel::setText);

    connect(notification, &Notification::dismissed, this, &NotificationsDrawerWidget::animateDismiss);

    d->timeout = new tVariantAnimation(this);
    d->timeout->setStartValue(1.0);
    d->timeout->setEndValue(0.0);
    d->timeout->setEasingCurve(QEasingCurve::Linear);
    d->timeout->setForceAnimation(true);
    if (notification->timeout() == 0) {
        d->timeout->setDuration(-1);
    } else {
        d->timeout->setDuration(notification->timeout());
    }
    connect(d->timeout, &tVariantAnimation::finished, this, &NotificationsDrawerWidget::animateDismiss);
    connect(d->tracker, &NotificationTracker::pauseTimeouts, this, [ = ] {
        d->timeout->pause();
    });
    connect(d->tracker, &NotificationTracker::resumeTimeouts, this, [ = ] {
        if (d->shouldTimeoutRun && d->n->timeout() != 0) {
            d->timeout->start();
        }
    });

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

    ui->actionsWidget->setFixedHeight(0);
    connect(notification, &Notification::actionsChanged, this, &NotificationsDrawerWidget::setupActions);
    setupActions();

    d->actionsWidgetHeight = new tVariantAnimation(this);
    d->actionsWidgetHeight->setDuration(250);
    d->actionsWidgetHeight->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->actionsWidgetHeight, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        ui->actionsWidget->setFixedHeight(value.toInt());
        ui->mainFrame->setFixedHeight(ui->mainFrame->sizeHint().height());
        this->updateGeometry();
    });
    connect(d->actionsWidgetHeight, &tVariantAnimation::finished, this, [ = ] {
        ui->mainFrame->setFixedHeight(ui->mainFrame->sizeHint().height());
        this->updateGeometry();
    });

    ui->buttonBox->setParent(ui->mainFrame);
    ui->buttonBox->move(ui->mainFrame->width() - ui->buttonBox->width(), 0);
    ui->buttonBox->setVisible(false);

    ui->mainFrame->installEventFilter(this);

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
        d->shouldTimeoutRun = true;
        if (d->n->timeout() != 0) {
            d->timeout->start();
        }
    });
    anim->start();
}

void NotificationsDrawerWidget::animateDismiss() {
    d->shouldTimeoutRun = false;

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
    ui->buttonBox->move(ui->mainFrame->width() - ui->buttonBox->width(), 0);
}

bool NotificationsDrawerWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->mainFrame) {
        switch (event->type()) {
            case QEvent::Enter:
                showButtons();
                break;
            case QEvent::Leave:
                hideButtons();
                break;
            default:
                break;
        }
    }
    return false;
}

void NotificationsDrawerWidget::setupActions() {
    for (QPushButton* button : d->actions) {
        ui->actionsLayout->removeWidget(button);
        button->deleteLater();
    }
    d->actions.clear();

    for (Notification::Action action : d->n->actions()) {
        QPushButton* button = new QPushButton();
        button->setText(action.text);
        button->setIcon(action.icon);
        connect(button, &QPushButton::clicked, this, [ = ] {
            emit d->n->actionInvoked(action);
            this->animateDismiss();
        });
        ui->actionsLayout->addWidget(button);
        d->actions.append(button);
    }
}

void NotificationsDrawerWidget::showButtons() {
    d->tracker->pauseTimeouts();
    ui->buttonBox->setVisible(true);

    d->actionsWidgetHeight->stop();
    d->actionsWidgetHeight->setStartValue(ui->actionsWidget->height());
    d->actionsWidgetHeight->setEndValue(ui->actionsWidget->sizeHint().height());
    d->actionsWidgetHeight->start();
}

void NotificationsDrawerWidget::hideButtons() {
    d->tracker->resumeTimeouts();
    ui->buttonBox->setVisible(false);

    d->actionsWidgetHeight->stop();
    d->actionsWidgetHeight->setStartValue(ui->actionsWidget->height());
    d->actionsWidgetHeight->setEndValue(0);
    d->actionsWidgetHeight->start();
}

void NotificationsDrawerWidget::on_closeButton_clicked() {
    d->n->dismiss(Notification::NotificationUserDismissed);
}

void NotificationsDrawerWidget::on_dismissButton_clicked() {
    this->animateDismiss();
}
