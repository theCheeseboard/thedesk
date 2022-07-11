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
#include "wirelesschunkupdater.h"

#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Manager>

#include "networkplugincommon.h"

struct WirelessChunkUpdaterPrivate {
        NetworkManager::AccessPoint::Ptr ap;
};

WirelessChunkUpdater::WirelessChunkUpdater(QObject* parent) :
    ChunkUpdater(parent) {
    d = new WirelessChunkUpdaterPrivate();

    NetworkManager::ActiveConnection::Ptr connection = NetworkManager::primaryConnection();
    d->ap.reset(new NetworkManager::AccessPoint(connection->specificObject()));

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::connectivityChanged, this, &WirelessChunkUpdater::updateChunk);
    connect(d->ap.data(), &NetworkManager::AccessPoint::ssidChanged, this, &WirelessChunkUpdater::updateChunk);
    connect(d->ap.data(), &NetworkManager::AccessPoint::signalStrengthChanged, this, &WirelessChunkUpdater::updateChunk);

    updateChunk();
}

WirelessChunkUpdater::~WirelessChunkUpdater() {
    delete d;
}

void WirelessChunkUpdater::updateChunk() {
    this->setText(d->ap->ssid());

    NetworkPluginCommon::WirelessType type = NetworkPluginCommon::WiFi;
    if (NetworkManager::connectivity() != NetworkManager::Full) type = NetworkPluginCommon::WiFiError;
    this->setIcon(QIcon::fromTheme(NetworkPluginCommon::iconForSignalStrength(d->ap->signalStrength(), type)));
}
