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
#include "onboardingbetathankyou.h"
#include "ui_onboardingbetathankyou.h"

#include <statemanager.h>
#include <onboardingmanager.h>

OnboardingBetaThankYou::OnboardingBetaThankYou(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingBetaThankYou) {
    ui->setupUi(this);
    ui->titleLabel->setBackButtonShown(true);
}

OnboardingBetaThankYou::~OnboardingBetaThankYou() {
    delete ui;
}

void OnboardingBetaThankYou::on_titleLabel_backButtonClicked() {
    StateManager::onboardingManager()->previousStep();
}


QString OnboardingBetaThankYou::name() {
    return "OnboardingBetaThankYou";
}

QString OnboardingBetaThankYou::displayName() {
    return tr("A Message");
}
void OnboardingBetaThankYou::on_nextButton_clicked() {
    StateManager::onboardingManager()->nextStep();
}
