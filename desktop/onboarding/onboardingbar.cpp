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
#include "onboardingbar.h"
#include "ui_onboardingbar.h"

#include <TimeDate/desktoptimedate.h>

OnboardingBar::OnboardingBar(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::OnboardingBar) {
    ui->setupUi(this);

    DesktopTimeDate::makeTimeLabel(ui->clock, DesktopTimeDate::Time);
    DesktopTimeDate::makeTimeLabel(ui->date, DesktopTimeDate::StandardDate);
}

OnboardingBar::~OnboardingBar() {
    delete ui;
}

void OnboardingBar::on_closeButton_clicked() {
    emit closeClicked();
}

void OnboardingBar::on_muteButton_toggled(bool checked) {
    emit muteToggled(checked);
}
