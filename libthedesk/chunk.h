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
#ifndef CHUNK_H
#define CHUNK_H

#include <QWidget>

class QuickWidgetContainer;
struct ChunkPrivate;
class Chunk : public QWidget {
        Q_OBJECT
    public:
        explicit Chunk();
        ~Chunk();

        virtual QString name() = 0;
        bool chunkRegistered();

        virtual int expandedHeight() = 0;
        virtual int statusBarHeight() = 0;
        void performStatusBarTransition(qreal percentage);

        void showQuickWidget();
        void hideQuickWidget();

    signals:
        void statusBarHeightChanged();
        void expandedHeightChanged();

        void clicked();

    protected:
        friend QuickWidgetContainer;
        virtual QWidget* quickWidget();

    private:
        ChunkPrivate* d;

        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
};

#endif // CHUNK_H
