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
#include <barmanager.h>
#include <tpopover.h>
#include "../popovers/wirelessnetworkselectionpopover.h"
#include "common.h"
#include <QHostInfo>
#include <QRandomGenerator>
#include <QFontDatabase>
#include <ttoast.h>
#include <icontextchunk.h>
#include "switchmanager.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Ipv6Setting>

struct WifiDevicePanePrivate {
    QListWidgetItem* item;
    NetworkManager::WirelessDevice::Ptr device;

    IconTextChunk* tetheringChunk;

    SwitchManager* switchManager = nullptr;

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
    ui->tetheringWidget->setFixedWidth(contentWidth);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->wifiOffIcon->setPixmap(QIcon::fromTheme("network-wireless-disconnected").pixmap(SC_DPI_T(QSize(128, 128), QSize)));
    ui->wifiBlockedIcon->setPixmap(QIcon::fromTheme("network-wireless-disconnected").pixmap(SC_DPI_T(QSize(128, 128), QSize)));
    ui->wifiFlightIcon->setPixmap(QIcon::fromTheme("flight-mode").pixmap(SC_DPI_T(QSize(128, 128), QSize)));

    ui->disconnectButton->setProperty("type", "destructive");
    ui->errorFrame->setVisible(false);
    ui->tetheringKey->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    d = new WifiDevicePanePrivate();
    d->item = new QListWidgetItem();
    d->device = NetworkManager::findNetworkInterface(uni).staticCast<NetworkManager::WirelessDevice>();

    d->tetheringChunk = new IconTextChunk("network-tethering");
    d->tetheringChunk->setIcon(QIcon::fromTheme("network-wireless-tethered"));
    d->tetheringChunk->setText(tr("Tethering"));

    connect(d->device.data(), &NetworkManager::WirelessDevice::activeConnectionChanged, this, &WifiDevicePane::updateNetworkName);
    connect(d->device.data(), &NetworkManager::WirelessDevice::activeAccessPointChanged, this, &WifiDevicePane::updateNetworkName);
    updateNetworkName();

    connect(d->device.data(), &NetworkManager::WirelessDevice::stateChanged, this, &WifiDevicePane::updateState);
    connect(d->device.data(), &NetworkManager::WirelessDevice::activeAccessPointChanged, this, &WifiDevicePane::updateState);
    connect(d->device.data(), &NetworkManager::WirelessDevice::wirelessCapabilitiesChanged, this, &WifiDevicePane::updateState);
    updateState();

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
        if (key.startsWith("NetworkPlugin/tethering")) {
            updateState();
        }
    });
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
                    QString text;
                    QString connUuid = d->settings.value("NetworkPlugin/tethering.uuid").toString();

                    if (d->device->activeConnection()->uuid() == connUuid) {
                        StateManager::hudManager()->showHud({
                            {"icon", "network-wireless-tethered"},
                            {"title", tr("Tethering")},
                            {"text", tr("Active")}
                        });
                    } else if (d->device->activeAccessPoint()) {
                        StateManager::hudManager()->showHud({
                            {"icon", "network-wireless-connected-100"},
                            {"title", d->device->activeAccessPoint()->ssid()},
                            {"text", tr("Connected")}
                        });
                    } else {
                        StateManager::hudManager()->showHud({
                            {"icon", "network-wireless-connected-100"},
                            {"title", tr("Wi-Fi")},
                            {"text", tr("Connected")}
                        });
                    }

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

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::wirelessEnabledChanged, this, &WifiDevicePane::updateNetworkCardState);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::wirelessHardwareEnabledChanged, this, &WifiDevicePane::updateNetworkCardState);
    updateNetworkCardState();
}

WifiDevicePane::~WifiDevicePane() {
    delete d->item;
    d->tetheringChunk->deleteLater();
    delete d;
    delete ui;
}

void WifiDevicePane::setSwitchManager(SwitchManager* switchManager) {
    d->switchManager = switchManager;
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

    if (d->device->wirelessCapabilities() & NetworkManager::WirelessDevice::ApCap || d->device->wirelessCapabilities() & NetworkManager::WirelessDevice::AdhocCap) {
        ui->tetheringWidget->setVisible(true);
        ui->tetheringLine->setVisible(true);

        QString ssid = d->settings.value("NetworkPlugin/tethering.ssid").toString();
        ssid = ssid.replace("$hostname", QHostInfo::localHostName());
        ui->tetheringSsid->setText(ssid);

        QString key = d->settings.value("NetworkPlugin/tethering.key").toString();
        if (key == ".") {
            QString validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()";
            QRandomGenerator* random = QRandomGenerator::global();
            key.clear();
            while (key.length() < 10) {
                key.append(validChars.at(random->bounded(validChars.length())));
            }
            d->settings.setValue("NetworkPlugin/tethering.key", key);
        }
        ui->tetheringKey->setText(key);

        bool tetheringOn = false;
        QSignalBlocker blocker(ui->tetheringSwitch);
        QString connUuid = d->settings.value("NetworkPlugin/tethering.uuid").toString();
        for (NetworkManager::ActiveConnection::Ptr connection : NetworkManager::activeConnections()) {
            if (connection->connection()->uuid() == connUuid) {
                tetheringOn = true;
            }
        }

        ui->tetheringSwitch->setChecked(tetheringOn);
        ui->tetheringSettings->setVisible(!tetheringOn);
        ui->actionsWidget->setVisible(!tetheringOn);
        ui->actionsLine->setVisible(!tetheringOn);

        if (tetheringOn && !StateManager::barManager()->isChunkRegistered(d->tetheringChunk)) {
            StateManager::barManager()->addChunk(d->tetheringChunk);
        } else if (!tetheringOn && StateManager::barManager()->isChunkRegistered(d->tetheringChunk)) {
            StateManager::barManager()->removeChunk(d->tetheringChunk);
        }
    } else {
        ui->tetheringWidget->setVisible(false);
        ui->tetheringLine->setVisible(false);
        ui->actionsWidget->setVisible(true);
        ui->actionsLine->setVisible(true);
        if (StateManager::barManager()->isChunkRegistered(d->tetheringChunk)) d->tetheringChunk->setVisible(false);
    }
}

