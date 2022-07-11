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
#include "simsettingspopover.h"
#include "ui_simsettingspopover.h"

#include "networkplugincommon.h"
#include <ModemManagerQt/Modem3Gpp>
#include <ttoast.h>

struct SimSettingsPopoverPrivate {
        ModemManager::ModemDevice::Ptr modem;

        enum Operation {
            EnablePin,
            DisablePin,
            ChangePin
        };
        Operation operation;
};

SimSettingsPopover::SimSettingsPopover(ModemManager::ModemDevice::Ptr modem, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SimSettingsPopover) {
    ui->setupUi(this);
    d = new SimSettingsPopoverPrivate();
    d->modem = modem;

    ui->titleLabel->setBackButtonShown(true);
    ui->currentPinTitleLabel->setBackButtonShown(true);
    ui->changeSimPinTitleLabel->setBackButtonShown(true);
    ui->callWaitingTitleLabel->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->spinner->setFixedSize(SC_DPI_T(QSize(32, 32), QSize));

    ui->modemImeiLabel->setText(modem->modemInterface()->equipmentIdentifier());
    ui->callWaitingSpinner->setVisible(false);
    ui->callWaitingSpinner->setFixedSize(SC_DPI_T(QSize(16, 16), QSize));

    if (modem->sim()->uni() == "/") {
        ui->modemImsiLabel->setText(tr("No SIM card"));
        ui->modemCarrierLabel->setText(tr("No SIM card"));
        ui->simSettingsWidget->setVisible(false);
    } else {
        ui->modemImsiLabel->setText(modem->sim()->imsi());
        ui->modemCarrierLabel->setText(modem->sim()->operatorName());
        ui->simSettingsWidget->setVisible(true);
    }

    prepareMainPage();
}

SimSettingsPopover::~SimSettingsPopover() {
    delete d;
    delete ui;
}

void SimSettingsPopover::on_currentSimPinAcceptButton_clicked() {
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->stackedWidget->setCurrentWidget(ui->spinnerPage);

    QDBusPendingReply<> pendingReply;
    switch (d->operation) {
        case SimSettingsPopoverPrivate::EnablePin:
            pendingReply = d->modem->sim()->enablePin(ui->currentPinBox->text(), true);
            break;
        case SimSettingsPopoverPrivate::DisablePin:
            pendingReply = d->modem->sim()->enablePin(ui->currentPinBox->text(), false);
            break;
        case SimSettingsPopoverPrivate::ChangePin:
            pendingReply = d->modem->sim()->changePin(ui->currentPinBox->text(), ui->changeSimPinBox->text());
            break;
    }

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (watcher->isError()) {
            prepareCurrentPinPage();

            QTimer::singleShot(0, [=] {
                ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
            });
        } else {
            ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
            ui->stackedWidget->setCurrentWidget(ui->startPage);
            prepareMainPage();
        }
    });
}

void SimSettingsPopover::on_titleLabel_backButtonClicked() {
    emit dismissed();
}

void SimSettingsPopover::on_enableSimPinButton_clicked() {
    d->operation = SimSettingsPopoverPrivate::EnablePin;
    ui->currentPinTitleLabel->setText(tr("Enable SIM PIN"));
    prepareCurrentPinPage();
}

void SimSettingsPopover::on_disableSimPinButton_clicked() {
    d->operation = SimSettingsPopoverPrivate::DisablePin;
    ui->currentPinTitleLabel->setText(tr("Disable SIM PIN"));
    prepareCurrentPinPage();
}

void SimSettingsPopover::on_currentPinTitleLabel_backButtonClicked() {
    if (d->operation == SimSettingsPopoverPrivate::ChangePin) {
        ui->stackedWidget->setCurrentWidget(ui->changeSimPinPage);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->startPage);
    }
}

void SimSettingsPopover::prepareCurrentPinPage() {
    ModemManager::UnlockRetriesMap retries;

    QDBusMessage unlockRetriesMessage = QDBusMessage::createMethodCall("org.freedesktop.ModemManager1", d->modem->uni(), "org.freedesktop.DBus.Properties", "Get");
    unlockRetriesMessage.setArguments({"org.freedesktop.ModemManager1.Modem", "UnlockRetries"});
    QDBusMessage unlockRetriesReply = QDBusConnection::systemBus().call(unlockRetriesMessage);
    QDBusArgument unlockRetriesArg = unlockRetriesReply.arguments().first().value<QDBusVariant>().variant().value<QDBusArgument>();
    unlockRetriesArg >> retries;

    ui->currentPinPageOperatorName->setText(NetworkPluginCommon::operatorNameForModem(d->modem));
    ui->pinRetryCount->setText(tr("You have %n remaining tries", nullptr, retries.value(MM_MODEM_LOCK_SIM_PIN)));
    ui->stackedWidget->setCurrentWidget(ui->currentPinPage);
}

void SimSettingsPopover::prepareMainPage() {
    ModemManager::Modem3gpp::Ptr modem3gpp(new ModemManager::Modem3gpp(d->modem->uni()));
    bool simLockEnabled = modem3gpp->enabledFacilityLocks() & MM_MODEM_3GPP_FACILITY_SIM;

    ui->enableSimPinButton->setVisible(!simLockEnabled);
    ui->disableSimPinButton->setVisible(simLockEnabled);
}

void SimSettingsPopover::on_changeSimPinTitleLabel_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->startPage);
}

void SimSettingsPopover::on_changeSimPinAcceptButton_clicked() {
    prepareCurrentPinPage();
}

void SimSettingsPopover::on_changeSimPinButton_clicked() {
    d->operation = SimSettingsPopoverPrivate::ChangePin;
    ui->currentPinTitleLabel->setText(tr("Change SIM PIN"));
    ui->changeSimPinOperatorName->setText(NetworkPluginCommon::operatorNameForModem(d->modem));
    ui->stackedWidget->setCurrentWidget(ui->changeSimPinPage);
}

void SimSettingsPopover::on_enableCallWaitingSwitch_toggled(bool checked) {
    this->setEnabled(false);
    ui->callWaitingSpinner->setVisible(true);

    QString uni = d->modem->uni();
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.ModemManager1", uni, "org.freedesktop.ModemManager1.Modem.Voice", "CallWaitingSetup");
    message.setArguments({checked});

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        this->setEnabled(true);
        ui->callWaitingSpinner->setVisible(false);
        watcher->deleteLater();
    });
}

void SimSettingsPopover::on_callWaitingTitleLabel_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->startPage);
}

void SimSettingsPopover::on_callWaitingButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->spinnerPage);

    QString uni = d->modem->uni();
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.ModemManager1", uni, "org.freedesktop.ModemManager1.Modem.Voice", "CallWaitingQuery");

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        QSignalBlocker blocker(ui->enableCallWaitingSwitch);

        if (watcher->isError()) {
            ui->stackedWidget->setCurrentWidget(ui->startPage);

            tToast* toast = new tToast(this);
            toast->setTitle(tr("Error"));
            toast->setText(tr("Couldn't get current Call Waiting status from carrier"));
            toast->show(this);
        } else {
            ui->stackedWidget->setCurrentWidget(ui->callWaitingPage);
            ui->enableCallWaitingSwitch->setChecked(watcher->reply().arguments().first().toBool());
        }
        watcher->deleteLater();
    });
}
