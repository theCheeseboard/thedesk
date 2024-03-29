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
#include "networkchunk.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QIcon>
#include <QTimer>

#include <actionquickwidget.h>
#include <barmanager.h>
#include <statemanager.h>
#include <statuscentermanager.h>

#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Manager>

#include "genericchunkupdater.h"
#include "wiredchunkupdater.h"
#include "wirelesschunkupdater.h"

struct NetworkChunkPrivate {
        bool chunkShown = false;
        QDBusServiceWatcher* nmWatcher;

        NetworkManager::ActiveConnection::Ptr primary;

        ChunkUpdater* updater = nullptr;
        NetworkManager::AccessPoint::Ptr ap;
};

NetworkChunk::NetworkChunk() :
    IconTextChunk("Network") {
    d = new NetworkChunkPrivate();

    d->nmWatcher = new QDBusServiceWatcher("org.freedesktop.NetworkManager", QDBusConnection::systemBus());
    connect(d->nmWatcher, &QDBusServiceWatcher::serviceRegistered, this, &NetworkChunk::networkManagerRunning);
    connect(d->nmWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &NetworkChunk::networkManagerGone);

    if (QDBusConnection::systemBus().interface()->registeredServiceNames().value().contains("org.freedesktop.NetworkManager")) networkManagerRunning();

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::primaryConnectionChanged, this, &NetworkChunk::updatePrimaryConnection);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::connectivityChanged, this, &NetworkChunk::updateText);

    ActionQuickWidget* quickWidget = new ActionQuickWidget(this);
    quickWidget->addAction(QIcon::fromTheme("configure"), tr("Network Settings"), [=] {
        StateManager::statusCenterManager()->showWithPane("NetworkManagerPane");
    });
    this->setQuickWidget(quickWidget);
}

NetworkChunk::~NetworkChunk() {
    networkManagerGone();
    delete d;
}

void NetworkChunk::networkManagerRunning() {
    if (!d->chunkShown) {
        StateManager::barManager()->addChunk(this);
        d->chunkShown = true;

        updatePrimaryConnection();
    }
}

void NetworkChunk::networkManagerGone() {
    if (d->chunkShown) {
        StateManager::barManager()->removeChunk(this);
        d->chunkShown = false;
    }
}

void NetworkChunk::updatePrimaryConnection() {
    if (d->updater) {
        d->updater->deleteLater();
        d->updater = nullptr;
    }

    d->primary = NetworkManager::primaryConnection();

    if (d->primary.isNull()) {
        // No primary connection exists
        QTimer::singleShot(0, [=] {
            this->setIcon(QIcon::fromTheme("network-wired-unavailable"));
            this->setText(tr("Disconnected"));
        });
        return;
    }

    connect(d->primary.data(), &NetworkManager::ActiveConnection::specificObjectChanged, this, &NetworkChunk::updatePrimaryConnection);

    switch (d->primary->type()) {
        case NetworkManager::ConnectionSettings::Wireless:
            d->updater = new WirelessChunkUpdater(this);
            break;
        case NetworkManager::ConnectionSettings::Wired:
            d->updater = new WiredChunkUpdater(this);
            break;
        default:
            d->updater = new GenericChunkUpdater(this);
            break;
    }

    connect(d->updater, &ChunkUpdater::iconChanged, this, &NetworkChunk::setIcon);
    connect(d->updater, &ChunkUpdater::textChanged, this, &NetworkChunk::updateText);
    this->setIcon(d->updater->icon());
    this->setText(d->updater->text());
}

void NetworkChunk::updateText() {
    // Keep the text as is if there is no updater; we're disconnected!
    if (!d->updater) return;

    QStringList text;
    text.append(d->updater->text());

    switch (NetworkManager::connectivity()) {
        case NetworkManager::Portal:
            text.append(tr("Login Required"));
            break;
        case NetworkManager::Limited:
            text.append(tr("Can't get to the Internet"));
            break;
        case NetworkManager::NoConnectivity:
        case NetworkManager::UnknownConnectivity:
        case NetworkManager::Full:
            break;
    }

    this->setText(text.join(" · "));
}
