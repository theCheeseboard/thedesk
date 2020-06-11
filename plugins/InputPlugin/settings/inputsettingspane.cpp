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
#include "inputsettingspane.h"
#include "ui_inputsettingspane.h"

#include <QIcon>
#include "inputsettingsleftpane.h"

struct InputSettingsPanePrivate {
    InputSettingsLeftPane* leftPane;
};

InputSettingsPane::InputSettingsPane() :
    StatusCenterPane(),
    ui(new Ui::InputSettingsPane) {
    ui->setupUi(this);

    d = new InputSettingsPanePrivate();
    d->leftPane = new InputSettingsLeftPane();
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    connect(d->leftPane, &InputSettingsLeftPane::currentPaneChanged, this, [ = ](int row) {
        ui->stackedWidget->setCurrentIndex(row);
    });
}

InputSettingsPane::~InputSettingsPane() {
    d->leftPane->deleteLater();
    delete d;
    delete ui;
}


QString InputSettingsPane::name() {
    return "InputSettings";
}

QString InputSettingsPane::displayName() {
    return tr("Input");
}

QIcon InputSettingsPane::icon() {
    return QIcon::fromTheme("preferences-desktop-input");
}

QWidget* InputSettingsPane::leftPane() {
    return d->leftPane;
}
