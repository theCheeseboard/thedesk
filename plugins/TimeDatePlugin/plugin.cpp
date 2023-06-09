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

#include "settings/datetimepane.h"

#include "onboarding/onboardingtimezone.h"

struct PluginPrivate {
        int translationSet;

        DateTimePane* dateTimePane;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/TimeDatePlugin/translations"),
        "/usr/share/thedesk/TimeDatePlugin/translations"});

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/TimeDatePlugin/thedesk-timedate.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk-timedate.conf");

    d->dateTimePane = new DateTimePane();
    StateManager::statusCenterManager()->addPane(d->dateTimePane, StatusCenterManager::SystemSettings);

    QObject::connect(StateManager::onboardingManager(), &OnboardingManager::onboardingRequired, [=] {
        StateManager::onboardingManager()->addOnboardingStep(new OnboardingTimeZone());
    });
}

void Plugin::deactivate() {
    StateManager::statusCenterManager()->removePane(d->dateTimePane);
    d->dateTimePane->deleteLater();
    StateManager::localeManager()->removeTranslationSet(d->translationSet);
}
