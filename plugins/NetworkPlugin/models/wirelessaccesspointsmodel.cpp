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
#include "wirelessaccesspointsmodel.h"

#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>

struct WirelessAccessPointsModelPrivate {
        NetworkManager::WirelessDevice::Ptr device;
        QList<NetworkManager::AccessPoint::Ptr> displayedAPs;

        bool includeKnown;
};

WirelessAccessPointsModel::WirelessAccessPointsModel(QString deviceUni, bool includeKnown, QObject* parent) :
    QAbstractListModel(parent) {
    d = new WirelessAccessPointsModelPrivate();

    d->includeKnown = includeKnown;
    d->device = NetworkManager::findNetworkInterface(deviceUni).staticCast<NetworkManager::WirelessDevice>();
    d->device->requestScan();

    connect(d->device.data(), &NetworkManager::WirelessDevice::accessPointAppeared, this, &WirelessAccessPointsModel::addAp);
    connect(d->device.data(), &NetworkManager::WirelessDevice::accessPointDisappeared, this, &WirelessAccessPointsModel::removeAp);
    for (QString ap : d->device->accessPoints()) {
        this->addAp(ap);
    }
}

WirelessAccessPointsModel::~WirelessAccessPointsModel() {
    delete d;
}

int WirelessAccessPointsModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->displayedAPs.count();
}

QVariant WirelessAccessPointsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    NetworkManager::AccessPoint::Ptr ap = d->displayedAPs.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return ap->ssid();
        case Qt::UserRole:
            return QVariant::fromValue(ap);
        case Qt::UserRole + 1:
            return "ap";
    }

    return QVariant();
}

void WirelessAccessPointsModel::addAp(QString ap) {
    NetworkManager::AccessPoint::Ptr accessPoint(new NetworkManager::AccessPoint(ap));
    if (accessPoint->ssid().isEmpty()) return;

    for (NetworkManager::AccessPoint::Ptr oldAp : d->displayedAPs) {
        if (oldAp->uni() == ap) return;
        if (oldAp->ssid() == accessPoint->ssid()) return;
    }

    if (!d->includeKnown) {
        for (NetworkManager::Connection::Ptr connection : NetworkManager::listConnections()) {
            NetworkManager::WirelessSetting::Ptr wlSetting = connection->settings()->setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>();
            if (wlSetting) {
                if (wlSetting->ssid() == accessPoint->ssid()) return;
            }
        }
    }

    d->displayedAPs.append(accessPoint);
    emit dataChanged(index(0), index(rowCount()));
}

void WirelessAccessPointsModel::removeAp(QString ap) {
    for (NetworkManager::AccessPoint::Ptr accessPoint : d->displayedAPs) {
        if (accessPoint->uni() == ap) {
            d->displayedAPs.removeOne(accessPoint);
            emit dataChanged(index(0), index(rowCount()));
            return;
        }
    }
}
