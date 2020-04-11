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
#include "systemsettingsleftpane.h"
#include "ui_systemsettingsleftpane.h"

#include <the-libs_global.h>
#include <statemanager.h>
#include <statuscentermanager.h>

SystemSettingsLeftPane::SystemSettingsLeftPane(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SystemSettingsLeftPane) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->mainList->setIconSize(SC_DPI_T(QSize(32, 32), QSize));
}

SystemSettingsLeftPane::~SystemSettingsLeftPane() {
    delete ui;
}

void SystemSettingsLeftPane::appendItem(QListWidgetItem* item) {
    ui->mainList->addItem(item);
}

void SystemSettingsLeftPane::insertItem(int index, QListWidgetItem* item) {
    ui->mainList->insertItem(index, item);
}

void SystemSettingsLeftPane::removeItem(QListWidgetItem* item) {
    ui->mainList->removeItemWidget(item);
}

void SystemSettingsLeftPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->returnToRootMenu();
}
