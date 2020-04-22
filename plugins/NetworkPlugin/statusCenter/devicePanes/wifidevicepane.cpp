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
#include "wifidevicepane.h"
#include "ui_wifidevicepane.h"

#include <tsettings.h>
#include <statemanager.h>
#include <hudmanager.h>
#include <statuscentermanager.h>
#include <tpopover.h>
#include "../popovers/wirelessnetworkselectionpopover.h"
#include "common.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>

struct WifiDevicePanePrivate {
    QListWidgetItem* item;
    NetworkManager::WirelessDevice::Ptr device;

    tSettings settings;
};

WifiDevicePane::WifiDevicePane(QString uni, QWidget* parent) :
    AbstractDevicePane(parent),
    ui(new Ui::WifiDevicePane) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->actionsWidget->setFixedWidth(contentWidth);
    ui->statusWidget->setFixedWidth(contentWidth);

    ui->disconnectButton->setProperty("type", "destructive");
    ui->errorFrame->setVisible(false);

    d = new WifiDevicePanePrivate();
    d->item = new QListWidgetItem();
    d->device = NetworkManager::findNetworkInterface(uni).staticCast<NetworkManager::WirelessDevice>();

    connect(d->device.data(), &NetworkManager::WirelessDevice::activeConnectionChanged, this, &WifiDevicePane::updateNetworkName);
    connect(d->device.data(), &NetworkManager::WirelessDevice::activeAccessPointChanged, this, &WifiDevicePane::updateNetworkName);
    updateNetworkName();

    connect(d->device.data(), &NetworkManager::WirelessDevice::stateChanged, this, &WifiDevicePane::updateState);
    connect(d->device.data(), &NetworkManager::WirelessDevice::activeAccessPointChanged, this, &WifiDevicePane::updateState);
    updateState();


    connect(d->device.data(), &NetworkManager::WirelessDevice::stateChanged, this, [ = ](NetworkManager::Device::State newState, NetworkManager::Device::State oldState, NetworkManager::Device::StateChangeReason reason) {
        if (d->settings.value("NetworkPlugin/notifications.activation").toBool()) {
            switch (newState) {
                case NetworkManager::Device::Disconnected:
                    if (oldState != NetworkManager::Device::Failed) StateManager::hudManager()->showHud({
                        {"icon", "network-wireless-disconnected"},
                        {"title", tr("Wi-Fi")},
                        {"text", tr("Disconnected")}
                    });
                    break;
                case NetworkManager::Device::Activated: {
                    QString title;
                    if (d->device->activeAccessPoint()) {
                        title = d->device->activeAccessPoint()->ssid();
                    } else {
                        title = tr("Wi-Fi");
                    }

                    StateManager::hudManager()->showHud({
                        {"icon", "network-wireless-connected-100"},
                        {"title", title},
                        {"text", tr("Connected")}
                    });
                    break;
                }
                case NetworkManager::Device::Failed:
                    StateManager::hudManager()->showHud({
                        {"icon", "network-wireless-disconnected"},
                        {"title", tr("Wi-Fi")},
                        {"text", tr("Failed")}
                    });
                    break;
                default:
                    break;

            }
        }

        if (newState == NetworkManager::Device::Failed) {
            d->device->setAutoconnect(false);
            ui->errorFrame->setTitle(tr("Connection Failure"));

            QString reasonText = tr("Connecting to %1 failed because %2.");
            if (d->device->activeAccessPoint()) {
                reasonText = reasonText.arg(d->device->activeAccessPoint()->ssid());
            } else {
                reasonText = reasonText.arg(tr("the network"));
            }
            reasonText = reasonText.arg(Common::stateChangeReasonToString(reason));
            ui->errorFrame->setText(reasonText);
            ui->errorFrame->setState(tStatusFrame::Error);
            ui->errorFrame->setVisible(true);
        } else if (oldState != NetworkManager::Device::Failed) {
            ui->errorFrame->setVisible(false);
        }
    });
}

