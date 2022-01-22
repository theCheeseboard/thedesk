/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#include "wirelessonboardingsetup.h"
#include "ui_wirelessonboardingsetup.h"

#include "common.h"

#include <tpopover.h>
#include "statusCenter/popovers/wirelessnetworkselectionpopover.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>

struct WirelessOnboardingSetupPrivate {
    NetworkManager::WirelessDevice::Ptr device;
};

WirelessOnboardingSetup::WirelessOnboardingSetup(QString device, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::WirelessOnboardingSetup) {
    ui->setupUi(this);

    d = new WirelessOnboardingSetupPrivate();
    d->device = NetworkManager::WirelessDevice::Ptr(new NetworkManager::WirelessDevice(device));

    connect(d->device.data(), &NetworkManager::WirelessDevice::stateChanged, this, &WirelessOnboardingSetup::updateState);
    updateState();

    ui->spinner->setFixedSize(SC_DPI_T(QSize(16, 16), QSize));
}

WirelessOnboardingSetup::~WirelessOnboardingSetup() {
    delete ui;
    delete d;
}

void WirelessOnboardingSetup::on_selectNetworkButton_clicked() {
    //Turn on Wi-Fi in case it is disabled
    NetworkManager::setWirelessEnabled(true);

    WirelessNetworkSelectionPopover* selection = new WirelessNetworkSelectionPopover(d->device->uni());
    tPopover* popover = new tPopover(selection);
    popover->setPopoverWidth(SC_DPI(600));
    connect(selection, &WirelessNetworkSelectionPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, selection, &WirelessNetworkSelectionPopover::deleteLater);
    popover->show(this->window());
}

void WirelessOnboardingSetup::updateState() {
    ui->spinner->setVisible(Common::isDeviceConnecting(d->device));
}

