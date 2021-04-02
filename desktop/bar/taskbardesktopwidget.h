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
#ifndef TASKBARDESKTOPWIDGET_H
#define TASKBARDESKTOPWIDGET_H

#include <QWidget>

namespace Ui {
    class TaskbarDesktopWidget;
}

class DesktopWmWindow;
typedef QPointer<DesktopWmWindow> DesktopWmWindowPtr;

struct TaskbarDesktopWidgetPrivate;
class TaskbarDesktopWidget : public QWidget {
        Q_OBJECT

    public:
        explicit TaskbarDesktopWidget(uint desktop, QWidget* parent = nullptr);
        ~TaskbarDesktopWidget();

        void moveDesktop(uint newDesktop);

    private slots:
        void on_desktopOverviewButton_clicked();

    private:
        Ui::TaskbarDesktopWidget* ui;
        TaskbarDesktopWidgetPrivate* d;

        void resizeEvent(QResizeEvent* event);

        void windowAdded(DesktopWmWindowPtr window);
        void windowRemoved(DesktopWmWindowPtr window);
        void registerOnDesktop(DesktopWmWindowPtr window);
        void deregisterFromDesktop(DesktopWmWindowPtr window);

        void updateOverviewButtonIcon();
        void updateDesktop();
        void updateAnimation();
        void finishAnimation();
};

#endif // TASKBARDESKTOPWIDGET_H
