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
#include "unlockmodempopover.h"
#include "ui_unlockmodempopover.h"

#include <QDBusPendingCallWatcher>
#include <terrorflash.h>

struct UnlockModemPopoverPrivate {
    ModemManager::ModemDevice::Ptr modem;
};

UnlockModemPopover::UnlockModemPopover(ModemManager::ModemDevice::Ptr modem, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::UnlockModemPopover) {
    ui->setupUi(this);
    d = new UnlockModemPopoverPrivate();

    d->modem = modem;
    updatePage();

    ui->simPinTitleLabel->setBackButtonShown(true);
    ui->simPukTitleLabel->setBackButtonShown(true);
    ui->spinner->setFixedSize(SC_DPI_T(QSize(32, 32), QSize));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->simPukContainer->setFixedWidth(SC_DPI(600));

    ui->simPukWarning->setState(tStatusFrame::Warning);
    ui->simPukWarning->setTitle(tr("Don't guess the PUK!"));
    ui->simPukWarning->setText(tr("Once you exhaust your attempts, the SIM card will be blocked forever.\n\nYou'll need to contact your carrier for a new SIM card."));
}

UnlockModemPopover::~UnlockModemPopover() {
    delete d;
    delete ui;
}

void UnlockModemPopover::updatePage() {
    MMModemLock unlockRequired = d->modem->modemInterface()->unlockRequired();
    ModemManager::UnlockRetriesMap retries;

    QDBusMessage unlockRetriesMessage = QDBusMessage::createMethodCall("org.freedesktop.ModemManager1", d->modem->uni(), "org.freedesktop.DBus.Properties", "Get");
    unlockRetriesMessage.setArguments({"org.freedesktop.ModemManager1.Modem", "UnlockRetries"});
    QDBusMessage unlockRetriesReply = QDBusConnection::systemBus().call(unlockRetriesMessage);
    QDBusArgument unlockRetriesArg = unlockRetriesReply.arguments().first().value<QDBusVariant>().variant().value<QDBusArgument>();
    unlockRetriesArg >> retries;

    if (retries.value(MM_MODEM_LOCK_SIM_PIN) == 1) {
        ui->pinRetryCount->setText(tr("If you enter the incorrect PIN again, your SIM card will be PUK locked, and you'll need to contact your carrier."));
    } else {
        ui->pinRetryCount->setText(tr("You have %n remaining tries", nullptr, retries.value(MM_MODEM_LOCK_SIM_PIN)));
    }
    if (retries.value(MM_MODEM_LOCK_SIM_PUK) == 1) {
        ui->pukRetryCount->setText(tr("This is your final chance to get the PUK right before you'll need to obtain a new SIM card from your carrier."));
    } else {
        ui->pukRetryCount->setText(tr("You have %n remaining tries", nullptr, retries.value(MM_MODEM_LOCK_SIM_PUK)));
    }
    ui->simPinOperatorName->setText(d->modem->sim()->operatorName());
    ui->pukDescription->setText(tr("Contact your carrier to obtain the <b>SIM PUK</b>, and enter it below to unlock %1.").arg(QLocale().quoteString(d->modem->sim()->operatorName())));
    ui->simPinBox->clear();
    ui->simPukBox->clear();

    switch (unlockRequired) {
        case MM_MODEM_LOCK_SIM_PIN: {
            ui->stackedWidget->setCurrentWidget(ui->simPinPage);
            ui->simPinBox->setFocus();
            break;
        }
        case MM_MODEM_LOCK_SIM_PUK: {
            ui->stackedWidget->setCurrentWidget(ui->simPukPage);
            ui->simPukBox->setFocus();
            break;
        }
        default:
            emit done();
    }
}

void UnlockModemPopover::on_simPinAcceptButton_clicked() {
    bool ok;
    ui->simPinBox->text().toInt(&ok);
    if (!ok) {
        tErrorFlash::flashError(ui->simPinBox);
        return;
    }

    if (ui->simPinBox->text().count() < 4) {
        tErrorFlash::flashError(ui->simPinBox);
        return;
    }

    ui->stackedWidget->setCurrentWidget(ui->loadingPage);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->modem->sim()->sendPin(ui->simPinBox->text()));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        QTimer::singleShot(200, this, &UnlockModemPopover::updatePage);
        watcher->deleteLater();
    });
}

void UnlockModemPopover::on_simPinTitleLabel_backButtonClicked() {
    emit done();
}

void UnlockModemPopover::on_simPukAcceptButton_clicked() {
    bool ok;
    ui->simPukBox->text().toInt(&ok);
    if (!ok) {
        tErrorFlash::flashError(ui->simPukBox);
        return;
    }

    if (ui->simPukBox->text().count() < 8) {
        tErrorFlash::flashError(ui->simPukBox);
        return;
    }

    ui->simPukNewSimPin->text().toInt(&ok);
    if (!ok) {
        tErrorFlash::flashError(ui->simPukNewSimPin);
        return;
    }

    if (ui->simPukNewSimPin->text().count() < 4) {
        tErrorFlash::flashError(ui->simPukNewSimPin);
        return;
    }

    if (ui->simPukConfirmSimPin->text() != ui->simPukNewSimPin->text()) {
        tErrorFlash::flashError(ui->simPukConfirmSimPin);
        return;
    }

    ui->stackedWidget->setCurrentWidget(ui->loadingPage);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->modem->sim()->sendPuk(ui->simPukBox->text(), ui->simPukNewSimPin->text()));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        QTimer::singleShot(200, this, &UnlockModemPopover::updatePage);
        watcher->deleteLater();
    });
}

void UnlockModemPopover::on_simPukTitleLabel_backButtonClicked() {
    emit done();
}
