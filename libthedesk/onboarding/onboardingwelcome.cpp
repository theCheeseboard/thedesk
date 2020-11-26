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
#include "onboardingwelcome.h"
#include "ui_onboardingwelcome.h"

#include <statemanager.h>
#include <onboardingmanager.h>
#include <localemanager.h>

OnboardingWelcome::OnboardingWelcome(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingWelcome) {
    ui->setupUi(this);

    ui->emergencyButton->setVisible(false);

}

OnboardingWelcome::~OnboardingWelcome() {
    delete ui;
}

void OnboardingWelcome::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}


QString OnboardingWelcome::name() {
    return "OnboardingWelcome";
}

QString OnboardingWelcome::displayName() {
    return tr("Welcome");
}

void OnboardingWelcome::on_nextButton_clicked() {
    StateManager::onboardingManager()->nextStep();
}

void OnboardingWelcome::on_languageButton_clicked() {
    LocaleManager* manager = StateManager::localeManager();
    bool ok;
    QLocale locale = manager->showLocaleSelector(this->window(), &ok);
    if (ok) {
        if (manager->locales().contains(locale)) manager->removeLocale(locale);
        manager->prependLocale(locale);
    }
}
