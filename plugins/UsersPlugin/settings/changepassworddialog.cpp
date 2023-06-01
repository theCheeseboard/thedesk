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
#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"

#include <terrorflash.h>
#include <ttoast.h>

struct ChangePasswordDialogPrivate {
        UserPtr user;
};

ChangePasswordDialog::ChangePasswordDialog(UserPtr user, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ChangePasswordDialog) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);

    d = new ChangePasswordDialogPrivate();
    d->user = user;

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->lockedAccountWarning->setVisible(user->isLocked());
}

ChangePasswordDialog::~ChangePasswordDialog() {
    delete d;
    delete ui;
}

QCoro::Task<> ChangePasswordDialog::on_setPasswordButton_clicked() {
    if (ui->passwordLineEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->passwordLineEdit);
        co_return;
    }

    if (ui->passwordLineEdit->text() != ui->confirmPasswordLineEdit->text()) {
        tErrorFlash::flashError(ui->confirmPasswordLineEdit);
        co_return;
    }

    // Set the user's password
    ui->stackedWidget->setCurrentWidget(ui->processingPage);

    try {
        co_await d->user->setPassword(ui->passwordLineEdit->text(), ui->passwordHintLineEdit->text());
        emit done();
    } catch (UserManipulationException& ex) {
        // Bail out
        QTimer::singleShot(1000, [=] {
            ui->stackedWidget->setCurrentWidget(ui->setPasswordPage);

            tToast* toast = new tToast();
            toast->setTitle(tr("Couldn't set password"));
            toast->setText(ex.reason());
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        });
    }
}

QCoro::Task<> ChangePasswordDialog::on_askForPasswordButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->processingPage);

    try {
        co_await d->user->setPasswordMode(User::SetAtLogin);
        emit done();
    } catch (UserManipulationException& ex) {
        // Bail out
        QTimer::singleShot(1000, [this, ex] {
            ui->stackedWidget->setCurrentWidget(ui->setPasswordPage);

            tToast* toast = new tToast();
            toast->setTitle(tr("Couldn't set password policy"));
            toast->setText(ex.reason());
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        });
    }
}

QCoro::Task<> ChangePasswordDialog::on_noPasswordButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->processingPage);
    try {
        co_await d->user->setPasswordMode(User::NoPassword);
        emit done();
    } catch (UserManipulationException& ex) {
        // Bail out
        QTimer::singleShot(1000, [this, ex] {
            ui->stackedWidget->setCurrentWidget(ui->setPasswordPage);

            tToast* toast = new tToast();
            toast->setTitle(tr("Couldn't set password policy"));
            toast->setText(ex.reason());
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        });
    }
}

void ChangePasswordDialog::on_titleLabel_backButtonClicked() {
    emit done();
}
