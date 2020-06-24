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
#include "notificationwidget.h"
#include "ui_notificationwidget.h"

#include <QToolButton>

struct NotificationWidgetPrivate {
    NotificationPtr notification;
    QToolButton* dismissButton;
};

NotificationWidget::NotificationWidget(NotificationPtr notification, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NotificationWidget) {
    ui->setupUi(this);

    d = new NotificationWidgetPrivate();
    d->notification = notification;

    connect(notification, &Notification::dismissed, this, &NotificationWidget::deleteLater);
    connect(notification, &Notification::summaryChanged, this, [ = ](QString summary) {
        ui->summaryLabel->setText(summary);
    });
    connect(notification, &Notification::bodyChanged, this, [ = ](QString body) {
        ui->bodyLabel->setText(body);
    });
    ui->summaryLabel->setText(notification->summary());
    ui->bodyLabel->setText(notification->body());

    d->dismissButton = new QToolButton(this);
    d->dismissButton->setIcon(QIcon::fromTheme("window-close"));
    d->dismissButton->setFixedSize(d->dismissButton->sizeHint());
    connect(d->dismissButton, &QToolButton::clicked, this, [ = ] {
        notification->dismiss(Notification::NotificationUserDismissed);
    });
    d->dismissButton->hide();
}

NotificationWidget::~NotificationWidget() {
    delete d;
    delete ui;
}

void NotificationWidget::setIsLast(bool isLast) {
    ui->line->setVisible(!isLast);
}

void NotificationWidget::resizeEvent(QResizeEvent* event) {
    d->dismissButton->move(this->width() - d->dismissButton->width(), 0);
}

void NotificationWidget::enterEvent(QEvent* event) {
    d->dismissButton->show();
}

void NotificationWidget::leaveEvent(QEvent* event) {
    d->dismissButton->hide();
}
