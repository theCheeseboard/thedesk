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
#include "deleteuserdialog.h"
#include "ui_deleteuserdialog.h"

#include <ttoast.h>

struct DeleteUserDialogPrivate {
    UserPtr user;
    bool shouldDeleteFiles;
};

DeleteUserDialog::DeleteUserDialog(UserPtr user, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DeleteUserDialog) {
    ui->setupUi(this);
    d = new DeleteUserDialogPrivate();

    ui->titleLabel->setBackButtonShown(true);
    ui->doDeleteTitleLabel->setBackButtonShown(true);

    d->user = user;
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->deleteButton->setProperty("type", "destructive");
}

DeleteUserDialog::~DeleteUserDialog() {
    delete d;
    delete ui;
}

void DeleteUserDialog::on_deleteFilesButton_clicked() {
    d->shouldDeleteFiles = true;
    ui->stackedWidget->setCurrentWidget(ui->confirmPage);
    ui->deleteDataWarning->setVisible(true);
}

void DeleteUserDialog::on_keepFilesButton_clicked() {
    d->shouldDeleteFiles = false;
    ui->stackedWidget->setCurrentWidget(ui->confirmPage);
    ui->deleteDataWarning->setVisible(false);
}

void DeleteUserDialog::on_deleteButton_clicked() {
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->stackedWidget->setCurrentWidget(ui->processingPage);

    d->user->deleteUser(d->shouldDeleteFiles)->then([ = ] {
        emit done();
    })->error([ = ](QString err) {
        //Bail out
        QTimer::singleShot(1000, [ = ] {
            ui->stackedWidget->setCurrentWidget(ui->confirmPage);
            ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

            tToast* toast = new tToast();
            toast->setTitle(tr("Couldn't delete user"));
            toast->setText(err);
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        });
    });
}

void DeleteUserDialog::on_titleLabel_backButtonClicked() {
    emit done();
}

void DeleteUserDialog::on_doDeleteTitleLabel_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->initialPage);
}
