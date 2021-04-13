/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#ifndef SCREENARRANGER_H
#define SCREENARRANGER_H

#include <QWidget>

class SystemScreen;
struct ScreenArrangerPrivate;
class ScreenArranger : public QWidget {
        Q_OBJECT
    public:
        explicit ScreenArranger(QWidget* parent = nullptr);
        ~ScreenArranger();

        QSize sizeHint() const;

    signals:

    private:
        ScreenArrangerPrivate* d;

        void paintEvent(QPaintEvent* event);

        double scaling();
        QRectF totalScreen();
        QRectF screenRect(SystemScreen* screen);
        QRectF draggedScreenRect(QRectF screenRect);

        void updateScreens();

        // QWidget interface
    protected:
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
};

#endif // SCREENARRANGER_H
