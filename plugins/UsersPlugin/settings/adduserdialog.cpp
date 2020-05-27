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
#include "adduserdialog.h"
#include "ui_adduserdialog.h"

#include "user.h"
#include <ttoast.h>
#include <terrorflash.h>

struct AddUserDialogPrivate {
    User::PasswordMode passwordMode;
};

AddUserDialog::AddUserDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddUserDialog)
{
    ui->setupUi(this);
    d = new AddUserDialogPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->standardUserButton->click();
}

AddUserDialog::~AddUserDialog()
{
    delete d;
    delete ui;
}

void AddUserDialog::on_backButton_clicked()
{
    emit done();
}

void AddUserDialog::on_backButton_2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->detailsPage);
}

void AddUserDialog::on_nextButton_clicked()
{
    if (ui->fullNameLineEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->fullNameLineEdit);
        return;
    }

    if (ui->usernameLineEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->fullNameLineEdit);
        return;
    }

    ui->stackedWidget->setCurrentWidget(ui->securityPage);
}

void AddUserDialog::on_fullNameLineEdit_textChanged(const QString &arg1)
{
    ui->usernameLineEdit->setText(arg1.split(" ").first().toLower());
    ui->realNameConfirmLabel->setText(arg1);
}

void AddUserDialog::on_usernameLineEdit_textChanged(const QString &arg1)
{
    ui->usernameLineEdit->setText(arg1.toLower());
    ui->userNameConfirmLabel->setText(arg1);
}

void AddUserDialog::on_backButton_3_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->securityPage);
}

void AddUserDialog::on_usePasswordButton_clicked()
{
    if (ui->passwordLineEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->passwordLineEdit);
        return;
    }

    if (ui->passwordLineEdit->text() != ui->confirmPasswordLineEdit->text()) {
        tErrorFlash::flashError(ui->confirmPasswordLineEdit);
        return;
    }

    d->passwordMode = User::SetPassword;
    ui->passwordConfirmLabel->setText(tr("Set Password"));
    ui->stackedWidget->setCurrentWidget(ui->confirmPage);
}

void AddUserDialog::on_askForPasswordButton_clicked()
{
    d->passwordMode = User::SetAtLogin;
    ui->passwordConfirmLabel->setText(tr("Ask when logging in"));
    ui->stackedWidget->setCurrentWidget(ui->confirmPage);
}

void AddUserDialog::on_noPasswordButton_clicked()
{
    d->passwordMode = User::NoPassword;
    ui->passwordConfirmLabel->setText(tr("Not Set"));
    ui->stackedWidget->setCurrentWidget(ui->confirmPage);
}

void AddUserDialog::on_performAddUserButton_clicked()
{
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->stackedWidget->setCurrentWidget(ui->processingPage);

    //Add the user account
    int accountType = ui->administratorButton->isChecked() ? 1 : 0;
    QDBusMessage createMessage = QDBusMessage::createMethodCall("org.freedesktop.Accounts", "/org/freedesktop/Accounts", "org.freedesktop.Accounts", "CreateUser");
    createMessage.setArguments({
        ui->usernameLineEdit->text(),
        ui->fullNameLineEdit->text(),
        accountType
    });

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(createMessage));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        QString error;
        if (watcher->isError()) {
            error = watcher->error().message();
        } else {
            UserPtr u(new User(watcher->reply().arguments().first().value<QDBusObjectPath>()));

            //Set the user's password
            tPromiseResults<void> results;
            if (d->passwordMode == User::SetPassword) {
                results = u->setPassword(ui->passwordLineEdit->text(), ui->passwordHintLineEdit->text())->await();
            } else {
                results = u->setPasswordMode(d->passwordMode)->await();
            }

            if (results.error.isEmpty()) {
                error = results.error;
            }
        }

        if (error.isEmpty()) {
            emit done();
        } else {
            //Bail out
            QTimer::singleShot(1000, [=] {
                ui->stackedWidget->setCurrentWidget(ui->confirmPage);
                ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

                tToast* toast = new tToast();
                toast->setTitle(tr("Couldn't create user"));
                toast->setText(error);
                connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
                toast->show(this);
            });
        }
    });

//    QDBusReply<QDBusObjectPath> newUser = QDBusConnection::systemBus().call(createMessage);
//    if (newUser.error().isValid()) {
//        return;
//    } else {
//        d->editingUserPath = newUser.value().path();
//    }

//    QTimer::singleShot(2000, this, &AddUserDialog::done);
}

void AddUserDialog::on_administratorButton_clicked()
{
    ui->administratorButton->setChecked(true);
    ui->standardUserButton->setChecked(false);
    ui->userTypeConfirmLabel->setText(tr("Administrator"));
}

void AddUserDialog::on_standardUserButton_clicked()
{
    ui->administratorButton->setChecked(false);
    ui->standardUserButton->setChecked(true);
    ui->userTypeConfirmLabel->setText(tr("Standard"));
}
