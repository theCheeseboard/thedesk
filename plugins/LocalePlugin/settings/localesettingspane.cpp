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
#include "localesettingspane.h"
#include "ui_localesettingspane.h"

#include <QIcon>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <localemanager.h>

#include <TimeDate/desktoptimedate.h>

#include "common.h"

LocaleSettingsPane::LocaleSettingsPane() :
    StatusCenterPane(),
    ui(new Ui::LocaleSettingsPane) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->languageWidget->setFixedWidth(contentWidth);
    ui->regionWidget->setFixedWidth(contentWidth);

    QLocale locale;
    ui->dayOfWeekLabel->setText(locale.dayName(StateManager::localeManager()->formatLocale().firstDayOfWeek()));

    QList<Common::Country> countries = Common::countries();
    for (Common::Country country : countries) {
        ui->countryBox->addItem(country.text, country.country);
        if (country.isCurrent) ui->countryBox->setCurrentIndex(ui->countryBox->count() - 1);
    }

    connect(StateManager::localeManager(), &LocaleManager::formatCountryChanged, this, [ = ] {
        QLocale locale;
        ui->dayOfWeekLabel->setText(locale.dayName(StateManager::localeManager()->formatLocale().firstDayOfWeek()));

        StateManager::statusCenterManager()->requestLogout();
    });

    DesktopTimeDate::makeTimeLabel(ui->timeLabel, DesktopTimeDate::FullTime);
    DesktopTimeDate::makeTimeLabel(ui->dateLabel, DesktopTimeDate::StandardDate);
}

LocaleSettingsPane::~LocaleSettingsPane() {
    delete ui;
}


QString LocaleSettingsPane::name() {
    return "LocaleSettings";
}

QString LocaleSettingsPane::displayName() {
    return tr("Locale");
}

QIcon LocaleSettingsPane::icon() {
    return QIcon::fromTheme("preferences-system-locale");
}

QWidget* LocaleSettingsPane::leftPane() {
    return nullptr;
}

void LocaleSettingsPane::on_countryBox_currentIndexChanged(int index) {
    QLocale::Country country = ui->countryBox->itemData(index).value<QLocale::Country>();
    StateManager::localeManager()->setFormatCountry(country);
}
