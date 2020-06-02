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
#include "statuscenterquickswitch.h"
#include "ui_statuscenterquickswitch.h"

#include <quickswitch.h>

struct StatusCenterQuickSwitchPrivate {
    QuickSwitch* sw;
};

StatusCenterQuickSwitch::StatusCenterQuickSwitch(QuickSwitch* sw, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::StatusCenterQuickSwitch) {
    ui->setupUi(this);
    d = new StatusCenterQuickSwitchPrivate();

    d->sw = sw;
    ui->sw->setChecked(sw->isChecked());
    ui->titleLabel->setText(sw->title());

    sw->setSwitch(ui->sw);
    connect(sw, &QuickSwitch::titleChanged, ui->titleLabel, &QLabel::setText);
}

StatusCenterQuickSwitch::~StatusCenterQuickSwitch() {
    delete d;
    delete ui;
}

void StatusCenterQuickSwitch::on_sw_toggled(bool checked) {
    d->sw->setChecked(checked);
}
