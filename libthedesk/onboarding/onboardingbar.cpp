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
#include "onboardingbar.h"
#include "ui_onboardingbar.h"

#include <statemanager.h>
#include <onboardingmanager.h>
#include <tvariantanimation.h>
#include <TimeDate/desktoptimedate.h>
#include <QGraphicsOpacityEffect>

struct OnboardingBarPrivate {
    QGraphicsOpacityEffect* dateOpacity;
};

OnboardingBar::OnboardingBar(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::OnboardingBar) {
    ui->setupUi(this);

    d = new OnboardingBarPrivate();
    d->dateOpacity = new QGraphicsOpacityEffect();
    d->dateOpacity->setOpacity(StateManager::onboardingManager()->dateVisible() ? 1.0 : 0.0);
    ui->dateWidget->setGraphicsEffect(d->dateOpacity);

    QPalette pal = this->palette();
    pal.setColor(QPalette::WindowText, Qt::white);
    this->setPalette(pal);

    DesktopTimeDate::makeTimeLabel(ui->clock, DesktopTimeDate::Time);
    DesktopTimeDate::makeTimeLabel(ui->date, DesktopTimeDate::StandardDate);

    connect(StateManager::onboardingManager(), &OnboardingManager::dateVisibleChanged, this, [ = ](bool dateVisible) {
        tVariantAnimation* anim = new tVariantAnimation();
        anim->setStartValue(d->dateOpacity->opacity());
        anim->setEndValue(dateVisible ? 1.0 : 0.0);
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
            d->dateOpacity->setOpacity(value.toReal());
        });
        connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
        anim->start();
    });
}

OnboardingBar::~OnboardingBar() {
    delete ui;
}

void OnboardingBar::on_closeButton_clicked() {
    emit closeClicked();
}

void OnboardingBar::on_muteButton_toggled(bool checked) {
    emit muteToggled(checked);
}
