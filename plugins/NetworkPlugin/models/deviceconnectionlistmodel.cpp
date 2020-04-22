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
#include "deviceconnectionlistmodel.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Connection>

struct DeviceConnectionListModelPrivate {
    NetworkManager::Device::Ptr device;
};

DeviceConnectionListModel::DeviceConnectionListModel(QString deviceUni, QObject* parent)
    : QAbstractListModel(parent) {
    d = new DeviceConnectionListModelPrivate();
    d->device = NetworkManager::findNetworkInterface(deviceUni);
}

DeviceConnectionListModel::~DeviceConnectionListModel() {
    delete d;
}

int DeviceConnectionListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->device->availableConnections().count();
}

QVariant DeviceConnectionListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    NetworkManager::Connection::Ptr connection = d->device->availableConnections().at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return connection->name();
        case Qt::UserRole:
            return QVariant::fromValue(connection);
        case Qt::UserRole + 1:
            return "connection";
    }

    return QVariant();
}
