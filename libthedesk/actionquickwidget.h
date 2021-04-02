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
#ifndef ACTIONQUICKWIDGET_H
#define ACTIONQUICKWIDGET_H

#include <QWidget>
#include <functional>

namespace Ui {
    class ActionQuickWidget;
}

class Chunk;
class QuickWidgetContainer;
struct ActionQuickWidgetPrivate;
class ActionQuickWidget : public QWidget {
        Q_OBJECT

    public:
        explicit ActionQuickWidget(Chunk* parent = nullptr);
        explicit ActionQuickWidget(QuickWidgetContainer* parent = nullptr);
        ~ActionQuickWidget();

        void addAction(QAction* action);
        QAction* addAction(QString text, std::function<void()> triggered);
        QAction* addAction(QIcon icon, QString text, std::function<void()> triggered);

    private:
        Ui::ActionQuickWidget* ui;
        ActionQuickWidgetPrivate* d;

        bool event(QEvent* event);
};

#endif // ACTIONQUICKWIDGET_H
