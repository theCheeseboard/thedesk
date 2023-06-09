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

#include "daemons/keyboarddaemon.h"
#include "settings/inputsettingspane.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <localemanager.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tsettings.h>

struct PluginPrivate {
        int translationSet;

        InputSettingsPane* settings;
        KeyboardDaemon* keyboardDaemon;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/InputPlugin/translations"),
        "/usr/share/thedesk/InputPlugin/translations"});

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/InputPlugin/thedesk-input.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk-input.conf");

    d->settings = new InputSettingsPane();
    StateManager::statusCenterManager()->addPane(d->settings, StatusCenterManager::SystemSettings);

    d->keyboardDaemon = new KeyboardDaemon();
}

void Plugin::deactivate() {
    StateManager::statusCenterManager()->removePane(d->settings);
    d->settings->deleteLater();
    StateManager::localeManager()->removeTranslationSet(d->translationSet);
    delete d->keyboardDaemon;
}
