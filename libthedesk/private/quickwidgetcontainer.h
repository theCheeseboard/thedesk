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
#ifndef QUICKWIDGETCONTAINER_H
#define QUICKWIDGETCONTAINER_H

#include <QWidget>

namespace Ui {
    class QuickWidgetContainer;
}

class Chunk;
class CurrentAppWidget;
struct QuickWidgetContainerPrivate;
class QuickWidgetContainer : public QWidget {
        Q_OBJECT

    public:
        explicit QuickWidgetContainer(QWidget* parent = nullptr);
        ~QuickWidgetContainer();

        void showContainer();
        void hideContainer();
        bool isShowing();

    signals:
        void showing();
        void hiding();

    protected:
        friend CurrentAppWidget;
        void setQuickWidget(QWidget* widget);

    private:
        Ui::QuickWidgetContainer* ui;
        QuickWidgetContainerPrivate* d;

        void paintEvent(QPaintEvent* event);
        void changeEvent(QEvent* event);
        bool eventFilter(QObject* watched, QEvent* event);

        void calculatePosition();

};

#endif // QUICKWIDGETCONTAINER_H
