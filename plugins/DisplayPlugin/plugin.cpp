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

#include "settings/displaysettings.h"
#include "tsettings.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QKeySequence>
#include <Screens/screendaemon.h>
#include <Screens/systemscreen.h>
#include <hudmanager.h>
#include <keygrab.h>
#include <localemanager.h>
#include <statemanager.h>
#include <statuscentermanager.h>

#include <Screens/screendaemon.h>

#include "redshift/redshiftdaemon.h"

struct PluginPrivate {
        int translationSet;

        DisplaySettings* settingsPage;
        RedshiftDaemon* daemon;

        tSettings* settings;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/DisplayPlugin/translations"),
        "/usr/share/thedesk/DisplayPlugin/translations"});

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/DisplayPlugin/thedesk-display.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk-display.conf");

    d->settingsPage = new DisplaySettings();
    d->daemon = new RedshiftDaemon();

    StateManager::statusCenterManager()->addPane(d->settingsPage, StatusCenterManager::SystemSettings);

    d->settings = new tSettings();
    connect(d->settings, &tSettings::settingChanged, this, [this] {
        ScreenDaemon::instance()->setDpi(d->settings->value("Display/dpi").toInt());
    });
    ScreenDaemon::instance()->setDpi(d->settings->value("Display/dpi").toInt());
}

void Plugin::deactivate() {
    StateManager::localeManager()->removeTranslationSet(d->translationSet);
    d->daemon->deleteLater();

    StateManager::statusCenterManager()->removePane(d->settingsPage);
    d->settingsPage->deleteLater();

    d->settings->deleteLater();
}
