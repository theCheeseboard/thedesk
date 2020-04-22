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
#include "wirelessconnectionlistmodel.h"

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/Settings>

struct WirelessConnectionListModelPrivate {
    QList<NetworkManager::Connection::Ptr> connections;
};

WirelessConnectionListModel::WirelessConnectionListModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new WirelessConnectionListModelPrivate();

    connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionAdded, this, &WirelessConnectionListModel::newConnection);
    connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionRemoved, this, &WirelessConnectionListModel::removeConnection);
    for (NetworkManager::Connection::Ptr connection : NetworkManager::listConnections()) {
        this->newConnection(connection->path());
    }
}

WirelessConnectionListModel::~WirelessConnectionListModel() {
    delete d;
}

int WirelessConnectionListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return d->connections.count();
}

QVariant WirelessConnectionListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    NetworkManager::Connection::Ptr cn = d->connections.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return cn->name();
        case Qt::UserRole:
            return QVariant::fromValue(cn);
        case Qt::UserRole + 1:
            return "connection";
    }

    return QVariant();
}

void WirelessConnectionListModel::newConnection(QString connection) {
    NetworkManager::Connection::Ptr cn(new NetworkManager::Connection(connection));
    NetworkManager::WirelessSetting::Ptr wlSetting = cn->settings()->setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>();
    if (!wlSetting) return;

    d->connections.append(cn);
    emit dataChanged(index(0), index(rowCount()));
}

void WirelessConnectionListModel::removeConnection(QString connection) {
    for (NetworkManager::Connection::Ptr cn : d->connections) {
        if (cn->path() == connection) {
            d->connections.removeOne(cn);
            emit dataChanged(index(0), index(rowCount()));
            return;
        }
    }
}
