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
#ifndef NOTIFICATIONSSTATUSCENTERPANE_H
#define NOTIFICATIONSSTATUSCENTERPANE_H

#include <Applications/application.h>
#include <SystemJob/systemjob.h>
#include <statuscenterpane.h>

namespace Ui {
    class NotificationsStatusCenterPane;
}

class NotificationAppGroup;
class SystemJobController;
class NotificationTracker;
struct NotificationsStatusCenterPanePrivate;
class NotificationsStatusCenterPane : public StatusCenterPane {
        Q_OBJECT

    public:
        explicit NotificationsStatusCenterPane(NotificationTracker* tracker, SystemJobController* jobController);
        ~NotificationsStatusCenterPane();

    private slots:
        void on_titleLabel_backButtonClicked();

    private:
        Ui::NotificationsStatusCenterPane* ui;

        NotificationsStatusCenterPanePrivate* d;

        NotificationAppGroup* appGroupForDesktopEntry(QString desktopEntry, ApplicationPointer application);
        void registerJob(SystemJobPtr job);

        // StatusCenterPane interface
    public:
        QString name();
        QString displayName();
        QIcon icon();
        QWidget* leftPane();

        // QWidget interface
    protected:
        void changeEvent(QEvent* event);
};

#endif // NOTIFICATIONSSTATUSCENTERPANE_H
