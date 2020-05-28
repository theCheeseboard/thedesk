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
#include "connectionselectionpopover.h"
#include "ui_connectionselectionpopover.h"

struct ConnectionSelectionPopoverPrivate {
    NetworkManager::Connection::List connectionList;
};

ConnectionSelectionPopover::ConnectionSelectionPopover(NetworkManager::Connection::List connectionList, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ConnectionSelectionPopover) {
    ui->setupUi(this);
    d = new ConnectionSelectionPopoverPrivate();

    ui->titleLabel->setBackButtonShown(true);

    d->connectionList = connectionList;
    for (NetworkManager::Connection::Ptr connection : connectionList) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(connection->name());
        ui->connectionList->addItem(item);
    }
}

ConnectionSelectionPopover::~ConnectionSelectionPopover() {
    delete d;
    delete ui;
}

void ConnectionSelectionPopover::on_titleLabel_backButtonClicked() {
    emit reject();
}

void ConnectionSelectionPopover::on_connectionList_itemActivated(QListWidgetItem* item) {
    emit accept(d->connectionList.at(ui->connectionList->row(item)));
}
