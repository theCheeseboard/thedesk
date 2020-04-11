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
#include "chunk.h"

#include <statemanager.h>
#include <barmanager.h>
#include <QMouseEvent>

struct ChunkPrivate {

};

Chunk::Chunk() : QWidget(nullptr) {
    d = new ChunkPrivate();
}

Chunk::~Chunk() {
    delete d;
}

bool Chunk::chunkRegistered() {
    return StateManager::instance()->barManager()->isChunkRegistered(this);
}

void Chunk::performStatusBarTransition(qreal percentage) {
    Q_UNUSED(percentage);
    //noop
}

void Chunk::mousePressEvent(QMouseEvent* event) {
    event->accept();
}

void Chunk::mouseReleaseEvent(QMouseEvent* event) {
    if (event->pos().x() < this->width() && event->pos().x() > 0 &&
        event->pos().y() < this->height() && event->pos().y() > 0) {
        emit clicked();
    }
}
