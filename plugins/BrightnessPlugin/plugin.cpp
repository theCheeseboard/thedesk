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

#include "screenbrightnesschunk.h"
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
#include <tsettings.h>

struct PluginPrivate {
        int translationSet;

        KeyGrab* brightnessUp;
        KeyGrab* brightnessDown;

        ScreenBrightnessChunk* chunk;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/BrightnessPlugin/translations"),
        "/usr/share/thedesk/BrightnessPlugin/translations"});
    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/BrightnessPlugin/thedesk-brightness.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk-brightness.conf");

    d->brightnessUp = new KeyGrab(QKeySequence(Qt::Key_MonBrightnessUp), "brightnessUp");
    d->brightnessDown = new KeyGrab(QKeySequence(Qt::Key_MonBrightnessDown), "brightnessDown");
    connect(d->brightnessUp, &KeyGrab::activated, this, [=] {
        SystemScreen* screen = ScreenDaemon::instance()->primayScreen();
        if (screen && screen->isScreenBrightnessAvailable()) {
            double newBrightness = screen->screenBrightness() + 0.05;
            if (newBrightness > 1) newBrightness = 1;
            screen->setScreenBrightness(newBrightness);

            StateManager::instance()->hudManager()->showHud({
                {"icon",  "video-display" },
                {"title", tr("Brightness")},
                {"value", newBrightness   }
            });
        } else {
            StateManager::instance()->hudManager()->showHud({
                {"icon",  "video-display" },
                {"title", tr("Brightness")},
                {"text",  "Unavailable"   }
            });
        }
    });
    connect(d->brightnessDown, &KeyGrab::activated, this, [=] {
        SystemScreen* screen = ScreenDaemon::instance()->primayScreen();
        if (screen && screen->isScreenBrightnessAvailable()) {
            double newBrightness = screen->screenBrightness() - 0.05;
            if (newBrightness < 0) newBrightness = 0;
            screen->setScreenBrightness(newBrightness);

            StateManager::instance()->hudManager()->showHud({
                {"icon",  "video-display" },
                {"title", tr("Brightness")},
                {"value", newBrightness   }
            });
        } else {
            StateManager::instance()->hudManager()->showHud({
                {"icon",  "video-display" },
                {"title", tr("Brightness")},
                {"text",  "Unavailable"   }
            });
        }
    });

    d->chunk = new ScreenBrightnessChunk();
}

void Plugin::deactivate() {
    StateManager::localeManager()->removeTranslationSet(d->translationSet);

    d->brightnessUp->deleteLater();
    d->brightnessDown->deleteLater();

    d->chunk->deleteLater();
}
