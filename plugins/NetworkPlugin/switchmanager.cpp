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
#include "switchmanager.h"

#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QDBusConnectionInterface>
#include <QDBusInterface>

#include <statemanager.h>
#include <statuscentermanager.h>
#include <quickswitch.h>

#include <tsettings.h>

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>

struct SwitchManagerPrivate {
    QDBusServiceWatcher* nmWatcher;
    QDBusInterface* interface;

    QuickSwitch* flightModeSwitch;
    QuickSwitch* wifiSwitch;
    QuickSwitch* cellularSwitch;

    tSettings settings;
    bool block = false;
};

SwitchManager::SwitchManager(QObject* parent) : QObject(parent) {
    d = new SwitchManagerPrivate();

    d->interface = new QDBusInterface("org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager", "org.freedesktop.DBus.Properties", QDBusConnection::systemBus(), this);

    d->flightModeSwitch = new QuickSwitch("NetworkFlight");
    d->wifiSwitch = new QuickSwitch("NetworkWireless");
    d->cellularSwitch = new QuickSwitch("NetworkCellular");

    d->flightModeSwitch->setTitle(tr("Flight Mode"));
    connect(d->flightModeSwitch, &QuickSwitch::toggled, this, &SwitchManager::setFlightMode);
    if (d->settings.value("NetworkPlugin/flight.on").toBool()) {
        this->setFlightMode(true);
    }

    d->wifiSwitch->setTitle(tr("Wi-Fi"));
    d->wifiSwitch->setChecked(NetworkManager::isWirelessEnabled());
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::wirelessEnabledChanged, this, [ = ](bool enabled) {
        QSignalBlocker blocker(d->wifiSwitch);
        d->wifiSwitch->setChecked(enabled);
    });
    connect(d->wifiSwitch, &QuickSwitch::toggled, this, [ = ](bool enabled) {
        d->interface->asyncCall("Set", "org.freedesktop.NetworkManager", "WirelessEnabled", QVariant::fromValue(QDBusVariant(enabled)));
    });

    d->cellularSwitch->setTitle(tr("Cellular"));
    d->wifiSwitch->setChecked(NetworkManager::isWwanEnabled());
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::wwanEnabledChanged, this, [ = ](bool enabled) {
        QSignalBlocker blocker(d->cellularSwitch);
        d->cellularSwitch->setChecked(enabled);
    });
    connect(d->wifiSwitch, &QuickSwitch::toggled, this, [ = ](bool enabled) {
        d->interface->asyncCall("Set", "org.freedesktop.NetworkManager", "WwanEnabled", QVariant::fromValue(QDBusVariant(enabled)));
    });

    d->nmWatcher = new QDBusServiceWatcher("org.freedesktop.NetworkManager", QDBusConnection::systemBus());
    connect(d->nmWatcher, &QDBusServiceWatcher::serviceRegistered, this, &SwitchManager::networkManagerRunning);
    connect(d->nmWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &SwitchManager::networkManagerGone);

    if (QDBusConnection::systemBus().interface()->registeredServiceNames().value().contains("org.freedesktop.NetworkManager")) networkManagerRunning();

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceAdded, this, &SwitchManager::updateDevices);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceRemoved, this, &SwitchManager::updateDevices);
}

SwitchManager::~SwitchManager() {
    d->nmWatcher->deleteLater();
    d->flightModeSwitch->deleteLater();
    d->wifiSwitch->deleteLater();
    d->cellularSwitch->deleteLater();
    delete d;
}

void SwitchManager::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        d->flightModeSwitch->setTitle(tr("Flight Mode"));
        d->wifiSwitch->setTitle(tr("Wi-Fi"));
        d->cellularSwitch->setTitle(tr("Cellular"));
    }
}

void SwitchManager::networkManagerRunning() {
    StateManager::statusCenterManager()->addSwitch(d->flightModeSwitch);
    updateDevices();
}

void SwitchManager::networkManagerGone() {
    StatusCenterManager* mgr = StateManager::statusCenterManager();
    mgr->removeSwitch(d->flightModeSwitch);
    mgr->removeSwitch(d->wifiSwitch);
    mgr->removeSwitch(d->cellularSwitch);
}

void SwitchManager::updateDevices() {
    StatusCenterManager* mgr = StateManager::statusCenterManager();
    bool wifi = false;
    bool cellular = false;
    for (NetworkManager::Device::Ptr device : NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) wifi = true;
        if (device->type() == NetworkManager::Device::Modem) cellular = true;
    }

    if (wifi) {
        mgr->addSwitch(d->wifiSwitch);
    } else {
        mgr->removeSwitch(d->wifiSwitch);
    }

    if (cellular) {
        mgr->addSwitch(d->cellularSwitch);
    } else {
        mgr->removeSwitch(d->cellularSwitch);
    }
}

void SwitchManager::setFlightMode(bool flightOn) {
    QSignalBlocker blocker(d->flightModeSwitch);
    d->flightModeSwitch->setChecked(flightOn);

    if (flightOn) {
        d->settings.setValue("NetworkPlugin/flight.wifi", NetworkManager::isWirelessEnabled());
        d->settings.setValue("NetworkPlugin/flight.cellular", NetworkManager::isWwanEnabled());
        d->settings.setValue("NetworkPlugin/flight.on", true);

        d->interface->asyncCall("Set", "org.freedesktop.NetworkManager", "WirelessEnabled", QVariant::fromValue(QDBusVariant(false)));
        d->interface->asyncCall("Set", "org.freedesktop.NetworkManager", "WwanEnabled", QVariant::fromValue(QDBusVariant(false)));
    } else {
        d->interface->asyncCall("Set", "org.freedesktop.NetworkManager", "WirelessEnabled", QVariant::fromValue(QDBusVariant(d->settings.value("NetworkPlugin/flight.wifi").toBool())));
        d->interface->asyncCall("Set", "org.freedesktop.NetworkManager", "WwanEnabled", QVariant::fromValue(QDBusVariant(d->settings.value("NetworkPlugin/flight.cellular").toBool())));

        d->settings.setValue("NetworkPlugin/flight.on", false);
    }
}
