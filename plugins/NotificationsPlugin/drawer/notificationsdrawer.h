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
#ifndef NOTIFICATIONSDRAWER_H
#define NOTIFICATIONSDRAWER_H

#include <QDialog>
#include "notification.h"

namespace Ui {
    class NotificationsDrawer;
}

class NotificationTracker;
struct NotificationsDrawerPrivate;
class NotificationsDrawer : public QDialog {
        Q_OBJECT

    public:
        explicit NotificationsDrawer(NotificationTracker* tracker);
        ~NotificationsDrawer();

    private:
        Ui::NotificationsDrawer* ui;
        NotificationsDrawerPrivate* d;

        bool eventFilter(QObject* watched, QEvent* event);

        void updateGeometry();
        void showNotification(NotificationPtr notification);
};

#endif // NOTIFICATIONSDRAWER_H
