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
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->spinner->setFixedSize(SC_DPI_T(QSize(32, 32), QSize));
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
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        if (watcher->isError()) {
            prepareCurrentPinPage();

            QTimer::singleShot(0, [ = ] {
                ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
            });
        } else {
            ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
            ui->stackedWidget->setCurrentWidget(ui->startPage);
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
    ui->currentPinPageOperatorName->setText(d->modem->sim()->operatorName());
    ui->pinRetryCount->setText(tr("You have %n remaining tries", nullptr, d->modem->modemInterface()->unlockRetries().value(MM_MODEM_LOCK_SIM_PIN)));
    ui->stackedWidget->setCurrentWidget(ui->currentPinPage);
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
    ui->changeSimPinOperatorName->setText(d->modem->sim()->operatorName());
    ui->stackedWidget->setCurrentWidget(ui->changeSimPinPage);
}
