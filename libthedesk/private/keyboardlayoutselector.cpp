/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#include "keyboardlayoutselector.h"
#include "ui_keyboardlayoutselector.h"

#include "keyboardlayoutmodel.h"

struct KeyboardLayoutSelectorPrivate {
    KeyboardLayoutModel* model;
};

KeyboardLayoutSelector::KeyboardLayoutSelector(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::KeyboardLayoutSelector) {
    ui->setupUi(this);
    d = new KeyboardLayoutSelectorPrivate();

    ui->titleLabel->setBackButtonShown(true);

    d->model = new KeyboardLayoutModel(this);
    ui->keyboardLayoutSelection->setModel(d->model);
}

KeyboardLayoutSelector::~KeyboardLayoutSelector() {
    delete ui;
    delete d;
}

void KeyboardLayoutSelector::on_keyboardLayoutSelection_activated(const QModelIndex& index) {
    emit accepted(index.data(Qt::UserRole).toString());
}

void KeyboardLayoutSelector::on_titleLabel_backButtonClicked() {
    emit rejected();
}

