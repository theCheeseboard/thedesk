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
#include "managepluginpopover.h"
#include "ui_managepluginpopover.h"

#include <QUuid>
#include <QJsonValue>
#include "plugins/pluginmanager.h"

struct ManagePluginPopoverPrivate {
    QUuid uuid;
};

ManagePluginPopover::ManagePluginPopover(QUuid uuid, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ManagePluginPopover) {
    ui->setupUi(this);

    d = new ManagePluginPopoverPrivate();
    d->uuid = uuid;

    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel->setText(PluginManager::instance()->pluginMetadata(uuid, "name").toString());
    ui->blacklistTitle->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->uuidLabel->setText(tr("UUID: %1").arg(uuid.toString()));

    connect(PluginManager::instance(), &PluginManager::pluginsChanged, this, &ManagePluginPopover::updateState);
    updateState();

    ui->deactivateButton->setProperty("type", "destructive");
    ui->blacklistButton->setProperty("type", "destructive");
    ui->performBlacklistButton->setProperty("type", "destructive");
}

ManagePluginPopover::~ManagePluginPopover() {
    delete d;
    delete ui;
}

void ManagePluginPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void ManagePluginPopover::updateState() {

    PluginManager* manager = PluginManager::instance();
    if (manager->blacklistedPlugins().contains(d->uuid)) {
        ui->activateButton->setVisible(false);
        ui->deactivateButton->setVisible(false);
        ui->blacklistButton->setVisible(false);
        ui->errorDetailsWidget->setVisible(false);
        ui->removeBlacklistButton->setVisible(true);
        return;
    }

    ui->blacklistButton->setVisible(true);
    ui->removeBlacklistButton->setVisible(false);

    if (manager->erroredPlugins().contains(d->uuid)) {
        ui->errorDetailsLabel->setText(manager->pluginErrorReason(d->uuid));
        ui->errorDetailsWidget->setVisible(true);
    } else {
        ui->errorDetailsWidget->setVisible(false);
    }

    if (manager->loadedPlugins().contains(d->uuid)) {
        ui->activateButton->setVisible(false);
        ui->deactivateButton->setVisible(true);
    } else {
        ui->activateButton->setVisible(true);
        ui->deactivateButton->setVisible(false);
    }
}

void ManagePluginPopover::on_activateButton_clicked() {
    PluginManager::instance()->activatePlugin(d->uuid);
}

void ManagePluginPopover::on_deactivateButton_clicked() {
    PluginManager::instance()->deactivatePlugin(d->uuid);
}

void ManagePluginPopover::on_blacklistButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->blacklistPage);
}

void ManagePluginPopover::on_removeBlacklistButton_clicked() {
    PluginManager::instance()->removeBlacklistPlugin(d->uuid);
}

void ManagePluginPopover::on_performBlacklistButton_clicked() {
    PluginManager::instance()->blacklistPlugin(d->uuid);
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

void ManagePluginPopover::on_blacklistTitle_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}
