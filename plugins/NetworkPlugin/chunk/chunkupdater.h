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
#ifndef CHUNKUPDATER_H
#define CHUNKUPDATER_H

#include <QObject>
#include <QIcon>

struct ChunkUpdaterPrivate;
class ChunkUpdater : public QObject {
        Q_OBJECT
    public:
        explicit ChunkUpdater(QObject* parent = nullptr);
        ~ChunkUpdater();

        QIcon icon();
        QString text();

    signals:
        void iconChanged(QIcon icon);
        void textChanged(QString text);

    protected:
        void setText(QString text);
        void setIcon(QIcon icon);

    private:
        ChunkUpdaterPrivate* d;
};

#endif // CHUNKUPDATER_H
