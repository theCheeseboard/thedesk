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
#ifndef SEARCHRESULTSWIDGET_H
#define SEARCHRESULTSWIDGET_H

#include <QWidget>

namespace Ui {
    class SearchResultsWidget;
}

struct SearchResultsWidgetPrivate;
class SearchResultsWidget : public QWidget {
        Q_OBJECT

    public:
        explicit SearchResultsWidget(QWidget* parent = nullptr);
        ~SearchResultsWidget();

        void search(QString query);

        void launchSelected();

    private slots:
        void on_listView_clicked(const QModelIndex& index);

    signals:
        void closeGateway();

    private:
        Ui::SearchResultsWidget* ui;
        SearchResultsWidgetPrivate* d;

        void launch(QModelIndex index);
};

#endif // SEARCHRESULTSWIDGET_H
