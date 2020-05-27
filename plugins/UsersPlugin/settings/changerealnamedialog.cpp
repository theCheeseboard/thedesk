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
#include "changerealnamedialog.h"
#include "ui_changerealnamedialog.h"

#include <terrorflash.h>
#include <ttoast.h>

struct ChangeRealNameDialogPrivate {
    UserPtr user;
};

ChangeRealNameDialog::ChangeRealNameDialog(UserPtr user, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ChangeRealNameDialog) {
    ui->setupUi(this);
    d = new ChangeRealNameDialogPrivate();

    ui->titleLabel->setBackButtonShown(true);

    d->user = user;
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
}

ChangeRealNameDialog::~ChangeRealNameDialog() {
    delete d;
    delete ui;
}

void ChangeRealNameDialog::on_setPasswordButton_clicked() {
    if (ui->realNameLineEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->realNameLineEdit);
        return;
    }

    //Set the user's real name
    ui->stackedWidget->setCurrentWidget(ui->processingPage);

    d->user->setRealName(ui->realNameLineEdit->text())->then([ = ] {
        emit done();
    })->error([ = ](QString err) {
        //Bail out
        QTimer::singleShot(1000, [ = ] {
            ui->stackedWidget->setCurrentWidget(ui->setRealNamePage);

            tToast* toast = new tToast();
            toast->setTitle(tr("Couldn't set real name"));
            toast->setText(err);
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        });
    });
}

void ChangeRealNameDialog::on_titleLabel_backButtonClicked() {
    emit done();
}
