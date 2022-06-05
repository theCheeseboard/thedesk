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
#include "onboardingregion.h"
#include "ui_onboardingregion.h"

#include <QLocale>
#include <localemanager.h>
#include <onboardingmanager.h>
#include <statemanager.h>

#include "localeplugincommon.h"

OnboardingRegion::OnboardingRegion(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingRegion) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    search("");
}

OnboardingRegion::~OnboardingRegion() {
    delete ui;
}

void OnboardingRegion::on_nextButton_clicked() {
    StateManager::onboardingManager()->nextStep();
}

QString OnboardingRegion::name() {
    return "OnboardingRegion";
}

QString OnboardingRegion::displayName() {
    return tr("Region");
}
void OnboardingRegion::on_titleLabel_backButtonClicked() {
    StateManager::onboardingManager()->previousStep();
}

void OnboardingRegion::on_countriesWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous) {
    if (current) {
        StateManager::localeManager()->setFormatCountry(current->data(Qt::UserRole).value<QLocale::Country>());
    }
}

void OnboardingRegion::on_searchEdit_textChanged(const QString& arg1) {
    search(arg1);
}

void OnboardingRegion::search(QString query) {
    ui->countriesWidget->clear();

    QList<LocalePluginCommon::Country> countries = LocalePluginCommon::countries();
    for (const LocalePluginCommon::Country& country : qAsConst(countries)) {
        if (!country.text.contains(query, Qt::CaseInsensitive)) continue;

        QListWidgetItem* item = new QListWidgetItem();
        item->setText(country.text);
        item->setData(Qt::UserRole, country.country);
        ui->countriesWidget->addItem(item);
        if (country.isCurrent) ui->countriesWidget->setCurrentItem(item);
    }
}
