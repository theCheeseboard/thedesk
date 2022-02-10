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
#include <quickswitch.h>
#include <QDBusInterface>
#include "pairpopover.h"
#include "devicepopover.h"
#include <tpopover.h>
#include "btagent.h"
#include "devicedelegate.h"

#include <BluezQt/Manager>
#include <BluezQt/DevicesModel>
#include <BluezQt/Adapter>
#include <BluezQt/PendingCall>
#include <BluezQt/InitManagerJob>
#include <QSortFilterProxyModel>

struct BluetoothSettingsPanePrivate {
    QString currentHostname;

    BluezQt::ManagerPtr manager;
    BluezQt::AdapterPtr usableAdapter;
    BtAgent* agent;

    QuickSwitch* bluetoothSwitch;
};

BluetoothSettingsPane::BluetoothSettingsPane(BluezQt::ManagerPtr manager, BtAgent* agent) :
    StatusCenterPane(),
    ui(new Ui::BluetoothSettingsPane) {
    ui->setupUi(this);

    d = new BluetoothSettingsPanePrivate();
    d->manager = manager;
    d->agent = agent;

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->actionsWidget->setFixedWidth(contentWidth);
    ui->discoverabilityWidget->setFixedWidth(contentWidth);
    ui->devicesLabel->setFixedWidth(contentWidth);

    ui->bluetoothUnavailableIcon->setPixmap(QIcon::fromTheme("bluetooth").pixmap(SC_DPI_T(QSize(128, 128), QSize)));
    ui->bluetoothUnavailableIcon_2->setPixmap(QIcon::fromTheme("bluetooth").pixmap(SC_DPI_T(QSize(128, 128), QSize)));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);

    QDBusConnection::systemBus().connect("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(updateHostname()));
    updateHostname();

    d->bluetoothSwitch = new QuickSwitch("Bluetooth");
    d->bluetoothSwitch->setTitle(tr("Bluetooth"));
    connect(d->bluetoothSwitch, &QuickSwitch::toggled, this, [ = ](bool isEnabled) {
        if (isEnabled) {
            ui->enableBluetoothButton->click();
        } else {
            d->manager->setBluetoothBlocked(true);
        }
    });

    BluezQt::DevicesModel* devicesModel = new BluezQt::DevicesModel(d->manager.data());
    QSortFilterProxyModel* devicesFilter = new QSortFilterProxyModel();
    devicesFilter->setSourceModel(devicesModel);
    devicesFilter->setFilterRole(BluezQt::DevicesModel::PairedRole);
    devicesFilter->setFilterFixedString("true");
    ui->devicesList->setModel(devicesFilter);
    ui->devicesList->setItemDelegate(new DeviceDelegate(false));

    connect(d->manager.data(), &BluezQt::Manager::adapterAdded, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager.data(), &BluezQt::Manager::adapterRemoved, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager.data(), &BluezQt::Manager::usableAdapterChanged, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager.data(), &BluezQt::Manager::operationalChanged, this, &BluetoothSettingsPane::updateOperational);
    connect(d->manager.data(), &BluezQt::Manager::bluetoothBlockedChanged, this, &BluetoothSettingsPane::updateOperational);
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

    if (d->usableAdapter) d->usableAdapter->setName(hostname);

    ui->discoverabilityMessage->setText(tr("To pair with this device, look for %1 on the other device.").arg("<b>" + hostname + "</b>"));
}

void BluetoothSettingsPane::updateUsableAdapter() {
    if (d->usableAdapter) {
        d->usableAdapter->disconnect(this);
    }

    //Find an adapter
//    d->usableAdapter.clear();
//    for (const BluezQt::AdapterPtr& adapter : d->manager->adapters()) {
//        if (adapter->isPowered()) {
//            d->usableAdapter = adapter;
//            break;
//        }
//    }
//    if (!d->usableAdapter && !d->manager->adapters().isEmpty()) d->usableAdapter = d->manager->adapters().first();
    if (!d->manager->adapters().isEmpty()) {
        d->usableAdapter = d->manager->adapters().at(0);
    } else {
        d->usableAdapter = nullptr;
    }

    if (d->usableAdapter) {
        connect(d->usableAdapter.data(), &BluezQt::Adapter::poweredChanged, this, [ = ](bool powered) {
            updateOperational();
        });
        connect(d->usableAdapter.data(), &BluezQt::Adapter::discoverableChanged, this, [ = ](bool discoverable) {
            ui->visibilitySwitch->setChecked(discoverable);
        });
        ui->visibilitySwitch->setChecked(d->usableAdapter->isDiscoverable());

        updateHostname();
    }

    updateOperational();
}

void BluetoothSettingsPane::updateOperational() {
    QSignalBlocker blocker(d->bluetoothSwitch);
    if (!d->manager->isOperational()) {
        ui->stackedWidget->setCurrentWidget(ui->unavailablePage, false);
        if (StateManager::statusCenterManager()->isSwitchRegistered(d->bluetoothSwitch)) StateManager::statusCenterManager()->removeSwitch(d->bluetoothSwitch);
    } else if (d->manager->adapters().isEmpty()) {
        ui->stackedWidget->setCurrentWidget(ui->unavailablePage, false);
        if (StateManager::statusCenterManager()->isSwitchRegistered(d->bluetoothSwitch)) StateManager::statusCenterManager()->removeSwitch(d->bluetoothSwitch);
    } else if (d->manager->isBluetoothBlocked() || d->usableAdapter == nullptr || !d->usableAdapter->isPowered()) {
        ui->stackedWidget->setCurrentWidget(ui->disabledPage, false);
        d->bluetoothSwitch->setChecked(false);
        if (!StateManager::statusCenterManager()->isSwitchRegistered(d->bluetoothSwitch)) StateManager::statusCenterManager()->addSwitch(d->bluetoothSwitch);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->mainPage, false);
        d->bluetoothSwitch->setChecked(true);
        if (!StateManager::statusCenterManager()->isSwitchRegistered(d->bluetoothSwitch)) StateManager::statusCenterManager()->addSwitch(d->bluetoothSwitch);
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

void BluetoothSettingsPane::on_pairButton_clicked() {
    PairPopover* pairPopover = new PairPopover(d->manager.data(), d->agent);
    tPopover* popover = new tPopover(pairPopover);
    popover->setPopoverWidth(SC_DPI(600));
    connect(pairPopover, &PairPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, pairPopover, &PairPopover::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this->window());
}

void BluetoothSettingsPane::on_enableBluetoothButton_clicked() {
    d->manager->setBluetoothBlocked(false);
    if (!d->manager->adapters().isEmpty()) {
        d->manager->adapters().at(0)->setPowered(true);
    }
}

void BluetoothSettingsPane::on_devicesList_activated(const QModelIndex& index) {
    BluezQt::AdapterPtr adapter = d->manager->adapterForAddress(index.data(BluezQt::DevicesModel::AdapterAddressRole).toString());
    BluezQt::DevicePtr device = adapter->deviceForAddress(index.data(BluezQt::DevicesModel::AddressRole).toString());

    DevicePopover* devicePopover = new DevicePopover(device);
    tPopover* popover = new tPopover(devicePopover);
    popover->setPopoverWidth(SC_DPI(600));
    connect(devicePopover, &DevicePopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, devicePopover, &DevicePopover::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this->window());
}


void BluetoothSettingsPane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}
