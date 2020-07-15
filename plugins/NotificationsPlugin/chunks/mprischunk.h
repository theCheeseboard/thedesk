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
#ifndef MPRISCHUNK_H
#define MPRISCHUNK_H

#include <chunk.h>

namespace Ui {
    class MprisChunk;
}

struct MprisChunkPrivate;
class MprisChunk : public Chunk {
        Q_OBJECT

    public:
        explicit MprisChunk();
        ~MprisChunk();

    private:
        Ui::MprisChunk* ui;
        MprisChunkPrivate* d;

        void setupPlayer(QString service);
        void setCurrentPlayer(QString player);
        void updateMetadata();
        void updateApplication();
        void updateState();

        // Chunk interface
    public:
        QString name();
        int expandedHeight();
        int statusBarHeight();
    private slots:
        void on_playPauseButton_clicked();
        void on_backButton_clicked();
        void on_forwardButton_clicked();
};

#endif // MPRISCHUNK_H
