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
#include "networkstatuscenterleftpane.h"
#include "ui_networkstatuscenterleftpane.h"

#include <statemanager.h>
#include <statuscentermanager.h>

NetworkStatusCenterLeftPane::NetworkStatusCenterLeftPane(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NetworkStatusCenterLeftPane) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
}

NetworkStatusCenterLeftPane::~NetworkStatusCenterLeftPane() {
    delete ui;
}

void NetworkStatusCenterLeftPane::addItem(QListWidgetItem* item) {
    ui->devicesWidget->addItem(item);
}

void NetworkStatusCenterLeftPane::removeItem(QListWidgetItem* item) {
    ui->devicesWidget->takeItem(ui->devicesWidget->row(item));
}

void NetworkStatusCenterLeftPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->returnToRootMenu();
}

void NetworkStatusCenterLeftPane::on_devicesWidget_currentRowChanged(int currentRow) {
    emit currentChanged(currentRow);
}
