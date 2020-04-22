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
#include "pluginmanagement.h"
#include "ui_pluginmanagement.h"

#include <QIcon>
#include <statemanager.h>
#include <statuscentermanager.h>
#include "plugins/pluginmanager.h"
#include "pluginmodel.h"
#include "pluginitemdelegate.h"
#include "managepluginpopover.h"
#include <tpopover.h>
#include <QUuid>

struct PluginManagementPrivate {
    PluginModel* pluginModel;
};

PluginManagement::PluginManagement() :
    StatusCenterPane(),
    ui(new Ui::PluginManagement) {
    ui->setupUi(this);
    d = new PluginManagementPrivate();

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->descriptionLabel->setFixedWidth(contentWidth);

    d->pluginModel = new PluginModel();
    ui->pluginsList->setModel(d->pluginModel);
    ui->pluginsList->setItemDelegate(new PluginItemDelegate(this));
}

PluginManagement::~PluginManagement() {
    delete d;
    delete ui;
}

void PluginManagement::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}

QString PluginManagement::name() {
    return "SystemPluginManagement";
}

QString PluginManagement::displayName() {
    return tr("Plugins");
}

QIcon PluginManagement::icon() {
    return QIcon::fromTheme("preferences-system-plugins");
}

QWidget* PluginManagement::leftPane() {
    return nullptr;
}

void PluginManagement::on_pluginsList_activated(const QModelIndex& index) {
    ManagePluginPopover* manager = new ManagePluginPopover(index.data(Qt::UserRole + 1).toUuid());
    tPopover* popover = new tPopover(manager);
    popover->setPopoverWidth(SC_DPI(600));
    connect(manager, &ManagePluginPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
//    connect(popover, &tPopover::dismissed, manager, &ManagePluginPopover::deleteLater);
    popover->show(this->window());
}

void PluginManagement::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}
