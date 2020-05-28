/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
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
#ifndef USERSPANE_H
#define USERSPANE_H

#include <QWidget>
#include <tpromise.h>
#include <statuscenterpane.h>

namespace Ui {
    class UsersPane;
}

struct UsersPanePrivate;
class UsersPane : public StatusCenterPane {
        Q_OBJECT

    public:
        explicit UsersPane();
        ~UsersPane();

    private slots:
        void on_mainMenuButton_clicked();

        void on_addButton_clicked();

        void on_deleteUserButton_clicked();

        void on_changePasswordButton_clicked();

        void on_changeUserTypeButton_clicked();

        void on_changeRealNameButton_clicked();

        void on_lockUserButton_clicked();

        void on_usernameTitleLabel_backButtonClicked();

    private:
        Ui::UsersPane* ui;
        UsersPanePrivate* d;

        void changeEvent(QEvent* event);

        tPromise<void>* checkPolkit(bool isOwnUser);
        void currentUserChanged();

        // StatusCenterPane interface
    public:
        QString name();
        QString displayName();
        QIcon icon();
        QWidget* leftPane();

};

#endif // USERSPANE_H
