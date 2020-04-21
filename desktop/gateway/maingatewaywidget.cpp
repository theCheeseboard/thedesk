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
#include "maingatewaywidget.h"
#include "ui_maingatewaywidget.h"

#include <Wm/desktopwm.h>
#include <QScroller>
#include <tvariantanimation.h>
#include <application.h>
#include "appselectionmodel.h"
#include "appselectionmodellistdelegate.h"
#include "session/endsession.h"
#include <statemanager.h>
#include <powermanager.h>

struct MainGatewayWidgetPrivate {
    AppSelectionModel* model;
};

MainGatewayWidget::MainGatewayWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainGatewayWidget) {
    ui->setupUi(this);

    d = new MainGatewayWidgetPrivate();
    d->model = new AppSelectionModel(this);
    connect(d->model, &AppSelectionModel::loading, this, [ = ] {
        ui->gatewayTypeStack->setCurrentWidget(ui->gatewayLoading);
    });
    connect(d->model, &AppSelectionModel::ready, this, [ = ] {
        ui->gatewayTypeStack->setCurrentWidget(ui->gatewayList);
    });
    ui->gatewayTypeStack->setCurrentWidget(ui->gatewayLoading);
    ui->gatewayTypeStack->setCurrentAnimation(tStackedWidget::Fade);

    ui->usernameLabel->setText(tr("Hey, %1!").arg(DesktopWm::userDisplayName()));

    ui->appsList->setModel(d->model);
    ui->appsList->setItemDelegate(new AppSelectionModelListDelegate(this, true));
    ui->appsList->setFocusProxy(ui->searchBox);
    QScroller::grabGesture(ui->appsList->viewport(), QScroller::LeftMouseButtonGesture);

    this->setFocusProxy(ui->searchBox);
}

MainGatewayWidget::~MainGatewayWidget() {
    delete d;
    delete ui;
}

QSize MainGatewayWidget::sizeHint() const {
    QSize sizeHint = QWidget::sizeHint();
    sizeHint.setWidth(SC_DPI(400));
    return sizeHint;
}

void MainGatewayWidget::clearState() {
    ui->searchBox->setText("");
    d->model->search("");
}

void MainGatewayWidget::on_searchBox_textEdited(const QString& arg1) {
    d->model->search(arg1);
}

void MainGatewayWidget::on_appsList_clicked(const QModelIndex& index) {
    launch(index);
}

void MainGatewayWidget::on_searchBox_returnPressed() {
    QModelIndexList indices = ui->appsList->selectionModel()->selectedIndexes();
    if (indices.count() == 0) {
        if (d->model->rowCount() > 0) launch(d->model->index(0));
    } else {
        launch(indices.at(0));
    }
}

void MainGatewayWidget::launch(QModelIndex applicationIndex) {
    applicationIndex.data(Qt::UserRole + 3).value<ApplicationPointer>()->launch();
    emit closeGateway();
}

void MainGatewayWidget::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void MainGatewayWidget::on_endSessionButton_clicked() {
    StateManager::instance()->powerManager()->showPowerOffConfirmation();
}
