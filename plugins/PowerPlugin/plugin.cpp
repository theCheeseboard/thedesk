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

#include <QDebug>
#include <icontextchunk.h>
#include <statemanager.h>
#include <barmanager.h>
#include <statuscentermanager.h>
#include <icontextchunk.h>
#include <localemanager.h>
#include <QIcon>
#include <QApplication>
#include <QDir>
#include <tsettings.h>
#include "logindhandler.h"
#include <UPower/desktopupower.h>

struct PluginPrivate {
    DesktopUPower* upower;
    IconTextChunk* powerChunk;
    LogindHandler* logind;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    StateManager::localeManager()->addTranslationSet({
        QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/PowerPlugin/translations"),
        "/usr/share/thedesk/PowerPlugin/translations"
    });

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/PowerPlugin/defaults.conf"));
    tSettings::registerDefaults("/etc/theSuite/theDesk/PowerPlugin/defaults.conf");

    d->upower = new DesktopUPower(this);
    d->powerChunk = new IconTextChunk("Power");
    connect(d->upower, &DesktopUPower::overallStateChanged, this, [ = ] {
        d->powerChunk->setIcon(d->upower->overallStateIcon());
        d->powerChunk->setText(d->upower->overallStateDescription());

        if (d->upower->shouldShowOverallState() && !d->powerChunk->chunkRegistered()) {
            //Register the chunk
            StateManager::barManager()->addChunk(d->powerChunk);
        } else if (!d->upower->shouldShowOverallState() && d->powerChunk->chunkRegistered()) {
            //Deregister the chunk
            StateManager::barManager()->removeChunk(d->powerChunk);
        }
    });
    d->logind = new LogindHandler();
}

void Plugin::deactivate() {
    if (d->powerChunk->chunkRegistered()) {
        //Deregister the chunk
        StateManager::barManager()->removeChunk(d->powerChunk);
    }

    d->powerChunk->deleteLater();
    d->upower->deleteLater();
    d->logind->deleteLater();
}
