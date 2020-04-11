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
#ifndef NOTIFICATIONSINTERFACE_H
#define NOTIFICATIONSINTERFACE_H

#include <QObject>
#include <QVariantMap>

class NotificationTracker;
struct NotificationsInterfacePrivate;
class NotificationsInterface : public QObject {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

    public:
        explicit NotificationsInterface(NotificationTracker* tracker, QObject* parent = nullptr);
        ~NotificationsInterface();

    public Q_SLOTS:
        Q_SCRIPTABLE QStringList GetCapabilities();
        Q_SCRIPTABLE quint32 Notify(QString appName, quint32 replacesId, QString appIcon, QString summary, QString body, QStringList actions, QVariantMap hints, qint32 expireTimeout);
        Q_SCRIPTABLE void CloseNotification(quint32 id);
        Q_SCRIPTABLE QString GetServerInformation(QString& vendor, QString& version, QString& specVersion);

    signals:
        Q_SCRIPTABLE void NotificationClosed(quint32 id, quint32 reason);
        Q_SCRIPTABLE void ActionInvoked(quint32 id, QString actionKey);

    private:
        NotificationsInterfacePrivate* d;
};

#endif // NOTIFICATIONSINTERFACE_H
