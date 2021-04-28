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
#include "mousesettings.h"
#include "ui_mousesettings.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <tsettings.h>
#include "../backends/settingsbackend.h"

struct MouseSettingsPrivate {
    SettingsBackend* backend;
    tSettings settings;
};

MouseSettings::MouseSettings(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MouseSettings) {
    ui->setupUi(this);
    d = new MouseSettingsPrivate();

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->clickingWidget->setFixedWidth(contentWidth);
    ui->scrollingWidget->setFixedWidth(contentWidth);

    d->backend = SettingsBackend::backendForPlatform();
    if (!d->backend) {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        ui->stackedWidget->setCurrentIndex(0);

        connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
            Q_UNUSED(value);
            updateSettings(key);
        });
        updateAllSettings();
    }
}

MouseSettings::~MouseSettings() {
    delete d;
    delete ui;
}

void MouseSettings::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

void MouseSettings::on_leftPrimaryButton_toggled(bool checked) {
    if (checked) {
        d->settings.setValue("Input/mouse.primary", "left");
    }
}

void MouseSettings::on_rightPrimaryButton_toggled(bool checked) {
    if (checked) {
        d->settings.setValue("Input/mouse.primary", "right");
    }
}

void MouseSettings::updateSettings(QString key) {
    QVariant value = d->settings.value(key);
    if (key == "Input/mouse.primary") {
        if (value == "right") {
            d->backend->setPrimaryMouseButton(SettingsBackend::RightMouseButton);
            d->backend->setPrimaryTouchpadButton(SettingsBackend::RightMouseButton);
            ui->rightPrimaryButton->setChecked(true);
        } else {
            d->backend->setPrimaryMouseButton(SettingsBackend::LeftMouseButton);
            d->backend->setPrimaryTouchpadButton(SettingsBackend::LeftMouseButton);
            ui->leftPrimaryButton->setChecked(true);
        }
    } else if (key == "Input/touchpad.tapClick") {
        d->backend->setTapToClick(value.toBool());
        ui->tapToClickSwitch->setChecked(value.toBool());
    } else if (key == "Input/touchpad.naturalScroll") {
        d->backend->setNaturalScrolling(value.toBool());
        ui->naturalScrollingSwitch->setChecked(value.toBool());
    }
}

void MouseSettings::updateAllSettings() {
    updateSettings("Input/mouse.primary");
    updateSettings("Input/touchpad.tapClick");
    updateSettings("Input/touchpad.naturalScroll");
}

void MouseSettings::on_tapToClickSwitch_toggled(bool checked) {
    d->settings.setValue("Input/touchpad.tapClick", checked);
}

void MouseSettings::on_naturalScrollingSwitch_toggled(bool checked) {
    d->settings.setValue("Input/touchpad.naturalScroll", checked);
}
