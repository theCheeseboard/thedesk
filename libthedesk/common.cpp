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
#include "common.h"

int Common::getInsertionIndex(QStringList preferredOrder, QStringList currentItems, QString item) {
    //First, check to see if there is a preferred order for this item
    if (!preferredOrder.contains(item)) {
        //No preferred order so just add it at the end
        return -1;
    }

    //Find where this item is supposed to go
    int beforeIndex = preferredOrder.indexOf(item) - 1;
    if (beforeIndex == -1) {
        //Add this item at the beginning
        return 0;
    }

    //Iterate over the currently loaded items and insert the item at the correct place
    QStringList itemsBefore = preferredOrder.mid(0, beforeIndex + 1);
    for (int i = currentItems.count() - 1; i >= 0; i--) {
        if (itemsBefore.contains(currentItems.at(i))) {
            return i + 1;
        }
    }

    //None of the other items were found so just add it at the beginning
    return 0;
}
