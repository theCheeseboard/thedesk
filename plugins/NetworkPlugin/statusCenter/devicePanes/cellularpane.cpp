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

#include <QAction>
#include <tsettings.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <hudmanager.h>
#include <barmanager.h>
#include <tpopover.h>
#include <tnotification.h>
#include <transparentdialog.h>
#include "../popovers/unlockmodempopover.h"
#include "../popovers/simsettingspopover.h"
#include "common.h"

#include <icontextchunk.h>
#include <actionquickwidget.h>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/Settings>
#include <Manager>
#include <Modem>
#include <Sim>
#include <Modem3Gpp>

struct CellularPanePrivate {
    QListWidgetItem* item;
    NetworkManager::ModemDevice::Ptr device;
    ModemManager::ModemDevice::Ptr modem;
    ModemManager::Modem3gpp::Ptr modem3gpp;

    IconTextChunk* chunk;

    tSettings settings;
    NetworkManager::Device::State oldState;

    QAction* unlockSimAction;

    bool pinNotificationSent = false;
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
    d->modem3gpp = ModemManager::Modem3gpp::Ptr(new ModemManager::Modem3gpp(d->device->udi()));

    d->unlockSimAction = new QAction(this);
    d->unlockSimAction->setVisible(false);
    d->unlockSimAction->setIcon(QIcon::fromTheme("sim-card"));
    connect(d->unlockSimAction, &QAction::triggered, this, &CellularPane::unlockDevice);

    QAction* enableDisableCellularAction = new QAction(this);
    enableDisableCellularAction->setIcon(QIcon::fromTheme("network-cellular"));
    connect(enableDisableCellularAction, &QAction::triggered, this, [ = ] {
        NetworkManager::setWwanEnabled(!NetworkManager::isWwanEnabled());
    });

    ActionQuickWidget* quickWidget = new ActionQuickWidget(d->chunk);
    quickWidget->addAction(d->unlockSimAction);
    quickWidget->addAction(enableDisableCellularAction);
    d->chunk->setQuickWidget(quickWidget);

    auto updateWwanEnabled = [ = ](bool enabled) {
        if (enabled) {
            enableDisableCellularAction->setText(tr("Disable Cellular"));
        } else {
            enableDisableCellularAction->setText(tr("Enable Cellular"));
        }
    };
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::wwanEnabledChanged, this, updateWwanEnabled);
    updateWwanEnabled(NetworkManager::isWwanEnabled());

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
    connect(d->modem->modemInterface().data(), &ModemManager::Modem::currentModesChanged, this, &CellularPane::updateState);
    connect(d->modem->modemInterface().data(), &ModemManager::Modem::unlockRequiredChanged, this, &CellularPane::updateState);
    connect(d->modem3gpp.data(), &ModemManager::Modem3gpp::registrationStateChanged, this, &CellularPane::updateState);

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

    ui->unlockModemButton->setVisible(false);

    QStringList chunkParts;
    chunkParts.append(this->operatorName());

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

            if (d->modem->sim()->uni() == "/") {
                ui->errorFrame->setTitle(tr("No SIM Card"));
                ui->errorFrame->setText(tr("Insert a SIM card to connect to cellular services."));
                chunkParts.clear();
                chunkParts.append(tr("No SIM"));
                d->chunk->setIcon(QIcon::fromTheme("sim-card-none"));
            } else {
                ui->errorFrame->setTitle(tr("Unavailable"));

                QString reasonText;
                reasonText = tr("This network is unavailable because %2.");
                reasonText = reasonText.arg(Common::stateChangeReasonToString(stateReason.reason()));
                ui->errorFrame->setText(reasonText);
                d->chunk->setIcon(signalErrorIcon);
            }

            ui->errorFrame->setState(tStatusFrame::Warning);
            ui->errorFrame->setVisible(true);
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

            ModemManager::Modem::AccessTechnologies accessTechnology = d->modem->modemInterface()->accessTechnologies();

#if MM_CHECK_VERSION(1, 14, 0)
            if (accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_5GNR) {
                chunkParts.append("5G");
            } else
