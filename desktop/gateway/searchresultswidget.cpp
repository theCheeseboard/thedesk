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
#include "searchresultswidget.h"
#include "ui_searchresultswidget.h"

#include <Applications/application.h>
#include "appselectionmodel.h"
#include "appselectionmodellistdelegate.h"

struct SearchResultsWidgetPrivate {
    AppSelectionModel* model;
    int maxHeight = 0;
};

SearchResultsWidget::SearchResultsWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SearchResultsWidget) {
    ui->setupUi(this);
    d = new SearchResultsWidgetPrivate();

    d->model = new AppSelectionModel();
    ui->listView->setModel(d->model);
    ui->listView->setItemDelegate(new AppSelectionModelListDelegate(this, true));

    connect(d->model, &AppSelectionModel::dataChanged, this, [ = ] {
        int height = 1;
        for (int i = 0; i < d->model->rowCount(); i++) {
            height += ui->listView->sizeHintForRow(i);
        }
        if (height > this->parentWidget()->height()) height = this->parentWidget()->height() + 1;
        this->setFixedHeight(height);
    });
}

SearchResultsWidget::~SearchResultsWidget() {
    delete ui;
    delete d;
}

void SearchResultsWidget::search(QString query) {
    d->model->search(query);
}

void SearchResultsWidget::launchSelected() {
    QModelIndexList indices = ui->listView->selectionModel()->selectedIndexes();
    if (indices.count() == 0) {
        if (d->model->rowCount() > 0) launch(d->model->index(0));
    } else {
        launch(indices.at(0));
    }
}

void SearchResultsWidget::on_listView_clicked(const QModelIndex& index) {
    launch(index);
}

void SearchResultsWidget::launch(QModelIndex index) {
    index.data(Qt::UserRole + 3).value<ApplicationPointer>()->launch();
    emit closeGateway();
}
