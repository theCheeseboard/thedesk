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

#include "../eap/securityeappeap.h"
#include "../eap/securityeapunsupported.h"

#include <ttoast.h>
#include <terrorflash.h>
#include <tpopover.h>

#include "models/wirelessconnectionlistmodel.h"
#include "models/wirelessaccesspointsmodel.h"
#include "models/wirelessnetworklistdelegate.h"

#include "../connectionEditor/networkconnectioneditor.h"

struct WirelessNetworkSelectionPopoverPrivate {
    NetworkManager::WirelessDevice::Ptr device;
    WirelessConnectionListModel* knownNetworksModel;
    WirelessAccessPointsModel* newNetworksModel;

    NetworkManager::AccessPoint::Ptr apConnect;
    NetworkManager::WirelessSecurityType connectWithSecurity;

    SecurityEap* eapPane = nullptr;
    NetworkConnectionEditor* editor = nullptr;
};

WirelessNetworkSelectionPopover::WirelessNetworkSelectionPopover(QString deviceUni, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::WirelessNetworkSelectionPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->keyTitleLabel->setBackButtonShown(true);
    ui->eapMethodTitleLabel->setBackButtonShown(true);

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
        //Connect to a new AP
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
            case NetworkManager::SAE:
                ui->securityNetworkName->setText(d->apConnect->ssid());
                ui->networkKey->clear();
                ui->stackedWidget->setCurrentWidget(ui->keyPage);
                break;
            case NetworkManager::WpaEap:
            case NetworkManager::Wpa2Eap:
                ui->eapMethodTitleLabel->setText(d->apConnect->ssid());
                ui->networkKey->clear();
                ui->stackedWidget->setCurrentWidget(ui->eapMethodPage);
                break;
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
        case NetworkManager::SAE:
            securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::SAE);
            securitySettings->setPsk(ui->networkKey->text());
            securitySettings->setInitialized(true);
            break;
        case NetworkManager::WpaEap:
        case NetworkManager::Wpa2Eap:
            securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
            securitySettings->setInitialized(true);

            d->eapPane->populateSetting(eapSettings);
            eapSettings->setInitialized(true);
            break;
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
    menu->addAction(QIcon::fromTheme("edit-rename"), tr("Edit"), [ = ] {
        if (d->editor) {
            ui->manualPage->layout()->removeWidget(d->editor);
            d->editor->deleteLater();
        }

        d->editor = new NetworkConnectionEditor(cn, this);
        ui->manualPage->layout()->addWidget(d->editor);
        connect(d->editor, &NetworkConnectionEditor::accepted, this, [ = ] {
            ui->stackedWidget->setCurrentWidget(ui->selectionPage);
        });
        connect(d->editor, &NetworkConnectionEditor::rejected, this, [ = ] {
            ui->stackedWidget->setCurrentWidget(ui->selectionPage);
        });

        ui->stackedWidget->setCurrentWidget(ui->manualPage);
    });
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
    switch (d->connectWithSecurity) {
        case NetworkManager::StaticWep:
        case NetworkManager::DynamicWep:
        case NetworkManager::SAE:
            if (ui->networkKey->text().isEmpty()) {
                tErrorFlash::flashError(ui->networkKeyContainer);
                return;
            }
            break;
        case NetworkManager::WpaPsk:
        case NetworkManager::Wpa2Psk:
            if (ui->networkKey->text().length() < 8) {
                tErrorFlash::flashError(ui->networkKeyContainer);
                return;
            }
            break;
        default:
            return;
    }

    this->createConnection();
}

void WirelessNetworkSelectionPopover::on_eapMethodTitleLabel_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->selectionPage);
}

void WirelessNetworkSelectionPopover::on_eapMethodList_activated(const QModelIndex& index) {
    if (d->eapPane) {
        ui->stackedWidget->removeWidget(d->eapPane);
        d->eapPane->deleteLater();
        d->eapPane = nullptr;
    }

    switch (index.row()) {
        case 0: //TLS
        case 1: //LEAP
        case 2: //PWD
        case 3: //FAST
        case 4: //TTLS
            d->eapPane = new SecurityEapUnsupported(this);
            break;
        case 5: //PEAP
            d->eapPane = new SecurityEapPeap(this);
    }

    if (d->eapPane) {
        ui->stackedWidget->addWidget(d->eapPane);
        ui->stackedWidget->setCurrentWidget(d->eapPane);

        connect(d->eapPane, &SecurityEap::back, this, [ = ] {
            ui->stackedWidget->setCurrentWidget(ui->eapMethodPage);
        });
        connect(d->eapPane, &SecurityEap::done, this, &WirelessNetworkSelectionPopover::createConnection);
    }
}

void WirelessNetworkSelectionPopover::on_manualButton_clicked() {
    if (d->editor) {
        ui->manualPage->layout()->removeWidget(d->editor);
        d->editor->deleteLater();
    }

    NetworkManager::ConnectionSettings::Ptr settings(new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless));
    settings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    settings->setId(tr("Wireless"));

    d->editor = new NetworkConnectionEditor(settings);
    d->editor->setSaveButtonText(tr("Connect"));
    ui->manualPage->layout()->addWidget(d->editor);
    connect(d->editor, &NetworkConnectionEditor::accepted, this, [ = ](NetworkManager::Connection::Ptr connection) {
        //Activate this connection
        NetworkManager::activateConnection(connection->path(), d->device->uni(), "");
        emit done();
    });
    connect(d->editor, &NetworkConnectionEditor::rejected, this, [ = ] {
        ui->stackedWidget->setCurrentWidget(ui->selectionPage);
    });

    ui->stackedWidget->setCurrentWidget(ui->manualPage);
}

void WirelessNetworkSelectionPopover::on_stackedWidget_switchingFrame(int frame) {
    tPopover* popover = tPopover::popoverForPopoverWidget(this);
    if (ui->stackedWidget->widget(frame) == ui->manualPage) {
        popover->setPopoverWidth(d->editor->preferredPopoverWidth());
        popover->setDismissable(false);
    } else {
        popover->setPopoverWidth(SC_DPI(600));
        popover->setDismissable(true);
    }
}
