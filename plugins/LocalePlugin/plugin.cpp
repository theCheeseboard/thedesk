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
#include <statemanager.h>
#include <statuscentermanager.h>
#include <localemanager.h>
#include <QApplication>
#include <QDir>
#include <onboardingmanager.h>
#include "tsettings.h"

#include "onboarding/onboardingregion.h"
#include "settings/localesettingspane.h"

struct PluginPrivate {
    int translationSet;

    LocaleSettingsPane* localeSettingsPane;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({
        QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/LocalePlugin/translations"),
        "/usr/share/thedesk/LocalePlugin/translations"
    });

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/LocalePlugin/defaults.conf"));
    tSettings::registerDefaults("/etc/theSuite/theDesk/LocalePlugin/defaults.conf");

    d->localeSettingsPane = new LocaleSettingsPane();
    StateManager::statusCenterManager()->addPane(d->localeSettingsPane, StatusCenterManager::SystemSettings);

    connect(StateManager::onboardingManager(), &OnboardingManager::onboardingRequired, this, [ = ] {
        StateManager::onboardingManager()->addOnboardingStep(new OnboardingRegion);
    });
}

void Plugin::deactivate() {
    StateManager::statusCenterManager()->removePane(d->localeSettingsPane);
    d->localeSettingsPane->deleteLater();
    StateManager::localeManager()->removeTranslationSet(d->translationSet);
}
