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
#include "lockuserdialog.h"
#include "ui_lockuserdialog.h"

#include <ttoast.h>

struct LockUserDialogPrivate {
    UserPtr user;
};

LockUserDialog::LockUserDialog(UserPtr user, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::LockUserDialog) {
    ui->setupUi(this);
    d = new LockUserDialogPrivate();
    d->user = user;

    ui->lockAccountTitleLabel->setBackButtonShown(true);
    ui->unlockAccountTitleLabel->setBackButtonShown(true);

    if (user->isLocked()) {
        ui->stackedWidget->setCurrentWidget(ui->unlockPage);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->lockPage);
    }
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
}

LockUserDialog::~LockUserDialog() {
    delete d;
    delete ui;
}

void LockUserDialog::toggleLock() {
    ui->stackedWidget->setCurrentWidget(ui->processingPage);

    d->user->setLocked(!d->user->isLocked())->then([ = ] {
        emit done();
    })->error([ = ](QString err) {
        //Bail out
        QTimer::singleShot(1000, [ = ] {
            tToast* toast = new tToast();
            if (d->user->isLocked()) {
                ui->stackedWidget->setCurrentWidget(ui->unlockPage);
                toast->setTitle(tr("Couldn't unlock user"));
            } else {
                ui->stackedWidget->setCurrentWidget(ui->lockPage);
                toast->setTitle(tr("Couldn't lock user"));
            }
            toast->setText(err);
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        });
    });
}

void LockUserDialog::on_lockButton_clicked() {
    this->toggleLock();
}

void LockUserDialog::on_unlockButton_clicked() {
    this->toggleLock();
}

void LockUserDialog::on_lockAccountTitleLabel_backButtonClicked() {
    emit done();
}

void LockUserDialog::on_unlockAccountTitleLabel_backButtonClicked() {
    emit done();
}
