/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#include "onboardingnetwork.h"
#include "ui_onboardingnetwork.h"

#include <statemanager.h>
#include <onboardingmanager.h>

#include "wiredonboardingsetup.h"
#include "wirelessonboardingsetup.h"

#include <NetworkManagerQt/Manager>

OnboardingNetwork::OnboardingNetwork(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingNetwork) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->disconnectButton->setProperty("type", "destructive");
    ui->networkConnectedIcon->setPixmap(QIcon::fromTheme("network-wired-activated").pixmap(SC_DPI_T(QSize(128, 128), QSize)));

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::connectivityChanged, this, &OnboardingNetwork::updateConnectivity);
    updateConnectivity();

    //Find out what network devices are available
    for (const NetworkManager::Device::Ptr& device : NetworkManager::networkInterfaces()) {
        QWidget* setupPane = nullptr;
        switch (device->type()) {
            case NetworkManager::Device::Ethernet:
                setupPane = new WiredOnboardingSetup(device->uni());
                break;
            case NetworkManager::Device::Wifi:
                setupPane = new WirelessOnboardingSetup(device->uni());
                break;
            default:
                break;
        }

        if (setupPane) {
            ui->panesLayout->addWidget(setupPane);
        }
    }
}

OnboardingNetwork::~OnboardingNetwork() {
    delete ui;
}

void OnboardingNetwork::updateConnectivity() {
    switch (NetworkManager::connectivity()) {
        case NetworkManager::UnknownConnectivity:
        case NetworkManager::NoConnectivity:
            ui->skipInfoPrompt->setVisible(true);
            ui->nextButton->setText(tr("Continue without Internet"));
            ui->stackedWidget->setCurrentWidget(ui->networkSelectionPage);
            break;
        case NetworkManager::Limited:
        case NetworkManager::Full:
            ui->skipInfoPrompt->setVisible(false);
            ui->nextButton->setText(tr("Next"));
            ui->stackedWidget->setCurrentWidget(ui->networkConnectionCompletePage);
            break;
        case NetworkManager::Portal:
            ui->skipInfoPrompt->setVisible(false);
            ui->nextButton->setText(tr("Next"));
            ui->stackedWidget->setCurrentWidget(ui->networkConnectionCompletePage);
            break;
    }
}

QString OnboardingNetwork::name() {
    return QStringLiteral("OnboardingNetwork");
}

QString OnboardingNetwork::displayName() {
    return tr("Network");
}

void OnboardingNetwork::on_titleLabel_backButtonClicked() {
    StateManager::onboardingManager()->previousStep();
}

void OnboardingNetwork::on_nextButton_clicked() {
    StateManager::onboardingManager()->nextStep();
}


void OnboardingNetwork::on_disconnectButton_clicked() {
    //Disconnect from all active connections
    for (NetworkManager::ActiveConnection::Ptr connection : NetworkManager::activeConnections()) {
        NetworkManager::deactivateConnection(connection->path());
    }
}