#endif
                if (accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_LTE) {
                    chunkParts.append("LTE");
                } else if (accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS) {
                    chunkParts.append("H+");
                } else if (accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_HSDPA ||
                    accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_HSUPA ||
                    accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_HSPA) {
                    chunkParts.append("H");
                } else if (accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_UMTS ||
                    accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_EVDO0 ||
                    accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_EVDOA ||
                    accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_EVDOB) {
                    chunkParts.append("3G");
                } else if (accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_EDGE) {
                    chunkParts.append("E");
                } else if (accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_GPRS) {
                    chunkParts.append("G");
                } else if (accessTechnology & MM_MODEM_ACCESS_TECHNOLOGY_1XRTT) {
                    chunkParts.append("1X");
                }

            MMModem3gppRegistrationState modem3gppRegistration = d->modem3gpp->registrationState();
            if (modem3gppRegistration == MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING ||
                modem3gppRegistration == MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING_SMS_ONLY ||
                modem3gppRegistration == MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING_CSFB_NOT_PREFERRED) {
                chunkParts.append("R");
            }

            break;
    }

    MMModemLock unlockRequired = d->modem->modemInterface()->unlockRequired();
    ModemManager::UnlockRetriesMap retries = d->modem->modemInterface()->unlockRetries();
    switch (unlockRequired) {
        case MM_MODEM_LOCK_SIM_PIN: {
            ui->unlockModemButton->setText(tr("Enter SIM PIN"));
            ui->unlockModemButton->setVisible(true);
            chunkParts.append(tr("SIM PIN Required"));
            d->chunk->setIcon(QIcon::fromTheme("sim-card"));
            ui->connectButton->setVisible(false);

            ui->errorFrame->setTitle(tr("SIM PIN Required"));

            QString reasonText = tr("A SIM PIN is required to connect to the cellular network.");
            ui->errorFrame->setText(reasonText);
            ui->errorFrame->setState(tStatusFrame::Error);
            ui->errorFrame->setVisible(true);

            d->unlockSimAction->setText(tr("Enter SIM PIN"));
            d->unlockSimAction->setVisible(true);

            if (!d->pinNotificationSent) {
                tNotification* notification = new tNotification();
                notification->setSummary(tr("SIM PIN Required"));
                notification->setText(reasonText);
                notification->insertAction(QStringLiteral("unlock"), tr("Enter SIM PIN"));
                connect(notification, &tNotification::actionClicked, this, [ = ](QString key) {
                    if (key == QStringLiteral("unlock")) unlockDevice();
                });
                notification->post();
                d->pinNotificationSent = true;
            }
            break;
        }
        case MM_MODEM_LOCK_SIM_PUK: {
            ui->unlockModemButton->setText(tr("Enter SIM PUK"));
            ui->unlockModemButton->setVisible(true);
            chunkParts.append(tr("SIM PUK Required"));
            d->chunk->setIcon(QIcon::fromTheme("sim-card"));
            ui->connectButton->setVisible(false);

            ui->errorFrame->setTitle(tr("SIM PUK Required"));

            QString reasonText = tr("A SIM PUK is required to connect to the cellular network.");
            ui->errorFrame->setText(reasonText);
            ui->errorFrame->setState(tStatusFrame::Error);
            ui->errorFrame->setVisible(true);

            d->unlockSimAction->setText(tr("Enter SIM PUK"));
            d->unlockSimAction->setVisible(true);

            if (!d->pinNotificationSent) {
                tNotification* notification = new tNotification();
                notification->setSummary(tr("SIM PUK Required"));
                notification->setText(reasonText);
                notification->insertAction(QStringLiteral("unlock"), tr("Enter SIM PUK"));
                connect(notification, &tNotification::actionClicked, this, [ = ](QString key) {
                    if (key == QStringLiteral("unlock")) unlockDevice();
                });
                notification->post();
                d->pinNotificationSent = true;
            }
            break;
        }
        default:
            ui->unlockModemButton->setVisible(false);
            d->unlockSimAction->setVisible(false);
            d->pinNotificationSent = false;
    }

    d->oldState = stateReason.state();
    d->chunk->setText(chunkParts.join(" · "));
}

QString CellularPane::operatorName() {
    if (!d->modem->sim()->operatorName().isEmpty()) {
        return d->modem->sim()->operatorName();
    }
    return tr("Cellular");
}

void CellularPane::unlockDevice() {
    TransparentDialog* dialog = new TransparentDialog();
    dialog->setWindowFlag(Qt::FramelessWindowHint);
    dialog->setWindowFlag(Qt::WindowStaysOnTopHint);
    dialog->showFullScreen();

    QTimer::singleShot(500, [ = ] {
        UnlockModemPopover* popoverContents = new UnlockModemPopover(d->modem);

        tPopover* popover = new tPopover(popoverContents);
        popover->setPopoverSide(tPopover::Bottom);
        popover->setPopoverWidth(SC_DPI(600));
        popover->setPerformBlur(false);
        connect(popoverContents, &UnlockModemPopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, [ = ] {
            popover->deleteLater();
            dialog->deleteLater();
            popoverContents->deleteLater();
        });
        popover->show(dialog);
    });
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

void CellularPane::on_unlockModemButton_clicked() {
    unlockDevice();
}
