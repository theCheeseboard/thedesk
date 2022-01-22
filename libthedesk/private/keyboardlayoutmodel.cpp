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
#include "keyboardlayoutmodel.h"

#include <Wm/desktopwm.h>

struct KeyboardLayoutModelPrivate {
    QStringList layouts;
};

KeyboardLayoutModel::KeyboardLayoutModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new KeyboardLayoutModelPrivate();
    d->layouts = DesktopWm::availableKeyboardLayouts();

    std::sort(d->layouts.begin(), d->layouts.end(), [ = ](const QString & first, const QString & second) {
        return DesktopWm::keyboardLayoutDescription(first).localeAwareCompare(DesktopWm::keyboardLayoutDescription(second)) < 0;
    });
}

KeyboardLayoutModel::~KeyboardLayoutModel() {
    delete d;
}

int KeyboardLayoutModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->layouts.length();
}

QVariant KeyboardLayoutModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    QString layout = d->layouts.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return DesktopWm::keyboardLayoutDescription(layout);
        case Qt::UserRole:
            return layout;
    }

    return QVariant();
}
