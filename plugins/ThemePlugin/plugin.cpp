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

#include "tsettings.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <localemanager.h>
#include <onboardingmanager.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tapplication.h>

#include "onboarding/onboardingtheme.h"
#include "settings/themesettingspane.h"

struct PluginPrivate {
        ThemeSettingsPane* themeSettingsPane;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    tApplication::addPluginTranslator(CNTP_TARGET_NAME);

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/ThemePlugin/thedesk-theme.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk-theme.conf");

    d->themeSettingsPane = new ThemeSettingsPane();
    StateManager::statusCenterManager()->addPane(d->themeSettingsPane, StatusCenterManager::SystemSettings);

    connect(StateManager::onboardingManager(), &OnboardingManager::onboardingRequired, this, [=] {
        StateManager::onboardingManager()->addOnboardingStep(new OnboardingTheme);
    });
}

void Plugin::deactivate() {
    StateManager::statusCenterManager()->removePane(d->themeSettingsPane);
    d->themeSettingsPane->deleteLater();
    tApplication::removePluginTranslator(CNTP_TARGET_NAME);
}
