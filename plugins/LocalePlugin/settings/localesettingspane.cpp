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
#include <QMenu>

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

    QList<Common::Country> countries = Common::countries();
    for (Common::Country country : countries) {
        ui->countryBox->addItem(country.text, country.country);
        if (country.isCurrent) ui->countryBox->setCurrentIndex(ui->countryBox->count() - 1);
    }

    connect(StateManager::localeManager(), &LocaleManager::localesChanged, this, [ = ] {
        this->updateLanguages();
        StateManager::statusCenterManager()->requestLogout();
    });
    connect(StateManager::localeManager(), &LocaleManager::formatCountryChanged, this, [ = ] {
        this->updateRegionalFormats();
        StateManager::statusCenterManager()->requestLogout();
    });
    this->updateLanguages();
    this->updateRegionalFormats();

    DesktopTimeDate::makeTimeLabel(ui->timeLabel, DesktopTimeDate::FullTime);
    DesktopTimeDate::makeTimeLabel(ui->dateLabel, DesktopTimeDate::StandardDate);
}

LocaleSettingsPane::~LocaleSettingsPane() {
    delete ui;
}

void LocaleSettingsPane::updateLanguages() {
    ui->languagesList->clear();

    for (QLocale locale : StateManager::localeManager()->locales()) {
        QListWidgetItem* item = new QListWidgetItem();

        QString text = QStringLiteral("%1 (%2)").arg(QLocale::languageToString(locale.language())).arg(locale.nativeLanguageName());
        if (locale == StateManager::localeManager()->locales().first()) text.append(" · " + tr("Primary"));

        item->setText(text);
        item->setData(Qt::UserRole, locale);
        ui->languagesList->addItem(item);
    }
}

void LocaleSettingsPane::updateRegionalFormats() {
    QLocale locale;
    QLocale formatLocale = StateManager::localeManager()->formatLocale();
    ui->dayOfWeekLabel->setText(locale.dayName(formatLocale.firstDayOfWeek()));
    ui->numbersLabel->setText(formatLocale.toString(123456789.00, 'f', 2));
    ui->currencyLabel->setText(formatLocale.toCurrencyString(1234.56));

    switch (formatLocale.measurementSystem()) {
        case QLocale::MetricSystem:
            ui->measurementLabel->setText(tr("Metric"));
            break;
        case QLocale::ImperialUSSystem:
            ui->measurementLabel->setText(tr("Imperial"));
            break;
        case QLocale::ImperialUKSystem:
            ui->measurementLabel->setText(tr("Imperial (United Kingdom)"));
            break;
    }
}

void LocaleSettingsPane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
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

void LocaleSettingsPane::on_addLanguageButton_clicked() {
    LocaleManager* manager = StateManager::localeManager();
    bool ok;
    QLocale locale = manager->showLocaleSelector(this->window(), &ok);
    if (ok) {
        if (!manager->locales().contains(locale)) {
            manager->addLocale(locale);
        }
    }
}

void LocaleSettingsPane::on_languagesList_customContextMenuRequested(const QPoint& pos) {
    if (ui->languagesList->count() == 1) return;

    QListWidgetItem* item = ui->languagesList->itemAt(pos);
    if (item) {
        QMenu* menu = new QMenu();
        menu->addSection(tr("For this language"));
        if (ui->languagesList->row(item) != 0) menu->addAction(QIcon::fromTheme("go-up"), tr("Move Up"), [ = ] {
            StateManager::localeManager()->moveLocaleUp(item->data(Qt::UserRole).toLocale());
        });
        if (ui->languagesList->row(item) != ui->languagesList->count() - 1) menu->addAction(QIcon::fromTheme("go-down"), tr("Move Down"), [ = ] {
            StateManager::localeManager()->moveLocaleDown(item->data(Qt::UserRole).toLocale());
        });
        menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove"), [ = ] {
            StateManager::localeManager()->removeLocale(item->data(Qt::UserRole).toLocale());
        });
        connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
        menu->popup(ui->languagesList->mapToGlobal(pos));
    }
}
