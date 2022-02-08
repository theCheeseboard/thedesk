/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#include "keyboarddaemon.h"

#include <QKeySequence>
#include <tsettings.h>
#include <statemanager.h>
#include <hudmanager.h>
#include <barmanager.h>
#include <keygrab.h>
#include <Wm/desktopwm.h>
#include "chunks/keyboardlayoutchunk.h"

struct KeyboardDaemonPrivate {
    tSettings settings;
    KeyboardLayoutChunk* chunk;
};

KeyboardDaemon::KeyboardDaemon(QObject* parent)
    : QObject{parent} {
    d = new KeyboardDaemonPrivate();

    d->chunk = new KeyboardLayoutChunk();

    KeyGrab* nextLayoutGrab = new KeyGrab(QKeySequence(Qt::MetaModifier | Qt::Key_Space), "nextlayout", this);
    connect(nextLayoutGrab, &KeyGrab::activated, this, &KeyboardDaemon::setNextKeyboardLayout);
    KeyGrab* nextLayoutGrabAlternate = new KeyGrab(QKeySequence(Qt::MetaModifier | Qt::Key_Return), this);
    connect(nextLayoutGrabAlternate, &KeyGrab::activated, this, &KeyboardDaemon::setNextKeyboardLayout);

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key) {
        if (key == "Input/keyboard.layouts") {
            updateChunkVisibility();

            //Ensure that we have a valid keyboard layout
            QStringList layouts = d->settings.delimitedList("Input/keyboard.layouts");

            //Failsafe
            if (layouts.isEmpty()) return;

            if (!layouts.contains(DesktopWm::currentKeyboardLayout())) {
                //Set the first desktop layout
                DesktopWm::setCurrentKeyboardLayout(layouts.at(0));
            }
        }
    });
    updateChunkVisibility();
}

KeyboardDaemon::~KeyboardDaemon() {
    d->chunk->deleteLater();
    delete d;
}

void KeyboardDaemon::setNextKeyboardLayout() {
    QStringList layouts = d->settings.delimitedList("Input/keyboard.layouts");
    if (layouts.length() <= 1) return;

    int nextLayout = layouts.indexOf(DesktopWm::currentKeyboardLayout()) + 1;
    if (nextLayout >= layouts.length()) nextLayout = 0;

    QString layout = layouts.at(nextLayout);
    DesktopWm::setCurrentKeyboardLayout(layout);

    StateManager::instance()->hudManager()->showHud({
        {"icon", "input-keyboard"},
        {"title", tr("Keyboard Layout")},
        {"text", DesktopWm::keyboardLayoutDescription(layout)}
    });
}

void KeyboardDaemon::updateChunkVisibility() {
    bool shouldBeVisible = d->settings.delimitedList("Input/keyboard.layouts").length() > 1;
    if (shouldBeVisible && !d->chunk->chunkRegistered()) {
        StateManager::barManager()->addChunk(d->chunk);
    } else if (!shouldBeVisible && d->chunk->chunkRegistered()) {
        StateManager::barManager()->removeChunk(d->chunk);
    }
}
