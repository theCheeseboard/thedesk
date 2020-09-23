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
#include "cellularpane.h"
#include "ui_cellularpane.h"

#include <tsettings.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <hudmanager.h>
#include <barmanager.h>
#include <tpopover.h>
#include "../popovers/simsettingspopover.h"
#include "common.h"

#include <icontextchunk.h>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/Settings>
#include <Manager>
#include <Modem>
#include <Sim>

struct CellularPanePrivate {
    QListWidgetItem* item;
    NetworkManager::ModemDevice::Ptr device;
    ModemManager::ModemDevice::Ptr modem;

    IconTextChunk* chunk;

    tSettings settings;
    NetworkManager::Device::State oldState;
};

CellularPane::CellularPane(QString uni, QWidget* parent) :
    AbstractDevicePane(parent),
    ui(new Ui::CellularPane) {
    ui->setupUi(this);
    d = new CellularPanePrivate();

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->actionsWidget->setFixedWidth(contentWidth);
    ui->statusWidget->setFixedWidth(contentWidth);

    ui->disconnectButton->setProperty("type", "destructive");
    ui->errorFrame->setVisible(false);

    d->chunk = new IconTextChunk("network-cellular");
    d->item = new QListWidgetItem();
    d->device = NetworkManager::findNetworkInterface(uni).staticCast<NetworkManager::ModemDevice>();
    d->modem = ModemManager::findModemDevice(d->device->udi());

    d->item->setText(this->operatorName());
    ui->titleLabel->setText(this->operatorName());

    connect(d->device.data(), &NetworkManager::ModemDevice::stateChanged, this, &CellularPane::updateState);
    updateState();

    connect(d->device.data(), &NetworkManager::ModemDevice::stateChanged, this, [ = ](NetworkManager::Device::State newState, NetworkManager::Device::State oldState, NetworkManager::Device::StateChangeReason reason) {
        if (d->settings.value("NetworkPlugin/notifications.activation").toBool()) {
            switch (newState) {
                case NetworkManager::Device::Unavailable:
                    d->device->setAutoconnect(false);
                    Q_FALLTHROUGH();
                case NetworkManager::Device::Disconnected:
                    if (oldState != NetworkManager::Device::Failed) StateManager::hudManager()->showHud({
                        {"icon", "network-cellular-disconnected"},
                        {"title", tr("Cellular")},
                        {"text", tr("Disconnected")}
                    });
                    break;
                case NetworkManager::Device::Activated: {
                    d->device->setAutoconnect(true);
                    QString title = tr("Cellular");

                    StateManager::hudManager()->showHud({
                        {"icon", "network-cellular-activated"},
                        {"title", title},
                        {"text", tr("Connected")}
                    });
                    break;
                }
                case NetworkManager::Device::Failed:
                    d->device->setAutoconnect(false);
                    StateManager::hudManager()->showHud({
                        {"icon", "network-cellular-error"},
                        {"title", tr("Cellular")},
                        {"text", tr("Failed")}
                    });
                    break;
                default:
                    break;
            }
        }
    });

    connect(d->modem->modemInterface().data(), &ModemManager::Modem::signalQualityChanged, this, &CellularPane::updateState);

    StateManager::barManager()->addChunk(d->chunk);
}

CellularPane::~CellularPane() {
    StateManager::barManager()->removeChunk(d->chunk);
    d->chunk->deleteLater();
    delete d;
    delete ui;
}

