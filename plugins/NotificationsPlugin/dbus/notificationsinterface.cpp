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
#include "notificationsinterface.h"

#include <QDBusConnection>
#include "notification.h"
#include "notificationtracker.h"
#include "notifications_adaptor.h"

struct NotificationsInterfacePrivate {
    NotificationTracker* tracker;
};

NotificationsInterface::NotificationsInterface(NotificationTracker* tracker, QObject* parent) : QObject(parent) {
    new NotificationsAdaptor(this);
    QDBusConnection::sessionBus().registerService("org.freedesktop.Notifications");
    QDBusConnection::sessionBus().registerObject("/org/freedesktop/Notifications", this);

    d = new NotificationsInterfacePrivate();
    d->tracker = tracker;
    connect(tracker, &NotificationTracker::destroyed, this, &NotificationsInterface::deleteLater);
}

NotificationsInterface::~NotificationsInterface() {
    delete d;
    QDBusConnection::sessionBus().unregisterObject("/org/freedesktop/Notifications");
    QDBusConnection::sessionBus().unregisterService("org.freedesktop.Notifications");
}

QStringList NotificationsInterface::GetCapabilities() {
    return {"body"};
}

quint32 NotificationsInterface::Notify(QString appName, quint32 replacesId, QString appIcon, QString summary, QString body, QStringList actions, QVariantMap hints, qint32 expireTimeout) {
    NotificationPtr notification = d->tracker->get(replacesId);
    if (!notification) {
        notification = d->tracker->createNotification();
    }

    notification->setBody(body);
    notification->setSummary(summary);
    notification->setTimeout(expireTimeout);

    if (hints.contains("desktop-entry") && Application::allApplications().contains(hints.value("desktop-entry").toString())) {
        notification->setApplication(ApplicationPointer(new Application(hints.value("desktop-entry").toString())));
    } else {
        notification->setApplication(ApplicationPointer(new Application({
            {"Icon", "generic-app"},
            {"Name", appName}
        })));
    }

    return notification->id();
}

void NotificationsInterface::CloseNotification(quint32 id) {

}

QString NotificationsInterface::GetServerInformation(QString& vendor, QString& version, QString& specVersion) {
    vendor = "theSuite";
    version = "1.0";
    specVersion = "1.2";
    return "theDesk";
}
