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
#ifndef NOTIFICATIONAPPGROUP_H
#define NOTIFICATIONAPPGROUP_H

#include <QWidget>
#include <application.h>
#include "notification.h"

namespace Ui {
    class NotificationAppGroup;
}

struct NotificationAppGroupPrivate;
class NotificationAppGroup : public QWidget {
        Q_OBJECT

    public:
        explicit NotificationAppGroup(ApplicationPointer application, QWidget* parent = nullptr);
        ~NotificationAppGroup();

        void pushNotification(NotificationPtr notification);

    private slots:
        void on_dismissAllButton_clicked();

    private:
        Ui::NotificationAppGroup* ui;
        NotificationAppGroupPrivate* d;

        void setOrdering();
};

#endif // NOTIFICATIONAPPGROUP_H
