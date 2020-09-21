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
#include "barmanager.h"

#include <statemanager.h>

struct BarManagerPrivate {
    QList<Chunk*> chunks;
    int barHeight;
    int locks = 0;
};

BarManager::BarManager(QObject* parent) : QObject(parent) {
    d = new BarManagerPrivate();
}

BarManager::~BarManager() {
    delete d;
}

void BarManager::addChunk(Chunk* chunk) {
    if (!d->chunks.contains(chunk)) {
        d->chunks.append(chunk);
        emit chunkAdded(chunk);
    }
}

void BarManager::removeChunk(Chunk* chunk) {
    if (d->chunks.contains(chunk)) {
        d->chunks.removeOne(chunk);
        emit chunkRemoved(chunk);
    }
}

bool BarManager::isChunkRegistered(Chunk* chunk) {
    return d->chunks.contains(chunk);
}

bool BarManager::isBarLocked() {
    return d->locks != 0;
}

BarManager::BarLockPtr BarManager::acquireLock() {
    return BarLockPtr(new BarLock());
}

int BarManager::barHeight() {
    return d->barHeight;
}

QList<Chunk*> BarManager::chunks() {
    return d->chunks;
}

void BarManager::setBarHeight(int barHeight) {
    d->barHeight = barHeight;
    emit barHeightChanged(barHeight);
}

BarManager::BarLock::BarLock() {
    isLocked = true;
    StateManager::barManager()->d->locks++;

    //Lock the bar
    emit StateManager::barManager()->barLockedChanged(true);
}

BarManager::BarLock::~BarLock() {
    unlock();
}

void BarManager::BarLock::unlock() {
    if (isLocked) {
        isLocked = false;
        int locks = --StateManager::barManager()->d->locks;
        if (locks == 0) {
            //Unlock the bar
            emit StateManager::barManager()->barLockedChanged(false);
        }
    }
}
