/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#ifndef KEYBOARDLAYOUTCHUNK_H
#define KEYBOARDLAYOUTCHUNK_H

#include <QWidget>
#include <chunk.h>

namespace Ui {
    class KeyboardLayoutChunk;
}

struct KeyboardLayoutChunkPrivate;
class KeyboardLayoutChunk : public Chunk {
        Q_OBJECT

    public:
        explicit KeyboardLayoutChunk();
        ~KeyboardLayoutChunk();

    private:
        Ui::KeyboardLayoutChunk* ui;
        KeyboardLayoutChunkPrivate* d;

        void updateLayouts();
        void updateCurrentLayout();

        // Chunk interface
    public:
        QString name();
        int expandedHeight();
        int statusBarHeight();
};

#endif // KEYBOARDLAYOUTCHUNK_H
