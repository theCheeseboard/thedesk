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
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Security8021xSetting>
#include <QMenu>

#include <ttoast.h>

#include "models/wirelessconnectionlistmodel.h"
#include "models/wirelessaccesspointsmodel.h"
#include "models/wirelessnetworklistdelegate.h"

struct WirelessNetworkSelectionPopoverPrivate {
    NetworkManager::WirelessDevice::Ptr device;
    WirelessConnectionListModel* knownNetworksModel;
    WirelessAccessPointsModel* newNetworksModel;

    NetworkManager::AccessPoint::Ptr apConnect;
    NetworkManager::WirelessSecurityType connectWithSecurity;
};

WirelessNetworkSelectionPopover::WirelessNetworkSelectionPopover(QString deviceUni, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::WirelessNetworkSelectionPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->keyTitleLabel->setBackButtonShown(true);

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

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
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
        d->apConnect = networkInformation.value<NetworkManager::AccessPoint::Ptr>();
        d->connectWithSecurity = NetworkManager::findBestWirelessSecurity(d->device->wirelessCapabilities(), true, false, d->apConnect->capabilities(), d->apConnect->wpaFlags(), d->apConnect->rsnFlags());

        switch (d->connectWithSecurity) {
            case NetworkManager::NoneSecurity: {
                //Attempt to activate a new connection
                createConnection();
                break;
            }
            case NetworkManager::StaticWep:
            case NetworkManager::DynamicWep:
            case NetworkManager::WpaPsk:
            case NetworkManager::Wpa2Psk:
                ui->securityNetworkName->setText(d->apConnect->ssid());
                ui->stackedWidget->setCurrentWidget(ui->keyPage);
                break;
            case NetworkManager::WpaEap:
            case NetworkManager::Wpa2Eap:
//                break;
            case NetworkManager::SAE:
            case NetworkManager::Leap:
            case NetworkManager::UnknownSecurity:
            default: {
                tToast* toast = new tToast(this);
                toast->setTitle(tr("Unknown Security"));
                toast->setText(tr("Not sure how to connect to this network. Use manual setup to connect to it."));
                connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
                toast->show(this);
                break;
            }
        }
    }
}

void WirelessNetworkSelectionPopover::createConnection() {
    NetworkManager::ConnectionSettings settings(NetworkManager::ConnectionSettings::Wireless);
    NetworkManager::WirelessSetting::Ptr wirelessSettings = settings.setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>();
    NetworkManager::WirelessSecuritySetting::Ptr securitySettings = settings.setting(NetworkManager::Setting::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>();
    NetworkManager::Security8021xSetting::Ptr eapSettings = settings.setting(NetworkManager::Setting::Security8021x).staticCast<NetworkManager::Security8021xSetting>();

    settings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());

    wirelessSettings->setSsid(d->apConnect->ssid().toUtf8());

    switch (d->connectWithSecurity) {
        case NetworkManager::NoneSecurity:
            //Don't need to set anything special here
            break;
        case NetworkManager::StaticWep:
        case NetworkManager::DynamicWep:
            securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::Wep);
            securitySettings->setAuthAlg(NetworkManager::WirelessSecuritySetting::Shared);
            securitySettings->setWepKey0(ui->networkKey->text());
            securitySettings->setInitialized(true);
            break;
        case NetworkManager::WpaPsk:
        case NetworkManager::Wpa2Psk:
            securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
            securitySettings->setPsk(ui->networkKey->text());
            securitySettings->setInitialized(true);
            break;
        case NetworkManager::WpaEap:
        case NetworkManager::Wpa2Eap:
            securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
            securitySettings->setInitialized(true);
//                break;
        case NetworkManager::SAE:
        case NetworkManager::Leap:
        case NetworkManager::UnknownSecurity:
        default:
            return;
    }

    wirelessSettings->setInitialized(true);
//    securitySettings->setInitialized(true);
//    eapSettings->setInitialized(true);

    qDebug() << settings;
    qDebug() << settings.toMap();

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> r = NetworkManager::addAndActivateConnection(settings.toMap(), d->device->uni(), "");
    r.waitForFinished();
    if (r.isError()) {
        qDebug() << r.error().name() << r.error().message();
    }
    emit done();
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

void WirelessNetworkSelectionPopover::on_keyTitleLabel_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->selectionPage);
}

void WirelessNetworkSelectionPopover::on_newNetworksListView_activated(const QModelIndex& index) {
    activateConnection(index);
}

void WirelessNetworkSelectionPopover::on_securityConnectButton_clicked() {
    this->createConnection();
}
