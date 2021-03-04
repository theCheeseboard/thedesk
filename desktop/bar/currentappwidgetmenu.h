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
#ifndef CURRENTAPPWIDGETMENU_H
#define CURRENTAPPWIDGETMENU_H

#include <QWidget>
#include <Wm/desktopwmwindow.h>

namespace Ui {
    class CurrentAppWidgetMenu;
}

struct CurrentAppWidgetMenuPrivate;
class CurrentAppWidgetMenu : public QWidget {
        Q_OBJECT

    public:
        explicit CurrentAppWidgetMenu(QWidget* parent = nullptr);
        ~CurrentAppWidgetMenu();

        QSize sizeHint() const;

        void setWindow(DesktopWmWindowPtr window);
        void showForceStopScreen();

        void updateModifiers();

    private slots:
        void on_closeWindowButton_clicked();

        void on_doQuitImmediateButton_clicked();

        void on_cancelImmediateQuitButton_clicked();

        void on_quitImmediateButton_clicked();

        void on_stackedWidget_switchingFrame(int frame);

    signals:
        void done();

    private:
        Ui::CurrentAppWidgetMenu* ui;
        CurrentAppWidgetMenuPrivate* d;

        // QWidget interface
    protected:
        void keyPressEvent(QKeyEvent* event);
        void keyReleaseEvent(QKeyEvent* event);
};

#endif // CURRENTAPPWIDGETMENU_H
