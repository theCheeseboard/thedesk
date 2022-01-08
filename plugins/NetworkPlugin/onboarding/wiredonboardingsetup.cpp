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
#include "wiredonboardingsetup.h"
#include "ui_wiredonboardingsetup.h"

#include "common.h"
#include <QtConcurrent>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/Settings>

#include <tpopover.h>
#include "statusCenter/popovers/connectionselectionpopover.h"

struct WiredOnboardingSetupPrivate {
    NetworkManager::WiredDevice::Ptr device;
};

WiredOnboardingSetup::WiredOnboardingSetup(QString device, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::WiredOnboardingSetup) {
    ui->setupUi(this);
    d = new WiredOnboardingSetupPrivate();
    d->device = NetworkManager::WiredDevice::Ptr(new NetworkManager::WiredDevice(device));

    connect(d->device.data(), &NetworkManager::WiredDevice::carrierChanged, this, &WiredOnboardingSetup::updateState);
    connect(d->device.data(), &NetworkManager::WiredDevice::stateChanged, this, &WiredOnboardingSetup::updateState);
    updateState();

    ui->spinner->setFixedSize(SC_DPI_T(QSize(16, 16), QSize));
}

WiredOnboardingSetup::~WiredOnboardingSetup() {
    delete ui;
    delete d;
}

void WiredOnboardingSetup::on_connectButton_clicked() {
    NetworkManager::Connection::List connections = NetworkManager::listConnections();
    QtConcurrent::blockingFilter(connections, [ = ](const NetworkManager::Connection::Ptr & connection) {
        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->interfaceName() != "" && d->device->interfaceName() != settings->interfaceName()) return false; //This connection is not applicable to this device
        return QList<NetworkManager::ConnectionSettings::ConnectionType>({NetworkManager::ConnectionSettings::Wired}).contains(connection->settings()->connectionType());
    });

    if (connections.count() == 0) {
        //Create a new automatic connection
        NetworkManager::ConnectionSettings settings(NetworkManager::ConnectionSettings::Wired);
        settings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());
        settings.setInterfaceName(d->device->interfaceName());
        NetworkManager::addAndActivateConnection(settings.toMap(), d->device->uni(), "");
    } else if (connections.count() == 1) {
        //Use this connection
        NetworkManager::activateConnection(connections.first()->path(), d->device->uni(), "");
    } else {
        //Ask the user for the connection to use
        ConnectionSelectionPopover* selection = new ConnectionSelectionPopover(connections);
        tPopover* popover = new tPopover(selection);
        popover->setPopoverWidth(SC_DPI(600));
        connect(selection, &ConnectionSelectionPopover::reject, popover, &tPopover::dismiss);
        connect(selection, &ConnectionSelectionPopover::accept, this, [ = ](NetworkManager::Connection::Ptr connection) {
            NetworkManager::activateConnection(connection->path(), d->device->uni(), "");
            popover->dismiss();
        });
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, selection, &ConnectionSelectionPopover::deleteLater);
        popover->show(this->window());
    }
}

void WiredOnboardingSetup::updateState() {
    if (d->device->carrier()) {
        ui->stateDescription->setText(tr("Connect to the Internet using a wired network"));
        ui->connectButton->setEnabled(true);
    } else {
        ui->stateDescription->setText(tr("To connect to the Internet using a wired network, connect the network cable."));
        ui->connectButton->setEnabled(false);
    }

    ui->spinner->setVisible(Common::isDeviceConnecting(d->device));
}

