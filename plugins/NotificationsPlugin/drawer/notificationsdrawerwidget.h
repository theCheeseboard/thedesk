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
#ifndef NOTIFICATIONSDRAWERWIDGET_H
#define NOTIFICATIONSDRAWERWIDGET_H

#include <QWidget>

namespace Ui {
    class NotificationsDrawerWidget;
}

class Notification;
typedef QPointer<Notification> NotificationPtr;

struct NotificationsDrawerWidgetPrivate;
class NotificationTracker;
class NotificationsDrawerWidget : public QWidget {
        Q_OBJECT

    public:
        explicit NotificationsDrawerWidget(NotificationPtr notification, NotificationTracker* tracker, QWidget* parent = nullptr);
        ~NotificationsDrawerWidget();

        void show();
        void animateDismiss();

        QSize sizeHint() const;

    signals:
        void dismiss();

    private slots:
        void on_closeButton_clicked();

        void on_dismissButton_clicked();

    private:
        Ui::NotificationsDrawerWidget* ui;
        NotificationsDrawerWidgetPrivate* d;

        void resizeEvent(QResizeEvent* event);
        bool eventFilter(QObject* watched, QEvent* event);

        void setupActions();

        void showButtons();
        void hideButtons();
};

#endif // NOTIFICATIONSDRAWERWIDGET_H
