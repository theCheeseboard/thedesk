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

#include <statemanager.h>
#include <onboardingmanager.h>
#include <localemanager.h>
#include <QLocale>

OnboardingRegion::OnboardingRegion(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingRegion) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);

    QList<QListWidgetItem*> countries;
    for (int i = QLocale::Afghanistan + 1; i < QLocale::LastCountry; i++) {
        if (i == QLocale::World) continue;

        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QLocale::countryToString(static_cast<QLocale::Country>(i)));
        item->setData(Qt::UserRole, i);
        countries.append(item);
    }

    std::sort(countries.begin(), countries.end(), [ = ](const QListWidgetItem * first, const QListWidgetItem * second) {
        return first->text().localeAwareCompare(second->text()) < 0;
    });

    QLocale::Country current = StateManager::localeManager()->formatCountry();
    for (QListWidgetItem* item : countries) {
        ui->countriesWidget->addItem(item);
        if (static_cast<QLocale::Country>(item->data(Qt::UserRole).toInt()) == current) {
            ui->countriesWidget->setCurrentItem(item);
        }
    }
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
        StateManager::localeManager()->setFormatCountry(static_cast<QLocale::Country>(current->data(Qt::UserRole).toInt()));
    }
}
