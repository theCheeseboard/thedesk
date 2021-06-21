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
#include "recovery.h"
#include "ui_recovery.h"

#include <QIcon>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <powermanager.h>
#include <tsettings.h>

Recovery::Recovery() :
    StatusCenterPane(),
    ui(new Ui::Recovery) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->resetDEContainer->setFixedWidth(contentWidth);
    ui->resetDeviceContainer->setFixedWidth(contentWidth);

    ui->resetDEButton->setProperty("type", "destructive");
    ui->resetDeviceButton->setProperty("type", "destructive");

    if (theLibsGlobal::searchInPath("scallop-reset-ui").isEmpty()) {
        ui->resetDeviceContainer->setVisible(false);
        ui->resetDeviceLine->setVisible(false);
    }
}

Recovery::~Recovery() {
    delete ui;
}

void Recovery::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}


QString Recovery::name() {
    return "SystemRecovery";
}

QString Recovery::displayName() {
    return tr("Recovery");
}

QIcon Recovery::icon() {
    return QIcon::fromTheme("preferences-system-danger");
}

QWidget* Recovery::leftPane() {
    return nullptr;
}

void Recovery::on_resetDEButton_clicked() {
    QMetaObject::Connection* c = new QMetaObject::Connection();
    *c = connect(StateManager::instance()->powerManager(), &PowerManager::powerOffOperationCommencing, this, [ = ] {
        disconnect(*c);

        //Clear the settings
        tSettings settings;
        settings.clear();
        settings.sync();

        tSettings platformSettings("theDesk.platform");
        platformSettings.clear();
        platformSettings.sync();
    });
    StateManager::instance()->powerManager()->showPowerOffConfirmation(PowerManager::LogOut, tr("%1, log out and reset theDesk settings? This action is irreversible.\n\nWe'll go ahead and reset your settings in %n seconds if you don't do anything."))->then([ = ] {
        disconnect(*c);
        delete c;
    });
}

void Recovery::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

void Recovery::on_resetDeviceButton_clicked() {
    QProcess::startDetached("scallop-reset-ui", QStringList());
    StateManager::statusCenterManager()->hide();
}

