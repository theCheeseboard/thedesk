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
#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include "user.h"
#include <QWidget>

namespace Ui {
    class ChangePasswordDialog;
}

struct ChangePasswordDialogPrivate;
class ChangePasswordDialog : public QWidget {
        Q_OBJECT

    public:
        explicit ChangePasswordDialog(UserPtr user, QWidget* parent = nullptr);
        ~ChangePasswordDialog();

    private slots:
        QCoro::Task<> on_setPasswordButton_clicked();

        QCoro::Task<> on_askForPasswordButton_clicked();

        QCoro::Task<> on_noPasswordButton_clicked();

        void on_titleLabel_backButtonClicked();

    signals:
        void done();

    private:
        Ui::ChangePasswordDialog* ui;
        ChangePasswordDialogPrivate* d;
};

#endif // CHANGEPASSWORDDIALOG_H
