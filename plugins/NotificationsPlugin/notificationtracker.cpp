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
#include "notificationtracker.h"

#include <QPointer>
#include <QMap>
#include <QTimer>
#include "notification.h"

struct NotificationTrackerPrivate {
    quint32 lastNotification = 1;
    QMap<quint32, NotificationPtr> notifications;
};

NotificationTracker::NotificationTracker(QObject* parent) : QObject(parent) {
    d = new NotificationTrackerPrivate();
}

NotificationTracker::~NotificationTracker() {
    delete d;
}

NotificationPtr NotificationTracker::createNotification() {
    quint32 notificationId = d->lastNotification;
    NotificationPtr n(new Notification(notificationId));
    connect(n.data(), &Notification::dismissed, this, [ = ] {
        d->notifications.remove(notificationId);
    });
    d->notifications.insert(notificationId, n);
    d->lastNotification++;

    QTimer::singleShot(0, this, std::bind(&NotificationTracker::newNotification, this, n));

    return n;
}

NotificationPtr NotificationTracker::get(quint32 id) {
    if (id == 0) return this->createNotification();
    return d->notifications.value(id);
}
