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
#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QPointer>
#include <QIcon>
#include <Applications/application.h>

class NotificationTracker;
struct NotificationPrivate;
class Notification : public QObject {
        Q_OBJECT
    public:
        enum NotificationCloseReason : quint32 {
            NotificationExpired = 1,
            NotificationUserDismissed = 2,
            NotificationClosedByDBus = 3,
            NotificationCloseReasonUndefined = 4
        };

        enum Urgency {
            Low = 0,
            Normal = 1,
            Critical = 2
        };

        struct Action {
            QString text;
            QString identifier;
            QIcon icon;
        };

        ~Notification();

        quint32 id();

        void setSummary(QString summary);
        QString summary();

        void setBody(QString body);
        QString body();

        void setTimeout(qint32 timeout);
        qint32 timeout();

        void setApplication(ApplicationPointer application);
        ApplicationPointer application();

        void setActions(QList<Action> actions);
        QList<Action> actions();

        void setUrgency(Urgency urgency);
        Urgency urgency();

        void dismiss(NotificationCloseReason reason);

    signals:
        void summaryChanged(QString summary);
        void bodyChanged(QString body);
        void timeoutChanged(qint32 timeout);
        void applicationChanged(ApplicationPointer application);
        void actionsChanged(QList<Action> actions);
        void dismissed(NotificationCloseReason reason);
        void actionInvoked(Action action);

    protected:
        friend NotificationTracker;
        explicit Notification(quint32 id);

    private:
        NotificationPrivate* d;
};

typedef QPointer<Notification> NotificationPtr;

#endif // NOTIFICATION_H
