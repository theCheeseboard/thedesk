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
#include "bluetoothsettingspane.h"
#include "ui_bluetoothsettingspane.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <QDBusInterface>

#include <BluezQt/Manager>
#include <BluezQt/DevicesModel>
#include <BluezQt/Adapter>

struct BluetoothSettingsPanePrivate {
    QString currentHostname;

    BluezQt::Manager* manager;
    BluezQt::AdapterPtr usableAdapter;
};

BluetoothSettingsPane::BluetoothSettingsPane() :
    StatusCenterPane(),
    ui(new Ui::BluetoothSettingsPane) {
    ui->setupUi(this);

    d = new BluetoothSettingsPanePrivate();
    d->manager = new BluezQt::Manager(this);

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->actionsWidget->setFixedWidth(contentWidth);
    ui->discoverabilityWidget->setFixedWidth(contentWidth);
    ui->devicesLabel->setFixedWidth(contentWidth);

    ui->bluetoothUnavailableIcon->setPixmap(QIcon::fromTheme("bluetooth").pixmap(SC_DPI_T(QSize(128, 128), QSize)));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);

    QDBusConnection::systemBus().connect("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(updateHostname()));
    updateHostname();

    BluezQt::DevicesModel* devicesModel = new BluezQt::DevicesModel(d->manager);
    ui->devicesList->setModel(devicesModel);

    connect(d->manager, &BluezQt::Manager::adapterAdded, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager, &BluezQt::Manager::adapterRemoved, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager, &BluezQt::Manager::usableAdapterChanged, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager, &BluezQt::Manager::operationalChanged, this, &BluetoothSettingsPane::updateOperational);
    updateUsableAdapter();
    updateOperational();
}

BluetoothSettingsPane::~BluetoothSettingsPane() {
    delete d;
    delete ui;
}

void BluetoothSettingsPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

void BluetoothSettingsPane::updateHostname() {
    QDBusInterface hostnamed("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.hostname1", QDBusConnection::systemBus());
    QString hostname = hostnamed.property("PrettyHostname").toString();
    if (hostname.isEmpty()) hostname = hostnamed.property("Hostname").toString();
    d->currentHostname = hostname;

    ui->discoverabilityMessage->setText(tr("To pair with this device, look for %1 on the other device.").arg("<b>" + hostname + "</b>"));
}

void BluetoothSettingsPane::updateUsableAdapter() {
    if (d->usableAdapter) {
        d->usableAdapter->disconnect(this);
    }

    //Find an adapter
    d->usableAdapter.clear();
    for (BluezQt::AdapterPtr adapter : d->manager->adapters()) {
        if (adapter->isPowered()) {
            d->usableAdapter = adapter;
            break;
        }
    }
    if (!d->usableAdapter && !d->manager->adapters().isEmpty()) d->usableAdapter = d->manager->adapters().first();

    if (d->usableAdapter) {
        connect(d->usableAdapter.data(), &BluezQt::Adapter::discoverableChanged, this, [ = ](bool discoverable) {
            ui->visibilitySwitch->setChecked(discoverable);
        });
        ui->visibilitySwitch->setVisible(d->usableAdapter->isDiscoverable());
    }
}

void BluetoothSettingsPane::updateOperational() {
    if (!d->manager->isOperational()) {
        ui->stackedWidget->setCurrentWidget(ui->unavailablePage);
    } else if (d->manager->adapters().isEmpty()) {
        ui->stackedWidget->setCurrentWidget(ui->unavailablePage);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->mainPage);
    }
}

QString BluetoothSettingsPane::name() {
    return "BluetoothSettings";
}

QString BluetoothSettingsPane::displayName() {
    return tr("Bluetooth");
}

QIcon BluetoothSettingsPane::icon() {
    return QIcon::fromTheme("preferences-system-bluetooth");
}

QWidget* BluetoothSettingsPane::leftPane() {
    return nullptr;
}

void BluetoothSettingsPane::on_visibilitySwitch_toggled(bool checked) {
    if (d->usableAdapter) d->usableAdapter->setDiscoverable(checked);
}
