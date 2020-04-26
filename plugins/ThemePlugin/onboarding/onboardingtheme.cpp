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
#include "onboardingtheme.h"
#include "ui_onboardingtheme.h"

#include <statemanager.h>
#include <onboardingmanager.h>
#include <tsettings.h>

struct OnboardingThemePrivate {
    tSettings* platformSettings;
};

OnboardingTheme::OnboardingTheme(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingTheme) {
    ui->setupUi(this);
    d = new OnboardingThemePrivate();
    d->platformSettings = new tSettings("theDesk.platform", this);

    ui->titleLabel->setBackButtonShown(true);

    connect(d->platformSettings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
        if (key == "Palette/base") {
            updateSettings();
        }
    });

    QPalette lightPal;
    lightPal.setColor(QPalette::Window, QColor(210, 210, 210));
    lightPal.setColor(QPalette::WindowText, Qt::black);
    ui->lightContainer->setPalette(lightPal);

    QPalette darkPal;
    darkPal.setColor(QPalette::Window, QColor(40, 40, 40));
    darkPal.setColor(QPalette::WindowText, Qt::white);
    ui->darkContainer->setPalette(darkPal);

    updateSettings();
}

OnboardingTheme::~OnboardingTheme() {
    delete d;
    delete ui;
}

void OnboardingTheme::updateSettings() {
    QString baseColor = d->platformSettings->value("Palette/base").toString();
    if (baseColor == "dark") {
        ui->darkButton->setChecked(true);
        ui->lightButton->setChecked(false);
        ui->emojiLabel->setPixmap(QPixmap(":/thedesk/themeplugin/icons/moonman.svg"));
        ui->quote->setText(tr("Intellect has spoken; blinding shall be no more!"));
    } else if (baseColor == "light") {
        ui->darkButton->setChecked(false);
        ui->lightButton->setChecked(true);
        ui->emojiLabel->setPixmap(QPixmap(":/thedesk/themeplugin/icons/sunglassesman.svg"));
        ui->quote->setText(tr("One whose wisdom has brought them the light of day!"));
    }
}


QString OnboardingTheme::name() {
    return "OnboardingTheme";
}

QString OnboardingTheme::displayName() {
    return tr("Theme");
}

void OnboardingTheme::on_titleLabel_backButtonClicked() {
    StateManager::onboardingManager()->previousStep();
}

void OnboardingTheme::on_nextButton_clicked() {
    StateManager::onboardingManager()->nextStep();
}

void OnboardingTheme::on_lightButton_toggled(bool checked) {
    if (checked) {
        d->platformSettings->setValue("Palette/base", "light");
    } else {
        updateSettings();
    }
}

void OnboardingTheme::on_darkButton_toggled(bool checked) {
    if (checked) {
        d->platformSettings->setValue("Palette/base", "dark");
    } else {
        updateSettings();
    }
}
