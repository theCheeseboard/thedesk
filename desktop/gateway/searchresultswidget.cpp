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
#include "gatewaysearchmodel.h"
#include "gatewaysearchmodeldelegate.h"

struct SearchResultsWidgetPrivate {
    GatewaySearchModel* model;
    int maxHeight = 0;
};

SearchResultsWidget::SearchResultsWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SearchResultsWidget) {
    ui->setupUi(this);
    d = new SearchResultsWidgetPrivate();

    d->model = new GatewaySearchModel();
    ui->listView->setModel(d->model);
    ui->listView->setItemDelegate(new GatewaySearchModelDelegate(this));

    connect(d->model, &GatewaySearchModel::dataChanged, this, [ = ] {
        int height = 1;
        for (int i = 0; i < d->model->rowCount(); i++) {
            height += ui->listView->sizeHintForRow(i);
        }
        if (height > this->parentWidget()->height()) height = this->parentWidget()->height();// + 1;
        this->setFixedHeight(height);
    });

    QPalette pal = ui->listView->palette();
    pal.setColor(QPalette::Base, Qt::transparent);
    ui->listView->setPalette(pal);
    ui->line->setVisible(false);
}

SearchResultsWidget::~SearchResultsWidget() {
    delete ui;
    delete d;
}

void SearchResultsWidget::search(QString query) {
    ui->listView->selectionModel()->clear();
    d->model->search(query);
}

void SearchResultsWidget::moveSelectionUp()
{

}

void SearchResultsWidget::moveSelectionDown()
{

}

void SearchResultsWidget::launchSelected() {
    QModelIndexList indices = ui->listView->selectionModel()->selectedIndexes();
    if (indices.isEmpty() || !indices.first().isValid()) {
        if (d->model->rowCount() > 0) launch(d->model->index(0));
    } else {
        launch(indices.at(0));
    }
}

void SearchResultsWidget::on_listView_clicked(const QModelIndex& index) {
    launch(index);
}

void SearchResultsWidget::launch(QModelIndex index) {
    d->model->launch(index);
    emit closeGateway();
}
