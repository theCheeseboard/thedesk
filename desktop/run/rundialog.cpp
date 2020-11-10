/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
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
#include "rundialog.h"
#include "ui_rundialog.h"

#include "transparentdialog.h"
#include <QTimer>
#include <tpopover.h>
#include <keygrab.h>
#include <QProcess>
#include <terrorflash.h>

RunDialog::RunDialog(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RunDialog) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->widget->setFixedWidth(SC_DPI(600));

    this->setFocusProxy(ui->lineEdit);
}

RunDialog::~RunDialog() {
    delete ui;
}

void RunDialog::initialise() {
    KeyGrab* grab = new KeyGrab(QKeySequence(Qt::AltModifier | Qt::Key_F2), "run");
    connect(grab, &KeyGrab::activated, [ = ] {
        TransparentDialog* dialog = new TransparentDialog();
        dialog->setWindowFlag(Qt::FramelessWindowHint);
        dialog->setWindowFlag(Qt::WindowStaysOnTopHint);
        dialog->showFullScreen();

        QTimer::singleShot(500, [ = ] {
            RunDialog* popoverContents = new RunDialog();

            tPopover* popover = new tPopover(popoverContents);
            popover->setPopoverSide(tPopover::Bottom);
            popover->setPopoverWidth(popoverContents->sizeHint().height());
            popover->setPerformBlur(false);
            connect(popoverContents, &RunDialog::done, popover, &tPopover::dismiss);
            connect(popover, &tPopover::dismissed, popoverContents, &RunDialog::deleteLater);
            connect(popover, &tPopover::dismissed, [ = ] {
                popover->deleteLater();
                dialog->deleteLater();
            });
            popover->show(dialog);
            popoverContents->setFocus();
        });
    });
}

void RunDialog::on_titleLabel_backButtonClicked() {
    emit done();
}

void RunDialog::on_runButton_clicked() {
    QStringList parts = ui->lineEdit->text().split(" ");
    QString executable = parts.takeFirst();

    if (QProcess::startDetached(executable, parts)) {
        emit done();
    } else {
        tErrorFlash::flashError(ui->lineEdit);
    }
}