void CellularPane::updateState() {
    QIcon signalIcon = QIcon::fromTheme(Common::iconForSignalStrength(d->modem->modemInterface()->signalQuality().signal, Common::Cellular));
    QIcon signalErrorIcon = QIcon::fromTheme(Common::iconForSignalStrength(d->modem->modemInterface()->signalQuality().signal, Common::CellularError));
    ui->deviceIcon->setPixmap(QIcon::fromTheme("computer").pixmap(SC_DPI_T(QSize(96, 96), QSize)));
    ui->routerIcon->setPixmap(signalIcon.pixmap(SC_DPI_T(QSize(96, 96), QSize)));
    ui->routerName->setText(this->operatorName());
    d->chunk->setText(this->operatorName());

    NetworkManager::DeviceStateReason stateReason = d->device->stateReason();
    if (d->oldState != NetworkManager::Device::Failed) {
        //Only get rid of the error message here if the previous state was not failure.
        ui->errorFrame->setVisible(false);
    }

    switch (stateReason.state()) {
        case NetworkManager::Device::UnknownState:
        case NetworkManager::Device::Unmanaged:
        case NetworkManager::Device::Unavailable: {
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->leftStateLine->setEnabled(false);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(false);
            ui->connectButton->setVisible(false);

            d->device->setAutoconnect(false);
            ui->errorFrame->setTitle(tr("Unavailable"));

            QString reasonText;
            reasonText = tr("This network is unavailable because %2.");
            reasonText = reasonText.arg(Common::stateChangeReasonToString(stateReason.reason()));
            ui->errorFrame->setText(reasonText);
            ui->errorFrame->setState(tStatusFrame::Warning);
            ui->errorFrame->setVisible(true);
            d->chunk->setIcon(signalErrorIcon);
            break;
        }
        case NetworkManager::Device::Disconnected:
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->leftStateLine->setEnabled(false);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(false);
            ui->connectButton->setVisible(true);
            d->chunk->setIcon(signalErrorIcon);
            break;
        case NetworkManager::Device::Failed: {
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-cancel").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->leftStateLine->setEnabled(false);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(false);
            ui->connectButton->setVisible(true);
            d->chunk->setIcon(signalErrorIcon);

            ui->errorFrame->setTitle(tr("Connection Failure"));

            QString reasonText = tr("Connecting to the network failed because %2.");
            reasonText = reasonText.arg(Common::stateChangeReasonToString(stateReason.reason()));
            ui->errorFrame->setText(reasonText);
            ui->errorFrame->setState(tStatusFrame::Error);
            ui->errorFrame->setVisible(true);
            break;
        }
        case NetworkManager::Device::Preparing:
        case NetworkManager::Device::ConfiguringHardware:
        case NetworkManager::Device::NeedAuth:
        case NetworkManager::Device::ConfiguringIp:
        case NetworkManager::Device::CheckingIp:
        case NetworkManager::Device::WaitingForSecondaries:
        case NetworkManager::Device::Deactivating:
            ui->stateConnecting->setVisible(true);
            ui->stateIcon->setVisible(false);
            ui->leftStateLine->setEnabled(true);
            ui->rightStateLine->setEnabled(false);
            ui->disconnectButton->setVisible(true);
            ui->connectButton->setVisible(false);
            d->chunk->setIcon(signalIcon);
            break;
        case NetworkManager::Device::Activated:
            ui->stateConnecting->setVisible(false);
            ui->stateIcon->setVisible(true);
            ui->stateIcon->setPixmap(QIcon::fromTheme("dialog-ok").pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            ui->leftStateLine->setEnabled(true);
            ui->rightStateLine->setEnabled(true);
            ui->disconnectButton->setVisible(true);
            ui->connectButton->setVisible(false);
            d->chunk->setIcon(signalIcon);
            break;
    }

    d->oldState = stateReason.state();
}

QString CellularPane::operatorName() {
    if (!d->modem->sim()->operatorName().isEmpty()) {
        return d->modem->sim()->operatorName();
    }
    return tr("Cellular");
}


QListWidgetItem* CellularPane::leftPaneItem() {
    return d->item;
}

void CellularPane::on_disconnectButton_clicked() {
    d->device->disconnectInterface();
}

void CellularPane::on_simSettingsButton_clicked() {
    SimSettingsPopover* simSettings = new SimSettingsPopover(d->modem);
    tPopover* popover = new tPopover(simSettings);
    popover->setPopoverWidth(SC_DPI(600));
    connect(simSettings, &SimSettingsPopover::dismissed, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, simSettings, &SimSettingsPopover::deleteLater);
    popover->show(this->window());
}
