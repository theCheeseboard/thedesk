/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "devicepopover.h"
#include "ui_devicepopover.h"

#include "bluetoothplugincommon.h"
#include <BluezQt/Adapter>
#include <BluezQt/PendingCall>
#include <ttoast.h>

struct DevicePopoverPrivate {
        BluezQt::DevicePtr device;
};

DevicePopover::DevicePopover(BluezQt::DevicePtr device, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DevicePopover) {
    ui->setupUi(this);
    d = new DevicePopoverPrivate();
    d->device = device;

    ui->titleLabel->setBackButtonShown(true);

    ui->titleLabel->setText(device->name());
    ui->iconLabel->setPixmap(QIcon::fromTheme(device->icon()).pixmap(SC_DPI_T(QSize(64, 64), QSize)));
    ui->nameLabel->setText(device->name());
    ui->typeLabel->setText(BluetoothPluginCommon::stringForDeviceType(device->type()));

    connect(d->device.data(), &BluezQt::Device::connectedChanged, this, &DevicePopover::updateDevice);
    updateDevice();

    ui->removeButton->setProperty("type", "destructive");
}

DevicePopover::~DevicePopover() {
    delete d;
    delete ui;
}

void DevicePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void DevicePopover::on_removeButton_clicked() {
    // TODO: Ask
    d->device->adapter()->removeDevice(d->device);
    emit done();
}

void DevicePopover::on_connectButton_clicked() {
    BluezQt::PendingCall* call = d->device->connectToDevice();
    connect(call, &BluezQt::PendingCall::finished, this, [=] {
        if (call->error() != BluezQt::PendingCall::NoError) {
            tToast* toast = new tToast();
            toast->setTitle(tr("Failed to connect"));
            switch (call->error()) {
                default:
                    toast->setText(tr("Connecting to %1 failed.").arg(QLocale().quoteString(d->device->name())));
            }
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        }
    });
}

void DevicePopover::on_disconnectButton_clicked() {
    d->device->disconnectFromDevice();
}

void DevicePopover::updateDevice() {
    if (d->device->isConnected()) {
        ui->connectButton->setVisible(false);
        ui->disconnectButton->setVisible(true);
    } else {
        ui->connectButton->setVisible(true);
        ui->disconnectButton->setVisible(false);
    }
}
