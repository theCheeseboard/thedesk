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
#ifndef NOTIFICATIONTRACKER_H
#define NOTIFICATIONTRACKER_H

#include <QObject>
#include <QPointer>

class Notification;
typedef QPointer<Notification> NotificationPtr;

struct NotificationTrackerPrivate;
class NotificationTracker : public QObject {
        Q_OBJECT
    public:
        explicit NotificationTracker(QObject* parent = nullptr);
        ~NotificationTracker();

        NotificationPtr createNotification();
        NotificationPtr get(quint32 id);

    signals:
        void newNotification(NotificationPtr notification);

    private:
        NotificationTrackerPrivate* d;
};

#endif // NOTIFICATIONTRACKER_H
