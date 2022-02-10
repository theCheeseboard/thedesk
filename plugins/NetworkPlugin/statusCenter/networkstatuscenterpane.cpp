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
#include "networkstatuscenterpane.h"
#include "ui_networkstatuscenterpane.h"

#include "networkstatuscenterleftpane.h"
#include <QIcon>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>

#include "devicePanes/devicepane.h"
#include "devicePanes/wifidevicepane.h"
#include "devicePanes/wireddevicepane.h"
#include "devicePanes/cellularpane.h"

#include <statemanager.h>
#include <statuscentermanager.h>

#include <NetworkManagerQt/Manager>

struct NetworkStatusCenterPanePrivate {
    NetworkStatusCenterLeftPane* leftPane;
    QDBusServiceWatcher* nmWatcher;
    SwitchManager* switchManager;

    QStringList unis;
    QStringList visibleUnis;
    QMap<QString, AbstractDevicePane*> devicePanes;
};

NetworkStatusCenterPane::NetworkStatusCenterPane(SwitchManager* switchManager) :
    StatusCenterPane(),
    ui(new Ui::NetworkStatusCenterPane) {
    ui->setupUi(this);

    d = new NetworkStatusCenterPanePrivate();
    d->switchManager = switchManager;

    ui->menuButtonErrorPage->setVisible(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->menuButtonErrorPage, &QToolButton::setVisible);

    d->leftPane = new NetworkStatusCenterLeftPane();
    connect(d->leftPane, &NetworkStatusCenterLeftPane::currentChanged, this, [ = ](int index) {
        ui->devicesStack->setCurrentIndex(index);
    });

    ui->devicesStack->setCurrentAnimation(tStackedWidget::Lift);

    d->nmWatcher = new QDBusServiceWatcher("org.freedesktop.NetworkManager", QDBusConnection::systemBus());
    connect(d->nmWatcher, &QDBusServiceWatcher::serviceRegistered, this, &NetworkStatusCenterPane::networkManagerRunning);
    connect(d->nmWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &NetworkStatusCenterPane::networkManagerGone);

    if (QDBusConnection::systemBus().interface()->registeredServiceNames().value().contains("org.freedesktop.NetworkManager")) {
        networkManagerRunning();
    } else {
        networkManagerGone();
    }

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceAdded, this, &NetworkStatusCenterPane::deviceAdded);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceRemoved, this, &NetworkStatusCenterPane::deviceRemoved);
}

NetworkStatusCenterPane::~NetworkStatusCenterPane() {
    d->leftPane->deleteLater();
    delete d;
    delete ui;
}

void NetworkStatusCenterPane::networkManagerRunning() {
    ui->stackedWidget->setCurrentWidget(ui->activePage);

    for (NetworkManager::Device::Ptr device : NetworkManager::networkInterfaces()) {
        this->deviceAdded(device->uni());
    }
}

void NetworkStatusCenterPane::networkManagerGone() {
    QStringList unis = d->unis;
    for (QString str : unis) {
        this->deviceRemoved(str);
    }

    ui->stackedWidget->setCurrentWidget(ui->errorPage);
}

void NetworkStatusCenterPane::deviceAdded(QString uni) {
    d->unis.append(uni);

    AbstractDevicePane* devicePane;
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(uni);

    switch (device->type()) {
        case NetworkManager::Device::Wifi:
            devicePane = new WifiDevicePane(uni);
            static_cast<WifiDevicePane*>(devicePane)->setSwitchManager(d->switchManager);
            break;
        case NetworkManager::Device::Ethernet:
            devicePane = new WiredDevicePane(uni);
            break;
        case NetworkManager::Device::Modem:
            devicePane = new CellularPane(uni);
            break;
        default:
            devicePane = new DevicePane(uni);
    }

    auto managedChanged = [ = ] {
        if (device->managed() && !d->visibleUnis.contains(uni)) {
            d->visibleUnis.append(uni);
            d->leftPane->addItem(devicePane->leftPaneItem());
            ui->devicesStack->addWidget(devicePane);
        } else if (!device->managed() && d->visibleUnis.contains(uni)) {
            d->visibleUnis.removeOne(uni);
            d->leftPane->removeItem(devicePane->leftPaneItem());
            ui->devicesStack->removeWidget(devicePane);
        }
    };
    connect(device.data(), &NetworkManager::Device::managedChanged, this, managedChanged);
    managedChanged();
    d->devicePanes.insert(uni, devicePane);
}

void NetworkStatusCenterPane::deviceRemoved(QString uni) {
    AbstractDevicePane* devicePane = d->devicePanes.value(uni);
    d->unis.removeOne(uni);
    if (d->visibleUnis.contains(uni)) {
        d->leftPane->removeItem(devicePane->leftPaneItem());
        ui->devicesStack->removeWidget(devicePane);
        d->visibleUnis.removeOne(uni);
    }

    devicePane->deleteLater();
    d->devicePanes.remove(uni);
}

QString NetworkStatusCenterPane::name() {
    return "NetworkManagerPane";
}

QString NetworkStatusCenterPane::displayName() {
    return tr("Network");
}

QIcon NetworkStatusCenterPane::icon() {
    return QIcon::fromTheme("preferences-system-network");
}

QWidget* NetworkStatusCenterPane::leftPane() {
    return d->leftPane;
}

void NetworkStatusCenterPane::on_menuButtonErrorPage_clicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}


void NetworkStatusCenterPane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}
