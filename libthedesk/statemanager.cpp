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
#include "statemanager.h"

#include "barmanager.h"
#include "private/statemanager_p.h"

StateManagerPrivate* StateManager::d = nullptr;

StateManager::StateManager() : QObject(nullptr) {
    d->barManager = new BarManager(this);
}

StateManager* StateManager::instance() {
    if (d == nullptr) d = new StateManagerPrivate();

    if (d->instance == nullptr) d->instance = new StateManager;
    return d->instance;
}

BarManager* StateManager::barManager() {
    return d->barManager;
}
