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
#include "plugin.h"

#include "eventhandler.h"
#include "settings/powersettings.h"
#include "sleepmonitor.h"
#include <DesktopPowerProfiles/desktoppowerprofiles.h>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <UPower/desktopupower.h>
#include <actionquickwidget.h>
#include <barmanager.h>
#include <icontextchunk.h>
#include <localemanager.h>
#include <quickswitch.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tapplication.h>
#include <tsettings.h>

struct PluginPrivate {
        DesktopPowerProfiles* powerProfiles;

        DesktopUPower* upower;
        IconTextChunk* powerChunk;
        EventHandler* logind;
        PowerSettings* powerSettings;
        QuickSwitch* powerStretchSwitch;
        IconTextChunk* powerStretchChunk;

        SleepMonitor* sleepMonitor;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    tApplication::addPluginTranslator(CNTP_TARGET_NAME);

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/PowerPlugin/thedesk-power.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk-power.conf");

    d->powerProfiles = new DesktopPowerProfiles();

    d->upower = new DesktopUPower(this);
    d->powerChunk = new IconTextChunk("Power");
    connect(d->upower, &DesktopUPower::overallStateChanged, this, [=] {
        d->powerChunk->setIcon(d->upower->overallStateIcon());
        d->powerChunk->setText(d->upower->overallStateDescription());

        if (d->upower->shouldShowOverallState() && !d->powerChunk->chunkRegistered()) {
            // Register the chunk
            StateManager::barManager()->addChunk(d->powerChunk);
        } else if (!d->upower->shouldShowOverallState() && d->powerChunk->chunkRegistered()) {
            // Deregister the chunk
            StateManager::barManager()->removeChunk(d->powerChunk);
        }
    });
    d->logind = new EventHandler();

    d->powerSettings = new PowerSettings(d->powerProfiles);
    StateManager::statusCenterManager()->addPane(d->powerSettings, StatusCenterManager::SystemSettings);

    d->powerStretchSwitch = new QuickSwitch("PowerStretch");
    d->powerStretchSwitch->setTitle(tr("Power Stretch"));
    d->powerStretchSwitch->setChecked(d->powerProfiles->currentPowerProfile() == DesktopPowerProfiles::PowerStretch);
    connect(d->powerStretchSwitch, &QuickSwitch::toggled, this, [=](bool powerStretch) {
        if (powerStretch) {
            d->powerProfiles->setCurrentPowerProfile(DesktopPowerProfiles::PowerStretch);
        } else {
            d->powerProfiles->setCurrentPowerProfile(DesktopPowerProfiles::Balanced);
        }
    });

    d->powerStretchChunk = new IconTextChunk("PowerStretch");
    d->powerStretchChunk->setIcon(QIcon::fromTheme("battery-stretch"));
    d->powerStretchChunk->setText(tr("Power Stretch"));
    if (d->powerProfiles->currentPowerProfile() == DesktopPowerProfiles::PowerStretch) StateManager::barManager()->addChunk(d->powerStretchChunk);

    ActionQuickWidget* quickWidget = new ActionQuickWidget(d->powerStretchChunk);
    quickWidget->addAction(QIcon::fromTheme("battery-stretch"), tr("Disable Power Stretch"), [=] {
        d->powerProfiles->setCurrentPowerProfile(DesktopPowerProfiles::Balanced);
    });
    d->powerStretchChunk->setQuickWidget(quickWidget);

    connect(d->powerProfiles, &DesktopPowerProfiles::powerProfileChanged, this, [=] {
        d->powerStretchSwitch->setChecked(d->powerProfiles->currentPowerProfile() == DesktopPowerProfiles::PowerStretch);
        if (d->powerProfiles->currentPowerProfile() == DesktopPowerProfiles::PowerStretch && !d->powerStretchChunk->chunkRegistered()) {
            // Register the chunk
            StateManager::barManager()->addChunk(d->powerStretchChunk);
        } else if (d->powerProfiles->currentPowerProfile() != DesktopPowerProfiles::PowerStretch && d->powerStretchChunk->chunkRegistered()) {
            // Deregister the chunk
            StateManager::barManager()->removeChunk(d->powerStretchChunk);
        }
    });
    StateManager::statusCenterManager()->addSwitch(d->powerStretchSwitch);

    d->sleepMonitor = new SleepMonitor();
}

void Plugin::deactivate() {
    if (d->powerChunk->chunkRegistered()) {
        // Deregister the chunk
        StateManager::barManager()->removeChunk(d->powerChunk);
    }
    if (d->powerStretchChunk->chunkRegistered()) {
        // Deregister the chunk
        StateManager::barManager()->removeChunk(d->powerStretchChunk);
    }

    StateManager::statusCenterManager()->removeSwitch(d->powerStretchSwitch);
    StateManager::statusCenterManager()->removePane(d->powerSettings);

    d->powerStretchSwitch->deleteLater();
    d->powerChunk->deleteLater();
    d->powerStretchChunk->deleteLater();
    d->upower->deleteLater();
    d->logind->deleteLater();
    d->powerSettings->deleteLater();
    d->powerProfiles->deleteLater();
    d->sleepMonitor->deleteLater();
    tApplication::removePluginTranslator(CNTP_TARGET_NAME);
}
