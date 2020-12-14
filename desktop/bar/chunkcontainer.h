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
#ifndef CHUNKCONTAINER_H
#define CHUNKCONTAINER_H

#include <QWidget>

namespace Ui {
    class ChunkContainer;
}

class Chunk;
struct ChunkContainerPrivate;
class ChunkContainer : public QWidget {
        Q_OBJECT

    public:
        explicit ChunkContainer(QWidget* parent = nullptr);
        ~ChunkContainer();

        int statusBarHeight();
        int expandedHeight();
        int currentAppWidgetX();
        void barHeightChanged(int height);

    signals:
        void statusBarHeightChanged();
        void expandedHeightChanged();
        void chunksChanged();

    private:
        Ui::ChunkContainer* ui;
        ChunkContainerPrivate* d;

        void paintEvent(QPaintEvent* event);
        void chunkAdded(Chunk* chunk);
        void chunkRemoved(Chunk* chunk);
};

#endif // CHUNKCONTAINER_H
