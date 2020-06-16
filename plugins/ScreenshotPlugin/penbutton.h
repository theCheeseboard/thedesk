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
#ifndef PENBUTTON_H
#define PENBUTTON_H

#include <QPushButton>

struct PenButtonPrivate;
class PenButton : public QPushButton {
        Q_OBJECT
    public:
        explicit PenButton(QColor color, QWidget* parent = nullptr);
        ~PenButton();

        QSize sizeHint() const;
        QColor color() const;

    signals:

    private:
        void paintEvent(QPaintEvent* event);

        PenButtonPrivate* d;

        void updateChecked();

        // QAbstractButton interface
    protected:
        void checkStateSet();
        void nextCheckState();
};

#endif // PENBUTTON_H
