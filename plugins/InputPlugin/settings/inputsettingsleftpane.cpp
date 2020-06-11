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
#include "inputsettingsleftpane.h"
#include "ui_inputsettingsleftpane.h"

#include <statemanager.h>
#include <statuscentermanager.h>

InputSettingsLeftPane::InputSettingsLeftPane(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::InputSettingsLeftPane) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->listWidget->setIconSize(SC_DPI_T(QSize(32, 32), QSize));
}

InputSettingsLeftPane::~InputSettingsLeftPane() {
    delete ui;
}

void InputSettingsLeftPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->returnToRootMenu();
}

void InputSettingsLeftPane::on_listWidget_currentRowChanged(int currentRow) {
    emit currentPaneChanged(currentRow);
}
