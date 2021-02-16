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
#include "pairpopover.h"
#include <tpopover.h>
#include "btagent.h"

#include <BluezQt/Manager>
#include <BluezQt/DevicesModel>
#include <BluezQt/Adapter>
#include <BluezQt/PendingCall>
#include <BluezQt/InitManagerJob>
#include <QSortFilterProxyModel>

struct BluetoothSettingsPanePrivate {
    QString currentHostname;

    BluezQt::Manager* manager;
    BluezQt::AdapterPtr usableAdapter;
    BtAgent* agent;
};

BluetoothSettingsPane::BluetoothSettingsPane() :
    StatusCenterPane(),
    ui(new Ui::BluetoothSettingsPane) {
    ui->setupUi(this);

    d = new BluetoothSettingsPanePrivate();
    d->manager = new BluezQt::Manager(this);
    d->agent = new BtAgent(this);

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
    QSortFilterProxyModel* devicesFilter = new QSortFilterProxyModel();
    devicesFilter->setSourceModel(devicesModel);
    devicesFilter->setFilterRole(BluezQt::DevicesModel::PairedRole);
    devicesFilter->setFilterFixedString("true");
    ui->devicesList->setModel(devicesFilter);

    connect(d->manager, &BluezQt::Manager::adapterAdded, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager, &BluezQt::Manager::adapterRemoved, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager, &BluezQt::Manager::usableAdapterChanged, this, &BluetoothSettingsPane::updateUsableAdapter);
    connect(d->manager, &BluezQt::Manager::operationalChanged, this, &BluetoothSettingsPane::updateOperational);
    updateUsableAdapter();
    updateOperational();

    BluezQt::InitManagerJob* initManagerJob = d->manager->init();
    connect(initManagerJob, &BluezQt::InitManagerJob::result, this, [ = ] {
        BluezQt::PendingCall* agentRegister = d->manager->registerAgent(d->agent);
        connect(agentRegister, &BluezQt::PendingCall::finished, [ = ] {
            d->manager->requestDefaultAgent(d->agent);
        });
    });
    initManagerJob->start();
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
    d->usableAdapter.clear();
    for (const BluezQt::AdapterPtr& adapter : d->manager->adapters()) {
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
        ui->visibilitySwitch->setChecked(d->usableAdapter->isDiscoverable());

        updateHostname();
    }
}

void BluetoothSettingsPane::updateOperational() {
    if (!d->manager->isOperational()) {
        ui->stackedWidget->setCurrentWidget(ui->unavailablePage, false);
    } else if (d->manager->adapters().isEmpty()) {
        ui->stackedWidget->setCurrentWidget(ui->unavailablePage, false);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->mainPage, false);
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
    PairPopover* pairPopover = new PairPopover(d->manager, d->agent);
    tPopover* popover = new tPopover(pairPopover);
    popover->setPopoverWidth(SC_DPI(600));
    connect(pairPopover, &PairPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, pairPopover, &PairPopover::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this->window());
}
