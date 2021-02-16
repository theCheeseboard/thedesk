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
#include "pairpopover.h"
#include "ui_pairpopover.h"

#include <BluezQt/Adapter>
#include <BluezQt/DevicesModel>
#include <BluezQt/Device>
#include <BluezQt/PendingCall>
#include <QSortFilterProxyModel>
#include <ttoast.h>
#include "btagent.h"

struct PairPopoverPrivate {
    BluezQt::Manager* manager;
    BluezQt::AdapterPtr adapter;
    BtAgent* agent;

    tPromiseFunctions<QString>::SuccessFunction pairSuccessFunction;
    tPromiseFunctions<QString>::FailureFunction pairFailFunction;
};

PairPopover::PairPopover(BluezQt::Manager* manager, BtAgent* agent, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PairPopover) {
    ui->setupUi(this);
    d = new PairPopoverPrivate();
    d->manager = manager;
    d->adapter = d->manager->usableAdapter();
    d->agent = agent;

    ui->titleLabel->setBackButtonShown(true);
    ui->confirmCodeTitleLabel->setBackButtonShown(true);
    ui->confirmCodeReturnIcon->setPixmap(QIcon::fromTheme("key-enter").pixmap(SC_DPI_T(QSize(16, 16), QSize)));

    BluezQt::DevicesModel* devicesModel = new BluezQt::DevicesModel(d->manager);
    QSortFilterProxyModel* devicesFilter = new QSortFilterProxyModel();
    devicesFilter->setSourceModel(devicesModel);
    devicesFilter->setFilterRole(BluezQt::DevicesModel::PairedRole);
    devicesFilter->setFilterFixedString("false");
    QSortFilterProxyModel* devicesFilter2 = new QSortFilterProxyModel();
    devicesFilter2->setSourceModel(devicesFilter);
    devicesFilter2->setFilterRole(BluezQt::DevicesModel::AdapterAddressRole);
    devicesFilter2->setFilterFixedString(d->adapter->address());
    ui->pairList->setModel(devicesFilter2);

    d->adapter->startDiscovery();
}

PairPopover::~PairPopover() {
    d->adapter->stopDiscovery();
    delete d;
    delete ui;
}

tPromise<QString>* PairPopover::triggerPairConfirmation(PairPopover::PairConfirmationType pairType, QString pinCode) {
    return TPROMISE_CREATE_SAME_THREAD(QString, {
        d->pairSuccessFunction = res;
        d->pairFailFunction = rej;

        switch (pairType) {
            case PairPopover::ConfirmPinCode:
                ui->stackedWidget->setCurrentWidget(ui->confirmCodePage);
                ui->confirmCodeActionLabel->setText("Confirm this pairing code");
                ui->confirmCodeSubtitle->setVisible(false);
                ui->confirmCodeText->setText(pinCode);
                ui->confirmCodeReturnIcon->setVisible(false);
                ui->confirmCodeAcceptButton->setVisible(true);
                break;
            case PairPopover::KeyPinCode:
                ui->stackedWidget->setCurrentWidget(ui->confirmCodePage);
                ui->confirmCodeActionLabel->setText("Key this in on the device");
                ui->confirmCodeSubtitle->setText(tr("Then press ENTER or RETURN."));
                ui->confirmCodeSubtitle->setVisible(true);
                ui->confirmCodeText->setText(pinCode);
                ui->confirmCodeReturnIcon->setVisible(true);
                ui->confirmCodeAcceptButton->setVisible(false);
                break;
        }
    });
}

void PairPopover::cancelPendingPairing() {
//    if (d->pairFailFunction) d->pairFailFunction("Cancelled");
}

void PairPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void PairPopover::on_pairList_activated(const QModelIndex& index) {
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);

    BluezQt::DevicePtr device = d->adapter->deviceForAddress(index.data(BluezQt::DevicesModel::AddressRole).toString());
    d->agent->capturePairRequests(device, this);
    ui->deviceNameConfirmCode->setText(device->name());

    BluezQt::PendingCall* pairRequest = device->pair();
    connect(pairRequest, &BluezQt::PendingCall::finished, this, [ = ] {
        d->agent->capturePairRequests(nullptr, nullptr);

        if (pairRequest->error() == BluezQt::PendingCall::NoError) {
            emit done();
        } else {
            ui->stackedWidget->setCurrentWidget(ui->deviceSelectionPage);


            tToast* toast = new tToast();
            toast->setTitle(tr("Failed to pair"));
            switch (pairRequest->error()) {
                case BluezQt::PendingCall::AuthenticationFailed:
                    toast->setText(tr("Pairing with %1 failed because the pairing code was incorrect.").arg(QLocale().quoteString(device->name())));
                    break;
                case BluezQt::PendingCall::AuthenticationRejected:
                    toast->setText(tr("Pairing with %1 failed because the device rejected the pairing request.").arg(QLocale().quoteString(device->name())));
                    break;
                case BluezQt::PendingCall::AuthenticationTimeout:
                    toast->setText(tr("Pairing with %1 failed because the pairing code was not confirmed in time.").arg(QLocale().quoteString(device->name())));
                    break;
                default:
                    toast->setText(tr("Pairing with %1 failed.").arg(QLocale().quoteString(device->name())));
            }
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        }
    });
}

void PairPopover::on_confirmCodeAcceptButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);
    d->pairSuccessFunction("");
}

void PairPopover::on_confirmCodeTitleLabel_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);
    d->pairFailFunction("Failure");
}
