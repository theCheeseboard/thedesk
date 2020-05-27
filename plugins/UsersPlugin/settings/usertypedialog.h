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
#ifndef USERTYPEDIALOG_H
#define USERTYPEDIALOG_H

#include <QWidget>
#include "user.h"

namespace Ui {
    class UserTypeDialog;
}

struct UserTypeDialogPrivate;
class UserTypeDialog : public QWidget
{
        Q_OBJECT

    public:
        explicit UserTypeDialog(UserPtr user, QWidget *parent = nullptr);
        ~UserTypeDialog();

    private slots:
        void on_backButton_clicked();

        void on_standardUserButton_clicked();

        void on_administratorButton_clicked();

    signals:
        void done();

    private:
        Ui::UserTypeDialog *ui;
        UserTypeDialogPrivate* d;

        void setUserType(User::UserType type);
};

#endif // USERTYPEDIALOG_H
