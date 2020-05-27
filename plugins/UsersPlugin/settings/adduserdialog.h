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
#ifndef ADDUSERDIALOG_H
#define ADDUSERDIALOG_H

#include <QWidget>

namespace Ui {
    class AddUserDialog;
}

struct AddUserDialogPrivate;
class AddUserDialog : public QWidget
{
        Q_OBJECT

    public:
        explicit AddUserDialog(QWidget *parent = nullptr);
        ~AddUserDialog();

    private slots:
        void on_backButton_clicked();

        void on_backButton_2_clicked();

        void on_nextButton_clicked();

        void on_fullNameLineEdit_textChanged(const QString &arg1);

        void on_usernameLineEdit_textChanged(const QString &arg1);

        void on_backButton_3_clicked();

        void on_usePasswordButton_clicked();

        void on_askForPasswordButton_clicked();

        void on_noPasswordButton_clicked();

        void on_performAddUserButton_clicked();

        void on_administratorButton_clicked();

        void on_standardUserButton_clicked();

    signals:
        void done();

    private:
        Ui::AddUserDialog *ui;
        AddUserDialogPrivate* d;
};

#endif // ADDUSERDIALOG_H
