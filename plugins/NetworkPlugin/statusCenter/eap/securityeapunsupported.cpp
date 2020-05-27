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
#include "securityeapunsupported.h"
#include "ui_securityeapunsupported.h"

SecurityEapUnsupported::SecurityEapUnsupported(QWidget* parent) :
    SecurityEap(parent),
    ui(new Ui::SecurityEapUnsupported) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
}

SecurityEapUnsupported::~SecurityEapUnsupported() {
    delete ui;
}

void SecurityEapUnsupported::on_titleLabel_backButtonClicked() {
    emit back();
}


void SecurityEapUnsupported::populateSetting(NetworkManager::Security8021xSetting::Ptr setting) {
}
