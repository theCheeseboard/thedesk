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
#ifndef BARWINDOW_H
#define BARWINDOW_H

#include <Gestures/gestureinteraction.h>
#include <QWidget>
#include <Wm/desktopwmwindow.h>

namespace Ui {
    class BarWindow;
}

struct BarWindowPrivate;
class BarWindow : public QWidget {
        Q_OBJECT

    public:
        explicit BarWindow(QWidget* parent = nullptr);
        ~BarWindow();

    private:
        Ui::BarWindow* ui;
        BarWindowPrivate* d;

        void resizeEvent(QResizeEvent* event);
        void enterEvent(QEnterEvent* event);
        void leaveEvent(QEvent* event);
        void paintEvent(QPaintEvent* event);
        void mouseMoveEvent(QMouseEvent* event);

        void trackWindow(DesktopWmWindowPtr window);

        void updatePrimaryScreen();
        void barHeightChanged();

        void showStatusCenter();
        void hideStatusCenter();

        void showBar();
        void hideBar();

        void trackBarPullDownGesture(GestureInteractionPtr gesture);
        void trackStatusCenterPullDownGesture(GestureInteractionPtr gesture);
        void trackStatusCenterPullUpGesture(GestureInteractionPtr gesture);
};

#endif // BARWINDOW_H
