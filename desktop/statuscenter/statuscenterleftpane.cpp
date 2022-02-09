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
#include "statuscenterleftpane.h"
#include "ui_statuscenterleftpane.h"

#include <QPainter>
#include "leftpanedelegate.h"
#include <the-libs_global.h>

struct StatusCenterLeftPanePrivate {
    bool attached;
};

StatusCenterLeftPane::StatusCenterLeftPane(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::StatusCenterLeftPane) {
    ui->setupUi(this);

    d = new StatusCenterLeftPanePrivate();
    ui->mainList->setIconSize(SC_DPI_T(QSize(32, 32), QSize));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->mainList->setItemDelegate(new LeftPaneDelegate(this));
}

StatusCenterLeftPane::~StatusCenterLeftPane() {
    delete d;
    delete ui;
}

void StatusCenterLeftPane::appendItem(QListWidgetItem* item) {
    ui->mainList->addItem(item);
}

void StatusCenterLeftPane::insertItem(int index, QListWidgetItem* item) {
    ui->mainList->insertItem(index, item);
}

void StatusCenterLeftPane::removeItem(QListWidgetItem* item) {
    ui->mainList->removeItemWidget(item);
}

void StatusCenterLeftPane::setSelected(QListWidgetItem* item) {
    item->setSelected(true);
}

void StatusCenterLeftPane::setAttached(bool attached) {
    d->attached = attached;
    if (attached) {
        if (this->layoutDirection() == Qt::RightToLeft) {
            this->layout()->setContentsMargins(1, 0, 1, 0);
        } else {
            this->layout()->setContentsMargins(0, 0, 1, 0);
        }
        this->setFixedWidth(SC_DPI(300) + 1);
    } else {
        this->layout()->setContentsMargins(0, 0, 0, 0);
        this->setFixedWidth(SC_DPI(300));
    }
}

void StatusCenterLeftPane::pushMenu(QWidget* menu) {
    ui->stackedWidget->addWidget(menu);
    ui->stackedWidget->setCurrentWidget(menu);
}

void StatusCenterLeftPane::popMenu() {
    if (ui->stackedWidget->count() == 1) return;

    QMetaObject::Connection* connection = new QMetaObject::Connection();
    *connection = connect(ui->stackedWidget, &tStackedWidget::currentChanged, this, [ = ] {
        disconnect(*connection);
        delete connection;

        QWidget* lastWidget = ui->stackedWidget->widget(ui->stackedWidget->currentIndex() + 1);
        ui->stackedWidget->removeWidget(lastWidget);
    });
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);
}

QWidget* StatusCenterLeftPane::peekMenu() {
    return ui->stackedWidget->currentWidget();
}

void StatusCenterLeftPane::paintEvent(QPaintEvent* event) {
    if (d->attached) {
        QPainter painter(this);
        painter.setPen(theLibsGlobal::lineColor(this->palette().color(QPalette::WindowText)));
        if (this->layoutDirection() == Qt::RightToLeft) {
            painter.drawLine(0, 0, 0, this->height());
        } else {
            painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height());
        }
    }
}

void StatusCenterLeftPane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    } else if (event->type() == QEvent::LayoutDirectionChange) {
        this->setAttached(d->attached);
    }
}

void StatusCenterLeftPane::on_mainList_currentRowChanged(int currentRow) {
    emit indexChanged(currentRow);
}

void StatusCenterLeftPane::on_mainList_clicked(const QModelIndex& index) {
    emit enterMenu(index.row());
}
