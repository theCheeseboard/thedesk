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
#include "keygrab.h"

#include <QKeySequence>
#include <Wm/desktopwm.h>

struct KeyGrabPrivate {
    QKeySequence seq;
    quint64 grabId;
    bool isPaused = true;
};

KeyGrab::KeyGrab(QKeySequence seq, QObject* parent) : QObject(parent) {
    d = new KeyGrabPrivate();
    d->seq = seq;

    init();
}

KeyGrab::KeyGrab(QKeySequence defaultSeq, QString settingName, QObject* parent) : QObject(parent) {
    d = new KeyGrabPrivate();
    d->seq = defaultSeq; //TODO

    init();
}

KeyGrab::~KeyGrab() {
    delete d;
}

void KeyGrab::pause() {
    DesktopWm::ungrabKey(d->grabId);
    d->isPaused = true;
}

void KeyGrab::resume() {
    Qt::Key key = static_cast<Qt::Key>(static_cast<uint>(d->seq[0]) & ~(Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier));
    Qt::KeyboardModifiers mod = static_cast<Qt::KeyboardModifiers>(static_cast<uint>(d->seq[0]) & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier));
    d->grabId = DesktopWm::grabKey(key, mod);
    d->isPaused = false;
}

void KeyGrab::init() {
    connect(DesktopWm::instance(), &DesktopWm::grabbedKeyPressed, this, [ = ](quint64 grabId) {
        if (d->grabId == grabId) emit activated();
    });
    resume();
}