void WifiDevicePane::updateNetworkCardState() {
    if (!NetworkManager::isWirelessHardwareEnabled()) {
        ui->stackedWidget->setCurrentWidget(ui->wifiBlockedPage);
    } else if (!NetworkManager::isWirelessEnabled()) {
        if (d->switchManager && d->switchManager->isFlightModeEnabled()) {
            ui->stackedWidget->setCurrentWidget(ui->wifiFlightPage);
        } else {
            ui->stackedWidget->setCurrentWidget(ui->wifiOffPage);
        }
    } else {
        ui->stackedWidget->setCurrentWidget(ui->mainPage);
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

void WifiDevicePane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

void WifiDevicePane::on_tetheringSwitch_toggled(bool checked) {
    if (checked) {
        NetworkManager::ConnectionSettings::Ptr settings(new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless));
        settings->setId("Tethering");
        settings->setAutoconnect(false);
        settings->setInterfaceName(d->device->interfaceName());

        NetworkManager::WirelessSetting::Ptr wirelessSettings = settings->setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>();
        NetworkManager::WirelessSecuritySetting::Ptr wirelessSecuritySettings = settings->setting(NetworkManager::Setting::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>();
        NetworkManager::Ipv4Setting::Ptr ipv4 = settings->setting(NetworkManager::Setting::Ipv4).staticCast<NetworkManager::Ipv4Setting>();
        NetworkManager::Ipv6Setting::Ptr ipv6 = settings->setting(NetworkManager::Setting::Ipv6).staticCast<NetworkManager::Ipv6Setting>();

        QString ssid = d->settings.value("NetworkPlugin/tethering.ssid").toString();
        ssid = ssid.replace("$hostname", QHostInfo::localHostName());
        wirelessSettings->setSsid(ssid.toUtf8());
        wirelessSettings->setSecurity("802-11-wireless-security");

        if (d->device->wirelessCapabilities() & NetworkManager::WirelessDevice::ApCap) {
            wirelessSettings->setMode(NetworkManager::WirelessSetting::Ap);
        } else {
            wirelessSettings->setMode(NetworkManager::WirelessSetting::Adhoc);
        }
        wirelessSettings->setInitialized(true);

        wirelessSecuritySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
        wirelessSecuritySettings->setGroup({NetworkManager::WirelessSecuritySetting::Ccmp});
        wirelessSecuritySettings->setPairwise({NetworkManager::WirelessSecuritySetting::Ccmp});
        wirelessSecuritySettings->setProto({NetworkManager::WirelessSecuritySetting::Rsn});
        wirelessSecuritySettings->setPsk(d->settings.value("NetworkPlugin/tethering.key").toString());
        wirelessSecuritySettings->setInitialized(true);

        ipv4->setMethod(NetworkManager::Ipv4Setting::Shared);
        ipv4->setInitialized(true);
        ipv6->setMethod(NetworkManager::Ipv6Setting::Ignored);
        ipv6->setInitialized(true);

        QString connUuid = d->settings.value("NetworkPlugin/tethering.uuid").toString();
        NetworkManager::Connection::Ptr connection;
        if (connUuid == ".") {
            connUuid = NetworkManager::ConnectionSettings::createNewUuid();
            d->settings.setValue("NetworkPlugin/tethering.uuid", connUuid);
        } else {
            for (NetworkManager::Connection::Ptr checkConnection : NetworkManager::listConnections()) {
                if (checkConnection->uuid() == connUuid) connection = checkConnection;
            }
        }

        settings->setUuid(connUuid);

        QDBusPendingCallWatcher* watcher;
        if (!connection) {
            watcher = new QDBusPendingCallWatcher(NetworkManager::addConnectionUnsaved(settings->toMap()));
        } else {
            watcher = new QDBusPendingCallWatcher(connection->updateUnsaved(settings->toMap()));
        }
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
            if (watcher->isError()) {
                tToast* toast = new tToast();
                toast->setTitle(tr("Couldn't configure tethering"));
                toast->setText(watcher->error().message());
                connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
                toast->show(this);

                updateState();
            } else {
                QString connectionPath;
                if (connection) {
                    connectionPath = connection->path();
                } else {
                    connectionPath = watcher->reply().arguments().first().value<QDBusObjectPath>().path();
                }

                NetworkManager::activateConnection(connectionPath, d->device->uni(), "");
            }
            watcher->deleteLater();
        });
    } else {
        QString connUuid = d->settings.value("NetworkPlugin/tethering.uuid").toString();
        for (NetworkManager::ActiveConnection::Ptr connection : NetworkManager::activeConnections()) {
            if (connection->connection()->uuid() == connUuid) {
                QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(connection->connection()->remove());
                connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
                    if (watcher->isError()) {
                        tToast* toast = new tToast();
                        toast->setTitle(tr("Couldn't switch off tethering"));
                        toast->setText(watcher->error().message());
                        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
                        toast->show(this);
                    }
                    updateState();
                    watcher->deleteLater();
                });
            }
        }
    }
}

void WifiDevicePane::on_turnWifiOnButton_clicked() {
    NetworkManager::setWirelessEnabled(true);
}

void WifiDevicePane::on_turnWifiOnButton_2_clicked() {
    NetworkManager::setWirelessEnabled(true);
}
