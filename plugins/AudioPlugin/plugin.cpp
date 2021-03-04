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
#include <QApplication>
#include <statemanager.h>
#include <localemanager.h>
#include <statuscentermanager.h>
#include <barmanager.h>
#include <QDir>
#include "audiochunk.h"
#include <tsettings.h>
#include "eventhandler.h"

struct PluginPrivate {
    int translationSet;

    EventHandler* keyHandler;
    AudioChunk* chunk;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({
        QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/AudioPlugin/translations"),
        "/usr/share/thedesk/AudioPlugin/translations"
    });

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/AudioPlugin/defaults.conf"));
    tSettings::registerDefaults("/etc/theSuite/theDesk/AudioPlugin/defaults.conf");

    d->keyHandler = new EventHandler();
    d->chunk = new AudioChunk();
    StateManager::barManager()->addChunk(d->chunk);
}

void Plugin::deactivate() {
    StateManager::barManager()->removeChunk(d->chunk);
    d->chunk->deleteLater();
    d->keyHandler->deleteLater();
    StateManager::localeManager()->removeTranslationSet(d->translationSet);
}
