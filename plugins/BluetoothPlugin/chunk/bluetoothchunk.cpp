/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "bluetoothchunk.h"

#include <statemanager.h>
#include <barmanager.h>
#include <QIcon>
#include <BluezQt/Device>

struct BluetoothChunkPrivate {
    BluezQt::ManagerPtr manager;
};

BluetoothChunk::BluetoothChunk(BluezQt::ManagerPtr manager) : IconTextChunk("bluetooth") {
    d = new BluetoothChunkPrivate();
    d->manager = manager;

    this->setIcon(QIcon::fromTheme("bluetooth"));

    connect(manager.data(), &BluezQt::Manager::deviceAdded, this, &BluetoothChunk::updateChunk);
    connect(manager.data(), &BluezQt::Manager::deviceRemoved, this, &BluetoothChunk::updateChunk);
    connect(manager.data(), &BluezQt::Manager::deviceChanged, this, &BluetoothChunk::updateChunk);
    updateChunk();
}

BluetoothChunk::~BluetoothChunk() {
    if (StateManager::barManager()->isChunkRegistered(this)) StateManager::barManager()->removeChunk(this);
    delete d;
}

void BluetoothChunk::updateChunk() {
    QVector<BluezQt::DevicePtr> activeDevices;
    for (const BluezQt::DevicePtr& device : d->manager->devices()) {
        if (device->isConnected()) activeDevices.append(device);
    }

    if (activeDevices.isEmpty()) {
        if (StateManager::barManager()->isChunkRegistered(this)) StateManager::barManager()->removeChunk(this);
    } else {
        if (activeDevices.count() == 1) {
            this->setText(activeDevices.first()->name());
        } else {
            this->setText(tr("%n devices connected", nullptr, activeDevices.count()));
        }

        if (!StateManager::barManager()->isChunkRegistered(this)) StateManager::barManager()->addChunk(this);
    }
}
