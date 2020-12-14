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
#ifndef CURRENTAPPWIDGET_H
#define CURRENTAPPWIDGET_H

#include <QWidget>

namespace Ui {
    class CurrentAppWidget;
}

struct CurrentAppWidgetPrivate;
class CurrentAppWidget : public QWidget {
        Q_OBJECT

    public:
        explicit CurrentAppWidget(QWidget* parent = nullptr);
        ~CurrentAppWidget();

        void barHeightChanging(float barTransitionPercentage);

    private:
        Ui::CurrentAppWidget* ui;
        CurrentAppWidgetPrivate* d;

        void activeWindowChanged();

        // QWidget interface
    protected:
        void enterEvent(QEvent* event);
        void leaveEvent(QEvent* event);
};

#endif // CURRENTAPPWIDGET_H
