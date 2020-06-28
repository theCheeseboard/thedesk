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
    return {"body", "actions", "action-icons", "body-markup"};
}

quint32 NotificationsInterface::Notify(QString appName, quint32 replacesId, QString appIcon, QString summary, QString body, QStringList actions, QVariantMap hints, qint32 expireTimeout) {
    NotificationPtr notification = d->tracker->get(replacesId);
    if (!notification) {
        notification = d->tracker->createNotification();
        connect(notification.data(), &Notification::dismissed, this, [ = ](Notification::NotificationCloseReason closeReason) {
            emit NotificationClosed(notification->id(), closeReason);
        });
        connect(notification.data(), &Notification::actionInvoked, this, [ = ](Notification::Action action) {
            emit ActionInvoked(notification->id(), action.identifier);
        });
    }

    notification->setBody(body);
    notification->setSummary(summary);
    notification->setTimeout(expireTimeout);

    if (actions.count() % 2 == 0) {
        QList<Notification::Action> actionList;
        for (int i = 0; i < actions.count(); i += 2) {
            Notification::Action action;
            action.identifier = actions.at(i);
            action.text = actions.at(i + 1);
            if (hints.value("action-icons", false).toBool()) action.icon = QIcon::fromTheme(action.identifier);
            actionList.append(action);
        }
        notification->setActions(actionList);
    }

    if (hints.contains("desktop-entry") && Application::allApplications().contains(hints.value("desktop-entry").toString())) {
        notification->setApplication(ApplicationPointer(new Application(hints.value("desktop-entry").toString())));
    } else {
        notification->setApplication(ApplicationPointer(new Application({
            {"Icon", "generic-app"},
            {"Name", appName}
        })));
    }

    if (hints.contains("urgency")) {
        notification->setUrgency(static_cast<Notification::Urgency>(hints.value("urgency").toInt()));
    }

    return notification->id();
}

void NotificationsInterface::CloseNotification(quint32 id) {
    NotificationPtr notification = d->tracker->get(id);
    if (notification) {
        notification->dismiss(Notification::NotificationClosedByDBus);
    }
}

QString NotificationsInterface::GetServerInformation(QString& vendor, QString& version, QString& specVersion) {
    vendor = "theSuite";
    version = "1.0";
    specVersion = "1.2";
    return "theDesk";
}
