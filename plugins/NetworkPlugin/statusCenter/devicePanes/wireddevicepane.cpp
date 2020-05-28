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
#include "wireddevicepane.h"
#include "ui_wireddevicepane.h"

#include <QtConcurrent>

#include "common.h"
#include "../popovers/connectionselectionpopover.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <hudmanager.h>
#include <tsettings.h>
#include <tpopover.h>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/Settings>

struct WiredDevicePanePrivate {
    QListWidgetItem* item;
    NetworkManager::WiredDevice::Ptr device;

    tSettings settings;
    NetworkManager::Device::State oldState;
};

WiredDevicePane::WiredDevicePane(QString uni, QWidget* parent) :
    AbstractDevicePane(parent),
    ui(new Ui::WiredDevicePane) {
    ui->setupUi(this);
    d = new WiredDevicePanePrivate();

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->actionsWidget->setFixedWidth(contentWidth);
    ui->statusWidget->setFixedWidth(contentWidth);

    ui->disconnectButton->setProperty("type", "destructive");
    ui->errorFrame->setVisible(false);

    d->item = new QListWidgetItem();
    d->item->setText(tr("Wired"));
    d->device = NetworkManager::findNetworkInterface(uni).staticCast<NetworkManager::WiredDevice>();

//    connect(d->device.data(), &NetworkManager::WirelessDevice::activeConnectionChanged, this, &WifiDevicePane::updateNetworkName);
//    connect(d->device.data(), &NetworkManager::WirelessDevice::activeAccessPointChanged, this, &WifiDevicePane::updateNetworkName);
//    updateNetworkName();

    connect(d->device.data(), &NetworkManager::WiredDevice::stateChanged, this, &WiredDevicePane::updateState);
    updateState();

    connect(d->device.data(), &NetworkManager::WiredDevice::stateChanged, this, [ = ](NetworkManager::Device::State newState, NetworkManager::Device::State oldState, NetworkManager::Device::StateChangeReason reason) {
        if (d->settings.value("NetworkPlugin/notifications.activation").toBool()) {
            switch (newState) {
                case NetworkManager::Device::Unavailable:
                    d->device->setAutoconnect(false);
                    Q_FALLTHROUGH();
                case NetworkManager::Device::Disconnected:
                    if (oldState != NetworkManager::Device::Failed) StateManager::hudManager()->showHud({
                        {"icon", "network-wired-unavailable"},
                        {"title", tr("Wired")},
                        {"text", tr("Disconnected")}
                    });
                    break;
                case NetworkManager::Device::Activated: {
                    d->device->setAutoconnect(true);
                    QString title = tr("Wired");

                    StateManager::hudManager()->showHud({
                        {"icon", "network-wired-activated"},
                        {"title", title},
                        {"text", tr("Connected")}
                    });
                    break;
                }
                case NetworkManager::Device::Failed:
                    d->device->setAutoconnect(false);
                    StateManager::hudManager()->showHud({
                        {"icon", "network-wired-error"},
                        {"title", tr("Wi-Fi")},
                        {"text", tr("Failed")}
                    });
                    break;
                default:
                    break;
            }
        }
    });

}

WiredDevicePane::~WiredDevicePane() {
    delete d;
    delete ui;
}

void WiredDevicePane::updateState() {
    ui->deviceIcon->setPixmap(QIcon::fromTheme("computer").pixmap(SC_DPI_T(QSize(96, 96), QSize)));
    ui->routerIcon->setPixmap(QIcon::fromTheme("network-modem").pixmap(SC_DPI_T(QSize(96, 96), QSize)));
    ui->routerName->setText(tr("Router"));

    NetworkManager::DeviceStateReason stateReason = d->device->stateReason();
    if (d->oldState != NetworkManager::Device::Failed) {
        //Only get rid of the error message here if the previous state was not failure.
        ui->errorFrame->setVisible(false);
    }

    switch (stateReason.state()) {
        case NetworkManager::Device::UnknownState:
        case NetworkManager::Device::Unmanaged:
        case NetworkManager::Device::Unavailable: {
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->leftStateLine->setEnabled(false);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(false);
            ui->connectButton->setVisible(false);

            d->device->setAutoconnect(false);
            ui->errorFrame->setTitle(tr("Unavailable"));

            QString reasonText;
            if (stateReason.reason() == NetworkManager::Device::CarrierReason) {
                reasonText = tr("Connect an Ethernet cable.");
            } else {
                reasonText = tr("This network is unavailable because %2.");
                reasonText = reasonText.arg(Common::stateChangeReasonToString(stateReason.reason()));
            }
            ui->errorFrame->setText(reasonText);
            ui->errorFrame->setState(tStatusFrame::Warning);
            ui->errorFrame->setVisible(true);
            break;
        }
        case NetworkManager::Device::Disconnected:
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->leftStateLine->setEnabled(false);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(false);
            ui->connectButton->setVisible(true);
            break;
        case NetworkManager::Device::Failed: {
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->leftStateLine->setEnabled(false);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(false);
            ui->connectButton->setVisible(true);

            ui->errorFrame->setTitle(tr("Connection Failure"));

            QString reasonText = tr("Connecting to the network failed because %2.");
            reasonText = reasonText.arg(Common::stateChangeReasonToString(stateReason.reason()));
            ui->errorFrame->setText(reasonText);
            ui->errorFrame->setState(tStatusFrame::Error);
            ui->errorFrame->setVisible(true);
            break;
        }
        case NetworkManager::Device::Preparing:
        case NetworkManager::Device::ConfiguringHardware:
        case NetworkManager::Device::NeedAuth:
        case NetworkManager::Device::ConfiguringIp:
        case NetworkManager::Device::CheckingIp:
        case NetworkManager::Device::WaitingForSecondaries:
        case NetworkManager::Device::Deactivating:
            ui->stateConnecting->setVisible(true);
            ui->stateIcon->setVisible(false);
            ui->leftStateLine->setEnabled(true);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(true);
            ui->connectButton->setVisible(false);
            break;
        case NetworkManager::Device::Activated:
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-ok").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->leftStateLine->setEnabled(true);
            ui->rightStateLine->setEnabled(true);
            ui->disconnectButton->setVisible(true);
            ui->connectButton->setVisible(false);
            break;
    }

    d->oldState = stateReason.state();
}


QListWidgetItem* WiredDevicePane::leftPaneItem() {
    return d->item;
}

void WiredDevicePane::on_disconnectButton_clicked() {
    d->device->disconnectInterface();
}

void WiredDevicePane::on_connectButton_clicked() {
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
