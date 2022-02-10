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
#include "accessibilitysettingspane.h"
#include "ui_accessibilitysettingspane.h"

#include <QIcon>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tsettings.h>

struct AccessibilitySettingsPanePrivate {
    tSettings settings;
};

AccessibilitySettingsPane::AccessibilitySettingsPane() :
    StatusCenterPane(),
    ui(new Ui::AccessibilitySettingsPane) {
    ui->setupUi(this);

    d = new AccessibilitySettingsPanePrivate();

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->typingWidget->setFixedWidth(contentWidth);
    ui->pointingWidget->setFixedWidth(contentWidth);

    connect(&d->settings, &tSettings::settingChanged, this, &AccessibilitySettingsPane::updateSetting);
    for (QString setting : {
            "Accessibility/stickykeys.active",
            "Accessibility/mousekeys.active"
        }) {
        updateSetting(setting, d->settings.value(setting));
    }
}

AccessibilitySettingsPane::~AccessibilitySettingsPane() {
    delete d;
    delete ui;
}

QString AccessibilitySettingsPane::name() {
    return "AccessibilitySettings";
}

QString AccessibilitySettingsPane::displayName() {
    return tr("Accessibility");
}

QIcon AccessibilitySettingsPane::icon() {
    return QIcon::fromTheme("preferences-desktop-accessibility");
}

QWidget* AccessibilitySettingsPane::leftPane() {
    return nullptr;
}

void AccessibilitySettingsPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

void AccessibilitySettingsPane::on_stickyKeysSwitch_toggled(bool checked) {
    d->settings.setValue("Accessibility/stickykeys.active", checked);
}

void AccessibilitySettingsPane::updateSetting(QString key, QVariant value) {
    if (key == "Accessibility/stickykeys.active") {
        ui->stickyKeysSwitch->setChecked(value.toBool());
    } else if (key == "Accessibility/mousekeys.active") {
        ui->mouseKeysSwitch->setChecked(value.toBool());
    }
}

void AccessibilitySettingsPane::on_mouseKeysSwitch_toggled(bool checked) {
    d->settings.setValue("Accessibility/mousekeys.active", checked);
}


void AccessibilitySettingsPane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}
