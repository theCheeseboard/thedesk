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
#include "identitypopover.h"
#include "ui_identitypopover.h"

#include "common.h"

IdentityPopover::IdentityPopover(PolkitQt1::Identity::List identities, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::IdentityPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);

    for (PolkitQt1::Identity identity : identities) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(Common::stringForIdentity(identity));
        item->setData(Qt::UserRole, identity.toString());
        ui->identityList->addItem(item);
    }

    ui->identityList->setFixedWidth(SC_DPI(600));
}

IdentityPopover::~IdentityPopover() {
    delete ui;
}

void IdentityPopover::on_identityList_itemActivated(QListWidgetItem* item) {
    emit selectIdentity(PolkitQt1::Identity::fromString(item->data(Qt::UserRole).toString()));
    emit done();
}

void IdentityPopover::on_titleLabel_backButtonClicked() {
    emit done();
}
