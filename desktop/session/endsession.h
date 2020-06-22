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
#ifndef ENDSESSION_H
#define ENDSESSION_H

#include <QWidget>
#include <tpromise.h>
#include <powermanager.h>

namespace Ui {
    class EndSession;
}

struct EndSessionPrivate;
class EndSession : public QWidget {
        Q_OBJECT

    public:
        ~EndSession();

        static tPromise<void>* showDialog(PowerManager::PowerOperation operation = PowerManager::All, QString message = "");

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_powerOffButton_clicked();

        void on_rebootButton_clicked();

        void on_logoutButton_clicked();

        void on_suspendButton_clicked();

        void on_lockButton_clicked();

        void on_screenOffButton_clicked();

        void on_switchUsersButton_clicked();

        void on_hibernateButton_clicked();

        void on_updatesAvailableTitle_backButtonClicked();

        void on_rebootNoUpdateButton_clicked();

        void on_rebootUpdateButton_clicked();

        void on_stackedWidget_switchingFrame(int frame);

    private:
        explicit EndSession(PowerManager::PowerOperation operation, QString message, QWidget* parent = nullptr);

        Ui::EndSession* ui;
        EndSessionPrivate* d;
};

#endif // ENDSESSION_H
