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
#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>
#include "notification.h"

namespace Ui {
    class NotificationWidget;
}

struct NotificationWidgetPrivate;
class NotificationWidget : public QWidget {
        Q_OBJECT

    public:
        explicit NotificationWidget(NotificationPtr notification, QWidget* parent = nullptr);
        ~NotificationWidget();

        void setIsLast(bool isLast);

    private:
        Ui::NotificationWidget* ui;
        NotificationWidgetPrivate* d;

        void resizeEvent(QResizeEvent* event);
        void enterEvent(QEvent* event);
        void leaveEvent(QEvent* event);
};

#endif // NOTIFICATIONWIDGET_H
