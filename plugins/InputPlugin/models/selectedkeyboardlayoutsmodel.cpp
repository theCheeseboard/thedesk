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
#include "selectedkeyboardlayoutsmodel.h"

#include <tsettings.h>
#include <Wm/desktopwm.h>

struct SelectedKeyboardLayoutsModelPrivate {
    tSettings settings;
};

SelectedKeyboardLayoutsModel::SelectedKeyboardLayoutsModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new SelectedKeyboardLayoutsModelPrivate();

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key) {
        if (key == "Input/keyboard.layouts") emit dataChanged(index(0), index(rowCount()));
    });
}

SelectedKeyboardLayoutsModel::~SelectedKeyboardLayoutsModel() {
    delete d;
}

int SelectedKeyboardLayoutsModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return d->settings.delimitedList("Input/keyboard.layouts").length();
}

QVariant SelectedKeyboardLayoutsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())  return QVariant();
    if (index.row() >= rowCount()) return QVariant();

    QString layout = d->settings.delimitedList("Input/keyboard.layouts").at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return DesktopWm::keyboardLayoutDescription(layout);
        case Qt::UserRole:
            return layout;
    }

    return QVariant();
}

void SelectedKeyboardLayoutsModel::addLayout(QString layout) {
    QStringList layouts = d->settings.delimitedList("Input/keyboard.layouts");
    if (layouts.contains(layout)) return;

    layouts.append(layout);
    layouts.removeAll("");
    d->settings.setDelimitedList("Input/keyboard.layouts", layouts);
}

void SelectedKeyboardLayoutsModel::moveUp(QString layout) {
    QStringList layouts = d->settings.delimitedList("Input/keyboard.layouts");
    if (!layouts.contains(layout)) return;

    int index = layouts.indexOf(layout);
    if (index != 0) {
        layouts.removeAt(index);
        layouts.insert(index - 1, layout);
        d->settings.setDelimitedList("Input/keyboard.layouts", layouts);
    }
}

void SelectedKeyboardLayoutsModel::moveDown(QString layout) {
    QStringList layouts = d->settings.delimitedList("Input/keyboard.layouts");
    if (!layouts.contains(layout)) return;

    int index = layouts.indexOf(layout);
    if (index != 0) {
        layouts.removeAt(index);
        layouts.insert(index + 1, layout);
        d->settings.setDelimitedList("Input/keyboard.layouts", layouts);
    }
}

void SelectedKeyboardLayoutsModel::removeLayout(QString layout) {
    QStringList layouts = d->settings.delimitedList("Input/keyboard.layouts");
    layouts.removeAll(layout);
    d->settings.setDelimitedList("Input/keyboard.layouts", layouts);
}
