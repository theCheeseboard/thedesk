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
#include "quickswitch.h"

struct QuickSwitchPrivate {
    QString name;
    QString title;
    bool checked = false;
};

QuickSwitch::QuickSwitch(QString name, QObject* parent) : QObject(parent) {
    d = new QuickSwitchPrivate();
    d->name = name;
}

QuickSwitch::~QuickSwitch() {
    delete d;
}

QString QuickSwitch::name() {
    return d->name;
}

void QuickSwitch::setTitle(QString title) {
    if (d->title == title) return;
    d->title = title;
    emit titleChanged(title);
}

QString QuickSwitch::title() {
    return d->title;
}

void QuickSwitch::setChecked(bool isChecked) {
    if (d->checked == isChecked) return;
    d->checked = isChecked;
    emit toggled(isChecked);
}

bool QuickSwitch::isChecked() {
    return d->checked;
}
