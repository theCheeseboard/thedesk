/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "defaultspane.h"
#include "ui_defaultspane.h"

#include <QIcon>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <MimeAssociations/mimeassociationmanager.h>

DefaultsPane::DefaultsPane() :
    StatusCenterPane(),
    ui(new Ui::DefaultsPane) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->defaultsWidget->setFixedWidth(contentWidth);

    loadDefaults();
}

DefaultsPane::~DefaultsPane() {
    delete ui;
}

void DefaultsPane::loadDefaults() {
    auto populateBox = [ = ](QComboBox * box, QString mimeType) {
        box->clear();

        QList<ApplicationPointer> apps = MimeAssociationManager::applicationsForMimeType(mimeType);

        if (apps.isEmpty()) {
            box->setEnabled(false);
        } else {
            box->setEnabled(true);

            QSignalBlocker blocker(box);
            for (const ApplicationPointer& app : apps) {
                box->addItem(QIcon::fromTheme(app->getProperty("Icon").toString()), app->getProperty("Name").toString(), app->desktopEntry());
            }

            ApplicationPointer app = MimeAssociationManager::defaultApplicationForMimeType(mimeType);
            if (app) {
                for (int i = 0; i < ui->defaultBrowserBox->count(); i++) {
                    if (box->itemData(i).toString() == app->desktopEntry()) ui->defaultBrowserBox->setCurrentIndex(i);
                }
            }
        }
    };

    populateBox(ui->defaultBrowserBox, "x-scheme-handler/http");
    populateBox(ui->defaultMailBox, "x-scheme-handler/mailto");
}

QString DefaultsPane::name() {
    return "DefaultAppsSettings";
}

QString DefaultsPane::displayName() {
    return tr("Default Applications");
}

QIcon DefaultsPane::icon() {
    return QIcon::fromTheme("preferences-desktop-default-applications");
}

QWidget* DefaultsPane::leftPane() {
    return nullptr;
}

void DefaultsPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

void DefaultsPane::on_defaultBrowserBox_currentIndexChanged(int index) {
    QString app = ui->defaultBrowserBox->itemData(index).toString();
    MimeAssociationManager::setDefaultApplicationForMimeType(app, "x-scheme-handler/http");
    MimeAssociationManager::setDefaultApplicationForMimeType(app, "x-scheme-handler/https");
}

void DefaultsPane::on_defaultMailBox_currentIndexChanged(int index) {
    QString app = ui->defaultMailBox->itemData(index).toString();
    MimeAssociationManager::setDefaultApplicationForMimeType(app, "x-scheme-handler/mailto");
}
