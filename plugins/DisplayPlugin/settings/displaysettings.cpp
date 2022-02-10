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
#include "displaysettings.h"
#include "ui_displaysettings.h"

#include <statemanager.h>
#include <statuscentermanager.h>

#include <QIcon>
#include <tsettings.h>

#include "arrange/arrangecontroller.h"

struct DisplaySettingsPrivate {
    tSettings settings;
};

DisplaySettings::DisplaySettings() :
    StatusCenterPane(),
    ui(new Ui::DisplaySettings) {
    ui->setupUi(this);

    d = new DisplaySettingsPrivate();

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->arrangeWidget->setFixedWidth(contentWidth);
    ui->arrangeButton->setFixedWidth(contentWidth);
    ui->scalingWidget->setFixedWidth(contentWidth);
    ui->redshiftWidget->setFixedWidth(contentWidth);

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key) {
        if (key.startsWith("Redshift/")) {
            updateRedshiftSettings();
        } else if (key == "Display/dpi") {
            updateDpiSettings();
        }
    });

    updateRedshiftSettings();
    updateDpiSettings();
}

DisplaySettings::~DisplaySettings() {
    delete d;
    delete ui;
}

void DisplaySettings::updateRedshiftSettings() {
    ui->scheduleRedshiftSwitch->setChecked(d->settings.value("Redshift/scheduleRedshift").toBool());
    ui->followSunlightSwitch->setChecked(d->settings.value("Redshift/followSunlightCycle").toBool());
    ui->redshiftStartTime->setTime(QTime::fromMSecsSinceStartOfDay(d->settings.value("Redshift/startTime").toInt()));
    ui->redshiftEndTime->setTime(QTime::fromMSecsSinceStartOfDay(d->settings.value("Redshift/endTime").toInt()));
    ui->redshiftIntensitySlider->setValue(d->settings.value("Redshift/intensity").toInt());

    ui->scheduleRedshiftConditionalWidget->setExpanded(d->settings.value("Redshift/scheduleRedshift").toBool());
    ui->followSunlightConditionalWidget->setExpanded(!d->settings.value("Redshift/followSunlightCycle").toBool());
}

void DisplaySettings::updateDpiSettings() {
    switch (d->settings.value("Display/dpi").toInt()) {
        case 96:
            ui->dpi100Button->setChecked(true);
            break;
        case 120:
            ui->dpi125Button->setChecked(true);
            break;
        case 144:
            ui->dpi150Button->setChecked(true);
            break;
        case 192:
            ui->dpi200Button->setChecked(true);
            break;
    }
}

QString DisplaySettings::name() {
    return "DisplaySettings";
}

QString DisplaySettings::displayName() {
    return tr("Displays");
}

QIcon DisplaySettings::icon() {
    return QIcon::fromTheme("preferences-desktop-display");
}

QWidget* DisplaySettings::leftPane() {
    return nullptr;
}

void DisplaySettings::on_titleLabel_backButtonClicked() {
    StateManager::instance()->statusCenterManager()->showStatusCenterHamburgerMenu();
}

void DisplaySettings::on_scheduleRedshiftSwitch_toggled(bool checked) {
    ui->scheduleRedshiftConditionalWidget->setExpanded(checked);
    d->settings.setValue("Redshift/scheduleRedshift", checked);
}

void DisplaySettings::on_followSunlightSwitch_toggled(bool checked) {
    ui->followSunlightConditionalWidget->setExpanded(!checked);
    d->settings.setValue("Redshift/followSunlightCycle", checked);
}

void DisplaySettings::on_redshiftStartTime_userTimeChanged(const QTime& time) {
    d->settings.setValue("Redshift/startTime", time.msecsSinceStartOfDay());
}

void DisplaySettings::on_redshiftEndTime_userTimeChanged(const QTime& time) {
    d->settings.setValue("Redshift/endTime", time.msecsSinceStartOfDay());
}

void DisplaySettings::on_redshiftIntensitySlider_valueChanged(int value) {
    d->settings.setValue("Redshift/intensity", value);
}

void DisplaySettings::on_arrangeButton_clicked() {
    StateManager::statusCenterManager()->hide();

    ArrangeController* controller = new ArrangeController();
    connect(controller, &ArrangeController::done, controller, &ArrangeController::deleteLater);
    controller->begin();
}

void DisplaySettings::on_dpi100Button_toggled(bool checked) {
    if (checked) {
        d->settings.setValue("Display/dpi", 96);
        StateManager::statusCenterManager()->requestLogout();
    }
}

void DisplaySettings::on_dpi125Button_toggled(bool checked) {
    if (checked) {
        d->settings.setValue("Display/dpi", 120);
        StateManager::statusCenterManager()->requestLogout();
    }
}

void DisplaySettings::on_dpi150Button_toggled(bool checked) {
    if (checked) {
        d->settings.setValue("Display/dpi", 144);
        StateManager::statusCenterManager()->requestLogout();
    }
}

void DisplaySettings::on_dpi200Button_toggled(bool checked) {
    if (checked) {
        d->settings.setValue("Display/dpi", 192);
        StateManager::statusCenterManager()->requestLogout();
    }
}


void DisplaySettings::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}
