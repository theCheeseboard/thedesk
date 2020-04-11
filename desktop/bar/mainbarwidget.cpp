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
#include "mainbarwidget.h"
#include "ui_mainbarwidget.h"

#include <the-libs_global.h>
#include "gateway/gateway.h"

MainBarWidget::MainBarWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainBarWidget) {
    ui->setupUi(this);

    ui->gatewayButton->setIconSize(SC_DPI_T(QSize(32, 32), QSize));
    connect(ui->chunkContainer, &ChunkContainer::expandedHeightChanged, this, &MainBarWidget::expandedHeightChanged);
    connect(ui->chunkContainer, &ChunkContainer::statusBarHeightChanged, this, &MainBarWidget::statusBarHeightChanged);
}

MainBarWidget::~MainBarWidget() {
    delete ui;
}

int MainBarWidget::statusBarHeight() {
    return ui->chunkContainer->statusBarHeight();
}

int MainBarWidget::expandedHeight() {
    return ui->chunkContainer->expandedHeight() + ui->mainContentsWidget->sizeHint().height();
}

void MainBarWidget::barHeightChanged(int height) {
    ui->chunkContainer->barHeightChanged(height);
}

void MainBarWidget::on_gatewayButton_clicked() {
    Gateway::instance()->show();
}
