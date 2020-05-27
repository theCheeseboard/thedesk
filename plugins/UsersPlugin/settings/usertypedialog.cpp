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
#include "usertypedialog.h"
#include "ui_usertypedialog.h"

#include <ttoast.h>

struct UserTypeDialogPrivate {
    UserPtr user;
};

UserTypeDialog::UserTypeDialog(UserPtr user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserTypeDialog)
{
    ui->setupUi(this);
    d = new UserTypeDialogPrivate();
    d->user = user;

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);

    if (user->userType() == User::Administrator) {
        ui->administratorButton->setChecked(true);
    } else {
        ui->standardUserButton->setChecked(true);
    }
}

UserTypeDialog::~UserTypeDialog()
{
    delete d;
    delete ui;
}

void UserTypeDialog::on_backButton_clicked()
{
    emit done();
}

void UserTypeDialog::on_standardUserButton_clicked()
{
    //Set the user type
    this->setUserType(User::StandardUser);
}

void UserTypeDialog::on_administratorButton_clicked()
{
    //Set the user type
    this->setUserType(User::Administrator);
}

void UserTypeDialog::setUserType(User::UserType type)
{
    if (d->user->userType() == type) {
        emit done();
        return;
    }

    //Set the user's account type
    ui->stackedWidget->setCurrentWidget(ui->processingPage);

    d->user->setUserType(type)->then([=] {
        emit done();
    })->error([=](QString err) {
        //Bail out
        QTimer::singleShot(1000, [=] {
            ui->stackedWidget->setCurrentWidget(ui->setAccountTypePage);

            tToast* toast = new tToast();
            toast->setTitle(tr("Couldn't set user type"));
            toast->setText(err);
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        });
    });
}
