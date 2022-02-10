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
#include "devicepane.h"
#include "ui_devicepane.h"

#include <statemanager.h>
#include <statuscentermanager.h>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>

struct DevicePanePrivate {
    QListWidgetItem* item;
    NetworkManager::Device::Ptr device;
};

DevicePane::DevicePane(QString uni, QWidget* parent) :
    AbstractDevicePane(parent),
    ui(new Ui::DevicePane) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->actionsWidget->setFixedWidth(contentWidth);
    ui->statusWidget->setFixedWidth(contentWidth);

    ui->disconnectButton->setProperty("type", "destructive");

    d = new DevicePanePrivate();
    d->item = new QListWidgetItem();
    d->device = NetworkManager::findNetworkInterface(uni);

    d->item->setText(d->device->interfaceName());
    ui->titleLabel->setText(d->device->interfaceName());

    connect(d->device.data(), &NetworkManager::Device::interfaceNameChanged, this, [ = ] {
        d->item->setText(d->device->interfaceName());
        ui->titleLabel->setText(d->device->interfaceName());
    });
}

DevicePane::~DevicePane() {
    delete d->item;
    delete d;
    delete ui;
}

QListWidgetItem* DevicePane::leftPaneItem() {
    return d->item;
}

void DevicePane::on_disconnectButton_clicked() {
    d->device->disconnectInterface();
}

void DevicePane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}


void DevicePane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}
