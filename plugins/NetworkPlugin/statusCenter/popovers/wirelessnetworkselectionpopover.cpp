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
#include "wirelessnetworkselectionpopover.h"
#include "ui_wirelessnetworkselectionpopover.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Utils>
#include <QMenu>

#include "models/wirelessconnectionlistmodel.h"
#include "models/wirelessaccesspointsmodel.h"
#include "models/wirelessnetworklistdelegate.h"

struct WirelessNetworkSelectionPopoverPrivate {
    NetworkManager::WirelessDevice::Ptr device;
    WirelessConnectionListModel* knownNetworksModel;
    WirelessAccessPointsModel* newNetworksModel;
};

WirelessNetworkSelectionPopover::WirelessNetworkSelectionPopover(QString deviceUni, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::WirelessNetworkSelectionPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);

    d = new WirelessNetworkSelectionPopoverPrivate();
    d->device = NetworkManager::findNetworkInterface(deviceUni).staticCast<NetworkManager::WirelessDevice>();

    d->knownNetworksModel = new WirelessConnectionListModel();
    ui->knownNetworksListView->setModel(d->knownNetworksModel);

    connect(d->knownNetworksModel, &WirelessConnectionListModel::dataChanged, this, [ = ] {
        ui->knownNetworksListView->setFixedHeight(d->knownNetworksModel->rowCount() * ui->knownNetworksListView->sizeHintForRow(0));
    });
    ui->knownNetworksListView->setItemDelegate(new WirelessNetworkListDelegate(deviceUni));
    ui->knownNetworksListView->setFixedHeight(d->knownNetworksModel->rowCount() * ui->knownNetworksListView->sizeHintForRow(0));

    d->newNetworksModel = new WirelessAccessPointsModel(deviceUni, false);
    ui->newNetworksListView->setModel(d->newNetworksModel);

    connect(d->newNetworksModel, &WirelessAccessPointsModel::dataChanged, this, [ = ] {
        ui->newNetworksListView->setFixedHeight(d->newNetworksModel->rowCount() * ui->newNetworksListView->sizeHintForRow(0));
    });
    ui->newNetworksListView->setItemDelegate(new WirelessNetworkListDelegate(deviceUni));
    ui->newNetworksListView->setFixedHeight(d->newNetworksModel->rowCount() * ui->newNetworksListView->sizeHintForRow(0));
}

WirelessNetworkSelectionPopover::~WirelessNetworkSelectionPopover() {
    d->knownNetworksModel->deleteLater();
    d->newNetworksModel->deleteLater();
    delete d;
    delete ui;
}

void WirelessNetworkSelectionPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void WirelessNetworkSelectionPopover::on_knownNetworksListView_activated(const QModelIndex& index) {
    activateConnection(index);
}

void WirelessNetworkSelectionPopover::activateConnection(const QModelIndex& index) {
    QVariant networkInformation = index.data(Qt::UserRole);
    if (networkInformation.canConvert<NetworkManager::Connection::Ptr>()) {
        //Attempt to activate this known connection
        NetworkManager::activateConnection(networkInformation.value<NetworkManager::Connection::Ptr>()->path(), d->device->uni(), "");
        emit done();
    } else if (networkInformation.canConvert<NetworkManager::AccessPoint::Ptr>()) {
        //TODO: Connect to a new AP
        NetworkManager::AccessPoint::Ptr ap = networkInformation.value<NetworkManager::AccessPoint::Ptr>();
        NetworkManager::WirelessSecurityType security = NetworkManager::findBestWirelessSecurity(d->device->wirelessCapabilities(), true, false, ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());

    }
}

void WirelessNetworkSelectionPopover::on_knownNetworksListView_customContextMenuRequested(const QPoint& pos) {
    QModelIndex index = ui->knownNetworksListView->indexAt(pos);
    NetworkManager::Connection::Ptr cn = index.data(Qt::UserRole).value<NetworkManager::Connection::Ptr>();

    QMenu* menu = new QMenu(this);
    menu->addSection(tr("For network %1").arg(cn->name()));
    menu->addAction(QIcon::fromTheme("list-remove"), tr("Forget Network"), [ = ] {
        cn->remove();
    });
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(ui->knownNetworksListView->mapToGlobal(pos));
}
