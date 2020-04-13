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
#include <QDir>
#include <keygrab.h>
#include <QKeySequence>
#include <Screens/screendaemon.h>
#include <Screens/systemscreen.h>
#include <hudmanager.h>

#include "redshift/colorramp.h"

struct PluginPrivate {
    int translationSet;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

#include <tvariantanimation.h>
void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({
        QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/BrightnessPlugin/translations"),
        "/usr/share/thedesk/BrightnessPlugin/translations"
    });

//    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
//        SystemScreen::GammaRamps ramps;
//        gammaRampsForTemp(&ramps.red, &ramps.green, &ramps.blue, 2000);
//        screen->adjustGammaRamps("redshift", ramps);
//    }
}

void Plugin::deactivate() {
    StateManager::localeManager()->removeTranslationSet(d->translationSet);
}
