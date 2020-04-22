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
#include "systemsettings.h"
#include "ui_systemsettings.h"

#include <QIcon>
#include <QListWidgetItem>
#include <statemanager.h>
#include <statuscentermanager.h>
#include "common/common.h"
#include "systemsettingsleftpane.h"
#include "statuscenter/statuscenterleftpane.h"
#include <tsettings.h>

#include "about/about.h"
#include "recovery/recovery.h"
#include "pluginmanagement/pluginmanagement.h"

struct SystemSettingsPrivate {
    SystemSettingsLeftPane* leftPane;
    StatusCenterLeftPane* mainLeftPane;

    QList<QPair<QString, StatusCenterPane*>> loadedPanes;
    QMap<StatusCenterPane*, QListWidgetItem*> paneItems;
    QStringList preferredPaneOrder;

    tSettings settings;
};

SystemSettings::SystemSettings(StatusCenterLeftPane* leftPane) :
    StatusCenterPane(),
    ui(new Ui::SystemSettings) {
    ui->setupUi(this);

    d = new SystemSettingsPrivate();
    d->mainLeftPane = leftPane;
    d->leftPane = new SystemSettingsLeftPane();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);

    d->preferredPaneOrder = d->settings.delimitedList("StatusCenter/settingsOrder");

    connect(StateManager::statusCenterManager(), &StatusCenterManager::paneAdded, this, [ = ](StatusCenterPane * pane, StatusCenterManager::PaneType type) {
        if (type == StatusCenterManager::SystemSettings) {
            this->addPane(pane);
        }
    });
    connect(StateManager::statusCenterManager(), &StatusCenterManager::paneRemoved, this, &SystemSettings::removePane);

    for (StatusCenterPane* pane : StateManager::statusCenterManager()->panes()) {
        if (StateManager::statusCenterManager()->paneType(pane) == StatusCenterManager::SystemSettings) this->addPane(pane);
    }

    connect(d->leftPane, &SystemSettingsLeftPane::indexChanged, this, &SystemSettings::selectPane);
    connect(d->leftPane, &SystemSettingsLeftPane::enterMenu, this, &SystemSettings::enterMenu);

    StateManager::statusCenterManager()->addPane(new About(), StatusCenterManager::SystemSettings);
    StateManager::statusCenterManager()->addPane(new Recovery(), StatusCenterManager::SystemSettings);
    StateManager::statusCenterManager()->addPane(new PluginManagement(), StatusCenterManager::SystemSettings);
}

SystemSettings::~SystemSettings() {
    d->leftPane->deleteLater();
    delete d;
    delete ui;
}

void SystemSettings::selectPane(int index) {
    StatusCenterPane* pane = d->loadedPanes.at(index).second;
    ui->stackedWidget->setCurrentWidget(pane);
}

void SystemSettings::enterMenu(int index) {
    StatusCenterPane* pane = d->loadedPanes.at(index).second;
    if (pane->leftPane()) {
        if (d->mainLeftPane->peekMenu() != pane->leftPane()) d->mainLeftPane->pushMenu(pane->leftPane());
    } else {
        StateManager::statusCenterManager()->hideStatusCenterHamburgerMenu();
    }
}

void SystemSettings::addPane(StatusCenterPane* pane) {
    QListWidgetItem* item = new QListWidgetItem();
    item->setText(pane->displayName());
    item->setIcon(pane->icon());

    connect(pane, &StatusCenterPane::displayNameChanged, this, [ = ] {
        if (d->paneItems.contains(pane)) {
            d->paneItems.value(pane)->setText(pane->displayName());
        }
    });
    connect(pane, &StatusCenterPane::iconChanged, this, [ = ] {
        if (d->paneItems.contains(pane)) {
            d->paneItems.value(pane)->setIcon(pane->icon());
        }
    });
    d->paneItems.insert(pane, item);

    QStringList currentItems;
    for (QPair<QString, StatusCenterPane*> item : d->loadedPanes) {
        currentItems.append(item.first);
    }
    int index = Common::getInsertionIndex(d->preferredPaneOrder, currentItems, pane->name());
    if (index == -1) {
        //Add it at the end
        ui->stackedWidget->addWidget(pane);
        d->leftPane->appendItem(item);
        d->loadedPanes.append({pane->name(), pane});
    } else {
        //Add this chunk at the beginning
        ui->stackedWidget->insertWidget(index, pane);
        d->leftPane->insertItem(index, item);
        d->loadedPanes.insert(index, {pane->name(), pane});
    }
}

void SystemSettings::removePane(StatusCenterPane* pane) {
    for (int i = 0; i < d->loadedPanes.count(); i++) {
        if (d->loadedPanes.at(i).second == pane) {
            QListWidgetItem* item = d->paneItems.take(pane);
            d->leftPane->removeItem(item);
            delete item;

            ui->stackedWidget->removeWidget(pane);
            d->loadedPanes.removeAt(i);
            return;
        }
    }
}

void SystemSettings::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}


QString SystemSettings::name() {
    return "SystemSettings";
}

QString SystemSettings::displayName() {
    return tr("System Settings");
}

QIcon SystemSettings::icon() {
    return QIcon::fromTheme("configure");
}

QWidget* SystemSettings::leftPane() {
    return d->leftPane;
}
