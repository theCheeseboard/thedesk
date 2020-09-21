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
#ifndef BARMANAGER_H
#define BARMANAGER_H

#include <QObject>
#include <QSharedPointer>

class Chunk;
class ChunkContainer;
class BarWindow;
struct BarManagerPrivate;
class BarManager : public QObject {
        Q_OBJECT

    public:
        class BarLock {
            public:
                ~BarLock();
                void unlock();

            protected:
                friend BarManager;

                BarLock();
                bool isLocked = false;
        };
        typedef QSharedPointer<BarLock> BarLockPtr;

        explicit BarManager(QObject* parent = nullptr);
        ~BarManager();

        void addChunk(Chunk* chunk);
        void removeChunk(Chunk* chunk);
        bool isChunkRegistered(Chunk* chunk);

        bool isBarLocked();

        BarLockPtr acquireLock();

        int barHeight();

    protected:
        friend ChunkContainer;
        friend BarWindow;
        QList<Chunk*> chunks();
        void setBarHeight(int barHeight);

    signals:
        void chunkAdded(Chunk* chunk);
        void chunkRemoved(Chunk* chunk);
        void barHeightTransitioning(qreal percentage);
        void barHeightChanged(int height);
        void barLockedChanged(bool isBarLocked);

    protected:
        BarManagerPrivate* d;
};

#endif // BARMANAGER_H
