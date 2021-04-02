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
#ifndef TASKBARAPPLICATIONWIDGET_H
#define TASKBARAPPLICATIONWIDGET_H

#include <QPushButton>

class DesktopWmWindow;
typedef QPointer<DesktopWmWindow> DesktopWmWindowPtr;

struct TaskbarApplicationWidgetPrivate;
class TaskbarApplicationWidget : public QPushButton {
        Q_OBJECT
    public:
        explicit TaskbarApplicationWidget(QString desktopEntry, uint desktop, QWidget* parent = nullptr);
        ~TaskbarApplicationWidget();

        void trackWindow(DesktopWmWindowPtr window);
        void removeTrackedWindow(DesktopWmWindowPtr window);
        QList<DesktopWmWindowPtr> trackedWindows();

        void setColor(QColor color);

    signals:
        void iconChanged();
        void windowsRemoved();

    private:
        TaskbarApplicationWidgetPrivate* d;

        void resizeEvent(QResizeEvent* event);
        void updateIcon();
        void updateActive();
};

#endif // TASKBARAPPLICATIONWIDGET_H
