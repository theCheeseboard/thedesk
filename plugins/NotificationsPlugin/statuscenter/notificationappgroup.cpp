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
#include "notificationappgroup.h"
#include "ui_notificationappgroup.h"

#include <QIcon>
#include <QLabel>
#include <tvariantanimation.h>

#include "jobwidget.h"
#include "notificationwidget.h"

struct NotificationAppGroupPrivate {
        ApplicationPointer application;
        QMap<NotificationPtr, NotificationWidget*> widgets;
        QList<NotificationWidget*> ordering;
        QList<JobWidget*> jobOrdering;
};

NotificationAppGroup::NotificationAppGroup(ApplicationPointer application, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NotificationAppGroup) {
    ui->setupUi(this);

    d = new NotificationAppGroupPrivate();
    d->application = application;

    if (d->application) {
        ui->iconLabel->setPixmap(QIcon::fromTheme(application->getProperty("Icon").toString()).pixmap(QSize(16, 16)));
        ui->appNameLabel->setText(application->getProperty("Name").toString());
    } else {
        ui->iconLabel->setPixmap(QIcon::fromTheme("generic-app").pixmap(QSize(16, 16)));
        ui->appNameLabel->setText(tr("Uncategorised"));
    }
}

NotificationAppGroup::~NotificationAppGroup() {
    delete d;
    delete ui;
}

void NotificationAppGroup::pushNotification(NotificationPtr notification) {
    NotificationWidget* w = new NotificationWidget(notification);
    connect(
        w, &NotificationWidget::destroyed, this, [=] {
            d->widgets.remove(notification);
            d->ordering.removeOne(w);
            this->dismissOrReorder();
        },
        Qt::QueuedConnection);
    ui->notificationsLayout->insertWidget(0, w);
    d->widgets.insert(notification, w);
    d->ordering.insert(0, w);

    setOrdering();
}

void NotificationAppGroup::pushJob(SystemJobPtr job) {
    auto w = new JobWidget(job);
    connect(w, &JobWidget::destroyed, this, [this, w] {
        d->jobOrdering.removeOne(w);
        this->dismissOrReorder();
    });
    ui->jobsLayout->insertWidget(0, w);
    d->jobOrdering.insert(0, w);
    setOrdering();
}

void NotificationAppGroup::dismissAll() {
    for (NotificationPtr notification : d->widgets.keys()) {
        notification->dismiss(Notification::NotificationUserDismissed);
    }
}

void NotificationAppGroup::setOrdering() {
    ui->dismissAllButton->setVisible(!d->ordering.isEmpty());

    for (JobWidget* w : d->jobOrdering) {
        w->setIsLast(false);
    }
    if (d->ordering.isEmpty()) d->jobOrdering.constLast()->setIsLast(true);

    for (NotificationWidget* w : d->ordering) {
        w->setIsLast(false);
    }
    if (!d->ordering.isEmpty()) d->ordering.last()->setIsLast(true);
}

void NotificationAppGroup::dismissOrReorder() {
    if (d->ordering.isEmpty() && d->jobOrdering.isEmpty()) {
        this->deleteLater();
    } else {
        setOrdering();
    }
}

void NotificationAppGroup::on_dismissAllButton_clicked() {
    dismissAll();
}
