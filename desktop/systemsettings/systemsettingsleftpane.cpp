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
#include "systemsettingsleftpane.h"
#include "ui_systemsettingsleftpane.h"

#include <the-libs_global.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <powermanager.h>
#include <tvariantanimation.h>

struct SystemSettingsLeftPanePrivate {
    bool isShowingLogOutRequired = false;
};

SystemSettingsLeftPane::SystemSettingsLeftPane(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SystemSettingsLeftPane) {
    ui->setupUi(this);

    d = new SystemSettingsLeftPanePrivate();

    ui->titleLabel->setBackButtonShown(true);
    ui->mainList->setIconSize(SC_DPI_T(QSize(32, 32), QSize));

    ui->logoutRequiredWidget->setFixedHeight(0);
    ui->logoutButton->setProperty("type", "destructive");

    connect(StateManager::statusCenterManager(), &StatusCenterManager::requestLogout, this, [ = ] {
        if (d->isShowingLogOutRequired) return;
        d->isShowingLogOutRequired = true;

        tVariantAnimation* anim = new tVariantAnimation(this);
        anim->setStartValue(0);
        anim->setEndValue(ui->logoutRequiredWidget->sizeHint().height());
        anim->setDuration(250);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
            ui->logoutRequiredWidget->setFixedHeight(value.toInt());
        });
        connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
        anim->start();
    });
}

SystemSettingsLeftPane::~SystemSettingsLeftPane() {
    delete d;
    delete ui;
}

void SystemSettingsLeftPane::appendItem(QListWidgetItem* item) {
    ui->mainList->addItem(item);
}

void SystemSettingsLeftPane::insertItem(int index, QListWidgetItem* item) {
    ui->mainList->insertItem(index, item);
}

void SystemSettingsLeftPane::removeItem(QListWidgetItem* item) {
    ui->mainList->removeItemWidget(item);
}

void SystemSettingsLeftPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->returnToRootMenu();
}

void SystemSettingsLeftPane::on_mainList_currentRowChanged(int currentRow) {
    emit indexChanged(currentRow);
}

void SystemSettingsLeftPane::on_mainList_clicked(const QModelIndex& index) {
    emit enterMenu(index.row());
}

void SystemSettingsLeftPane::on_logoutButton_clicked() {
    StateManager::instance()->powerManager()->showPowerOffConfirmation(PowerManager::LogOut);
}
