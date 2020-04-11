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

#include "splash/splashcontroller.h"

CrashWidget::CrashWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CrashWidget) {
    ui->setupUi(this);

    connect(SplashController::instance(), &SplashController::crash, this, [ = ] {
        ui->descriptionLabel->setText(tr("theDesk had a problem and has stopped working."));
    });
    connect(SplashController::instance(), &SplashController::startFail, this, [ = ] {
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
