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
#include "ipv4connectioneditorpane.h"
#include "ui_ipv4connectioneditorpane.h"

#include <QMenu>
#include <terrorflash.h>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/Ipv4Setting>

struct IPv4ConnectionEditorPanePrivate {
    NetworkManager::Ipv4Setting::Ptr setting;
};

IPv4ConnectionEditorPane::IPv4ConnectionEditorPane(NetworkManager::Setting::Ptr setting, QWidget* parent) :
    ConnectionEditorPane(setting, parent),
    ui(new Ui::IPv4ConnectionEditorPane) {
    ui->setupUi(this);
    d = new IPv4ConnectionEditorPanePrivate();

    this->reload(setting);
}

IPv4ConnectionEditorPane::~IPv4ConnectionEditorPane() {
    delete d;
    delete ui;
}

void IPv4ConnectionEditorPane::updateDns() {
    ui->dnsList->clear();
    for (QHostAddress address : d->setting->dns()) {
        QListWidgetItem* w = new QListWidgetItem();
        w->setText(address.toString());
        ui->dnsList->addItem(w);
    }
}

void IPv4ConnectionEditorPane::setAddresses() {
    d->setting->setAddressData({{
            {"address", ui->ipAddress->text()},
            {"prefix", static_cast<uint>(ui->ipSubnet->value())}
        }});
    emit changed();
}

QString IPv4ConnectionEditorPane::displayName() {
    return tr("IPv4");
}

void IPv4ConnectionEditorPane::on_configurationMethod_currentIndexChanged(int index) {
    bool configureAddresses = false;
    bool configureDns = false;

    switch (index) {
        case 0: //DHCP
            d->setting->setMethod(NetworkManager::Ipv4Setting::Automatic);
            d->setting->setIgnoreAutoDns(false);
            break;
        case 1: //DHCP, Manual DNS
            d->setting->setMethod(NetworkManager::Ipv4Setting::Automatic);
            d->setting->setIgnoreAutoDns(true);
            configureDns = true;
            break;
        case 2: //Manual
            d->setting->setMethod(NetworkManager::Ipv4Setting::Manual);
            configureAddresses = true;
            configureDns = true;
            break;
        case 3: //Shared
            d->setting->setMethod(NetworkManager::Ipv4Setting::Shared);
            break;
        case 4: //Disabled
            d->setting->setMethod(NetworkManager::Ipv4Setting::Disabled);
            break;
    }

    ui->addressesWidget->setExpanded(configureAddresses);
    ui->dnsWidget->setExpanded(configureDns);
    emit changed();
}

void IPv4ConnectionEditorPane::on_requireIPv4Switch_toggled(bool checked) {
    d->setting->setMayFail(!checked);
    emit changed();
}

void IPv4ConnectionEditorPane::reload(NetworkManager::Setting::Ptr setting) {
    d->setting = setting.staticCast<NetworkManager::Ipv4Setting>();
    d->setting->setInitialized(true);

    switch (d->setting->method()) {
        case NetworkManager::Ipv4Setting::Automatic:
            if (d->setting->ignoreAutoDns()) {
                //DHCP, Manual DNS
                ui->configurationMethod->setCurrentIndex(1);
            } else {
                //DHCP
                ui->configurationMethod->setCurrentIndex(0);
            }
            break;
        case NetworkManager::Ipv4Setting::LinkLocal:
            break;
        case NetworkManager::Ipv4Setting::Manual:
            ui->configurationMethod->setCurrentIndex(2);
            break;
        case NetworkManager::Ipv4Setting::Shared:
            ui->configurationMethod->setCurrentIndex(3);
            break;
        case NetworkManager::Ipv4Setting::Disabled:
            ui->configurationMethod->setCurrentIndex(4);
            break;
    }

    ui->requireIPv4Switch->setChecked(!d->setting->mayFail());

    if (d->setting->addressData().count() > 0) {
        QVariantMap addressData = d->setting->addressData().first();
        ui->ipAddress->setText(addressData.value("address").toString());
        ui->ipSubnet->setValue(addressData.value("prefix").toInt());
    }
    ui->ipGateway->setText(d->setting->gateway());

    updateDns();
}

NetworkManager::Setting::SettingType IPv4ConnectionEditorPane::type() {
    return NetworkManager::Setting::Ipv4;
}

bool IPv4ConnectionEditorPane::prepareSave() {
    switch (d->setting->method()) {
        case NetworkManager::Ipv4Setting::Automatic:
        case NetworkManager::Ipv4Setting::Shared:
            d->setting->setAddresses({});
            d->setting->setAddressData({});
            d->setting->setGateway("");
            if (!d->setting->ignoreAutoDns()) {
                //DHCP
                d->setting->setDns({});
            }
            break;
        case NetworkManager::Ipv4Setting::LinkLocal:
        case NetworkManager::Ipv4Setting::Manual:
        case NetworkManager::Ipv4Setting::Disabled:
            break;
    }

    return true;
}

void IPv4ConnectionEditorPane::on_addDnsServerButton_clicked() {
    QHostAddress address(ui->dnsServer->text());
    if (address.isNull()) {
        tErrorFlash::flashError(ui->dnsServer);
        return;
    }

    QList<QHostAddress> addresses = d->setting->dns();
    addresses.append(address);
    d->setting->setDns(addresses);

    ui->dnsServer->clear();
    updateDns();

    emit changed();
}

void IPv4ConnectionEditorPane::on_dnsList_customContextMenuRequested(const QPoint& pos) {
    QListWidgetItem* item = ui->dnsList->itemAt(pos);
    int row = ui->dnsList->row(item);

    QMenu* menu = new QMenu();
    menu->addSection(tr("For DNS server %1").arg(item->text()));
    menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove"), this, [ = ] {
        QList<QHostAddress> addresses = d->setting->dns();
        addresses.removeAt(row);
        d->setting->setDns(addresses);

        updateDns();
        emit changed();
    });
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(ui->dnsList->mapToGlobal(pos));
}

void IPv4ConnectionEditorPane::on_ipAddress_editingFinished() {
    QHostAddress addr(ui->ipAddress->text());
    if (addr.isNull()) {
        tErrorFlash::flashError(ui->ipAddress);
        ui->ipAddress->clear();
        return;
    }

    setAddresses();
}

void IPv4ConnectionEditorPane::on_ipSubnet_valueChanged(int arg1) {
    setAddresses();
}

void IPv4ConnectionEditorPane::on_ipGateway_editingFinished() {
    QHostAddress addr(ui->ipGateway->text());
    if (addr.isNull()) {
        tErrorFlash::flashError(ui->ipGateway);
        ui->ipAddress->clear();
        return;
    }

    d->setting->setGateway(ui->ipGateway->text());
}
