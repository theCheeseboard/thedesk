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
#include "crashwidget.h"
#include "ui_crashwidget.h"

#include "backtracepopover.h"
#include "splash/splashcontroller.h"
#include <libcontemporary_global.h>
#include <tpopover.h>

CrashWidget::CrashWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CrashWidget) {
    ui->setupUi(this);

    ui->iconLabel->setPixmap(QIcon(":/icons/crash.svg").pixmap(SC_DPI_T(QSize(128, 128), QSize)));

    connect(SplashController::instance(), &SplashController::crash, this, [=] {
        ui->descriptionLabel->setText(tr("theDesk had a problem and has stopped working."));
    });
    connect(SplashController::instance(), &SplashController::startFail, this, [=] {
        ui->descriptionLabel->setText(tr("theDesk couldn't start because of a problem."));
    });
}

CrashWidget::~CrashWidget() {
    delete ui;
}

void CrashWidget::on_logOutButton_clicked() {
    SplashController::instance()->logout();
}

void CrashWidget::on_relaunchButton_clicked() {
    SplashController::instance()->startDE();
}

void CrashWidget::on_detailsButton_clicked() {
    BacktracePopover* bt = new BacktracePopover();
    tPopover* popover = new tPopover(bt);
    popover->setPopoverWidth(SC_DPI(600));
    popover->setPopoverSide(tPopover::Bottom);
    connect(bt, &BacktracePopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, bt, &BacktracePopover::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this);
}
