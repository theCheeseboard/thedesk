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

#include "defaultspane.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <barmanager.h>
#include <icontextchunk.h>
#include <localemanager.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tsettings.h>

struct PluginPrivate {
        int translationSet;

        DefaultsPane* defaultsPane;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/DefaultsPlugin/translations"),
        "/usr/share/thedesk/DefaultsPlugin/translations"});

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/DefaultsPlugin/thedesk-defaults.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk-defaults.conf");

    d->defaultsPane = new DefaultsPane();
    StateManager::statusCenterManager()->addPane(d->defaultsPane, StatusCenterManager::SystemSettings);
}

void Plugin::deactivate() {
    StateManager::statusCenterManager()->removePane(d->defaultsPane);
    d->defaultsPane->deleteLater();

    StateManager::localeManager()->removeTranslationSet(d->translationSet);
}
