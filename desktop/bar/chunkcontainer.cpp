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
#include "chunkcontainer.h"
#include "ui_chunkcontainer.h"

#include <statemanager.h>
#include <barmanager.h>
#include <chunk.h>

struct ChunkContainerPrivate {
    BarManager* barManager;

    QList<QPair<QString, Chunk*>> loadedChunks;
    QStringList preferredChunkOrder = {
        "OverviewClock",
        "network-cellular",
        "network-wireless",
    };
};

ChunkContainer::ChunkContainer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ChunkContainer) {
    ui->setupUi(this);
    d = new ChunkContainerPrivate();
    d->barManager = StateManager::barManager();

    connect(d->barManager, &BarManager::chunkAdded, this, &ChunkContainer::chunkAdded);
    connect(d->barManager, &BarManager::chunkRemoved, this, &ChunkContainer::chunkRemoved);
    for (Chunk* chunk : d->barManager->chunks()) {
        this->chunkAdded(chunk);
    }

    connect(StateManager::barManager(), &BarManager::barHeightTransitioning, this, [ = ](qreal percentage) {
        int spacing = 3 + 3 * percentage;
        ui->chunkLayout->setSpacing(spacing);
    });
}

ChunkContainer::~ChunkContainer() {
    delete d;
    delete ui;
}

int ChunkContainer::statusBarHeight() {
    int maxHeight = 0;
    for (Chunk* chunk : d->barManager->chunks()) {
        maxHeight = qMax(chunk->statusBarHeight(), maxHeight);
    }
    return maxHeight;
}

int ChunkContainer::expandedHeight() {
    int maxHeight = 0;
    for (Chunk* chunk : d->barManager->chunks()) {
        maxHeight = qMax(chunk->expandedHeight(), maxHeight);
    }
    return maxHeight;
}

void ChunkContainer::barHeightchanged(int height) {
    if (height >= statusBarHeight() && height <= expandedHeight()) {
        this->setFixedHeight(height);
    }

    qreal percentageAnim = static_cast<qreal>((height - statusBarHeight())) / (expandedHeight() - statusBarHeight());
    if (percentageAnim < 0) percentageAnim = 0;
    if (percentageAnim > 1) percentageAnim = 1;
    d->barManager->barHeightTransitioning(percentageAnim);
}

void ChunkContainer::paintEvent(QPaintEvent* event) {

}

void ChunkContainer::chunkAdded(Chunk* chunk) {
    //First, check to see if there is a preferred order for this chunk
    if (!d->preferredChunkOrder.contains(chunk->name())) {
        //No preferred order so just add it at the end
        ui->chunkLayout->addWidget(chunk);
        d->loadedChunks.append({chunk->name(), chunk});
    }

    //Find where this chunk is supposed to go
    int beforeIndex = d->preferredChunkOrder.indexOf(chunk->name()) - 1;
    if (beforeIndex == -1) {
        //Add this chunk at the beginning
        ui->chunkLayout->insertWidget(0, chunk);
        d->loadedChunks.insert(0, {chunk->name(), chunk});
        return;
    }

    //Iterate over the currently loaded chunks and insert the chunk at the correct place
    QStringList chunksBefore = d->preferredChunkOrder.mid(0, beforeIndex + 1);
    for (int i = d->loadedChunks.count() - 1; i >= 0; i--) {
        if (chunksBefore.contains(d->loadedChunks.at(i).first)) {
            ui->chunkLayout->insertWidget(i + 1, chunk);
            d->loadedChunks.insert(i + 1, {chunk->name(), chunk});
            return;
        }
    }

    //None of the other chunks were found so just add it at the beginning
    ui->chunkLayout->insertWidget(0, chunk);
    d->loadedChunks.insert(0, {chunk->name(), chunk});
}

void ChunkContainer::chunkRemoved(Chunk* chunk) {
    for (int i = 0; i < d->loadedChunks.count(); i++) {
        if (d->loadedChunks.at(i).second == chunk) {
            ui->chunkLayout->removeWidget(chunk);
            d->loadedChunks.removeAt(i);
            return;
        }
    }
}