WifiDevicePane::~WifiDevicePane() {
    delete d->item;
    delete d;
    delete ui;
}

void WifiDevicePane::updateNetworkName() {
    if (d->device->isActive() && d->device->activeAccessPoint()) {
        d->item->setText(d->device->activeAccessPoint()->ssid());
        ui->titleLabel->setText(d->device->activeAccessPoint()->ssid());
    } else {
        d->item->setText(tr("Wi-Fi"));
        ui->titleLabel->setText(tr("Wi-Fi"));
    }
}

void WifiDevicePane::updateState() {
    ui->deviceIcon->setPixmap(QIcon::fromTheme("computer").pixmap(SC_DPI_T(QSize(96, 96), QSize)));
    switch (d->device->mode()) {
        case NetworkManager::WirelessDevice::Adhoc:
        case NetworkManager::WirelessDevice::ApMode:
            ui->routerIcon->setPixmap(QIcon::fromTheme("computer").pixmap(SC_DPI_T(QSize(96, 96), QSize)));
            break;
        case NetworkManager::WirelessDevice::Unknown:
        case NetworkManager::WirelessDevice::Infra:
            ui->routerIcon->setPixmap(QIcon::fromTheme("network-wireless-connected-100").pixmap(SC_DPI_T(QSize(96, 96), QSize)));
            break;
    }

    bool setRouterName = false;

    switch (d->device->state()) {
        case NetworkManager::Device::UnknownState:
        case NetworkManager::Device::Unmanaged:
        case NetworkManager::Device::Unavailable:
        case NetworkManager::Device::Disconnected:
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->routerName->setText(tr("Disconnected"));
            ui->leftStateLine->setEnabled(false);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(false);
            break;
        case NetworkManager::Device::Failed:
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->routerName->setText(tr("Failed"));
            ui->leftStateLine->setEnabled(false);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(false);
            break;
        case NetworkManager::Device::Preparing:
        case NetworkManager::Device::ConfiguringHardware:
        case NetworkManager::Device::NeedAuth:
        case NetworkManager::Device::ConfiguringIp:
        case NetworkManager::Device::CheckingIp:
        case NetworkManager::Device::WaitingForSecondaries:
        case NetworkManager::Device::Deactivating:
            ui->stateConnecting->setVisible(true);
            ui->stateIcon->setVisible(false);
            setRouterName = true;
            ui->leftStateLine->setEnabled(true);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(true);
            break;
        case NetworkManager::Device::Activated:
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-ok").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            setRouterName = true;
            ui->leftStateLine->setEnabled(true);
            ui->rightStateLine->setEnabled(true);
            ui->disconnectButton->setVisible(true);
            break;
    }

    if (setRouterName) {
        QString routerName;
        switch (d->device->mode()) {
            case NetworkManager::WirelessDevice::ApMode:
                routerName = tr("0 Devices");
                break;
            case NetworkManager::WirelessDevice::Infra:
                if (d->device->activeAccessPoint()) {
                    routerName = d->device->activeAccessPoint()->ssid();
                    break;
                }
                Q_FALLTHROUGH();
            default:
                routerName = tr("Unknown");
        }

        ui->routerName->setText(routerName);
    }
}

QListWidgetItem* WifiDevicePane::leftPaneItem() {
    return d->item;
}

void WifiDevicePane::on_disconnectButton_clicked() {
    d->device->disconnectInterface();
}

void WifiDevicePane::on_selectNetworkButton_clicked() {
    WirelessNetworkSelectionPopover* selection = new WirelessNetworkSelectionPopover(d->device->uni());
    tPopover* popover = new tPopover(selection);
    popover->setPopoverWidth(SC_DPI(600));
    connect(selection, &WirelessNetworkSelectionPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, selection, &WirelessNetworkSelectionPopover::deleteLater);
    popover->show(this->window());
}
