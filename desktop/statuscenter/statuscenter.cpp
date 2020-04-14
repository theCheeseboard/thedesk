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
#include "statuscenter.h"
#include "ui_statuscenter.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <statuscenterpane.h>
#include <QListWidgetItem>
#include <QPointer>
#include <tpopover.h>
#include "statuscenterleftpane.h"
#include "common/common.h"
#include "systemsettings/systemsettings.h"

#include <quickswitch.h>
#include "statuscenterquickswitch.h"

struct StatusCenterPrivate {
    StatusCenterLeftPane* leftPane;
    bool leftPaneAttached = false;

    QList<QPair<QString, StatusCenterPane*>> loadedPanes;
    QMap<StatusCenterPane*, QListWidgetItem*> paneItems;


    QList<QPair<QuickSwitch*, StatusCenterQuickSwitch*>> switchItems;

    QStringList preferredPaneOrder;
    QStringList preferredSwitchOrder;
};

StatusCenter::StatusCenter(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::StatusCenter) {
    ui->setupUi(this);

    d = new StatusCenterPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);
    d->leftPane = new StatusCenterLeftPane(this);

    if (this->width() <= SC_DPI(1024)) {
        d->leftPaneAttached = true;
        detachLeftPane();
    } else {
        d->leftPaneAttached = false;
        attachLeftPane();
    }

    connect(StateManager::statusCenterManager(), &StatusCenterManager::paneAdded, this, [ = ](StatusCenterPane * pane, StatusCenterManager::PaneType type) {
        if (type == StatusCenterManager::Informational) {
            this->addPane(pane);
        }
    });
    connect(StateManager::statusCenterManager(), &StatusCenterManager::paneRemoved, this, &StatusCenter::removePane);
    connect(StateManager::statusCenterManager(), &StatusCenterManager::showHamburgerMenu, this, &StatusCenter::showHamburgerMenu);
    connect(StateManager::statusCenterManager(), &StatusCenterManager::rootMenu, d->leftPane, &StatusCenterLeftPane::popMenu);
    connect(StateManager::statusCenterManager(), &StatusCenterManager::switchAdded, this, &StatusCenter::addSwitch);
    connect(StateManager::statusCenterManager(), &StatusCenterManager::switchRemoved, this, &StatusCenter::removeSwitch);

    for (StatusCenterPane* pane : StateManager::statusCenterManager()->panes()) {
        if (StateManager::statusCenterManager()->paneType(pane) == StatusCenterManager::Informational) this->addPane(pane);
    }
    for (QuickSwitch* sw : StateManager::statusCenterManager()->switches()) {
        this->addSwitch(sw);
    }

    connect(d->leftPane, &StatusCenterLeftPane::indexChanged, this, &StatusCenter::selectPane);
    connect(d->leftPane, &StatusCenterLeftPane::enterMenu, this, &StatusCenter::enterMenu);

    //Add internal items to the Status Center
    StateManager::instance()->statusCenterManager()->addPane(new SystemSettings(d->leftPane), StatusCenterManager::Informational);
}

StatusCenter::~StatusCenter() {
    delete d;
    delete ui;
}

void StatusCenter::on_closeButton_clicked() {
    StateManager::instance()->statusCenterManager()->hide();
}

void StatusCenter::resizeEvent(QResizeEvent* event) {
    if (this->width() <= SC_DPI(1024)) {
        detachLeftPane();
    } else {
        attachLeftPane();
    }
}

void StatusCenter::attachLeftPane() {
    if (d->leftPaneAttached) return;

    ui->leftPaneContainer->addWidget(d->leftPane);
    d->leftPane->setAttached(true);
    StateManager::statusCenterManager()->setIsHamburgerMenuRequired(false);
    d->leftPaneAttached = true;
}

void StatusCenter::detachLeftPane() {
    if (!d->leftPaneAttached) return;

    ui->leftPaneContainer->removeWidget(d->leftPane);
    d->leftPane->move(-d->leftPane->width(), 0);
    d->leftPane->setAttached(false);
    StateManager::statusCenterManager()->setIsHamburgerMenuRequired(true);
    d->leftPaneAttached = false;
}

void StatusCenter::showHamburgerMenu() {
    if (!tPopover::popoverForWidget(ui->mainWidget)) {
        tPopover* popover = new tPopover(d->leftPane);
        popover->setPopoverWidth(d->leftPane->width());
        popover->setPopoverSide(tPopover::Leading);
        connect(StateManager::statusCenterManager(), &StatusCenterManager::hideHamburgerMenu, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        popover->show(ui->mainWidget);
    }
}

void StatusCenter::selectPane(int index) {
    StatusCenterPane* pane = d->loadedPanes.at(index).second;
    ui->stackedWidget->setCurrentWidget(pane);
}

void StatusCenter::enterMenu(int index) {
    StatusCenterPane* pane = d->loadedPanes.at(index).second;
    if (pane->leftPane()) {
        if (d->leftPane->peekMenu() != pane->leftPane()) d->leftPane->pushMenu(pane->leftPane());
    } else {
        StateManager::statusCenterManager()->hideStatusCenterHamburgerMenu();
    }
}

void StatusCenter::addPane(StatusCenterPane* pane) {
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
        //Add this chunk at the correct index
        ui->stackedWidget->insertWidget(index, pane);
        d->leftPane->insertItem(index, item);
        d->loadedPanes.insert(index, {pane->name(), pane});
    }
}

void StatusCenter::removePane(StatusCenterPane* pane) {
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

void StatusCenter::addSwitch(QuickSwitch* sw) {
    QStringList currentItems;
    for (QPair<QuickSwitch*, StatusCenterQuickSwitch*> item : d->switchItems) {
        currentItems.append(item.first->name());
    }
    StatusCenterQuickSwitch* item = new StatusCenterQuickSwitch(sw);
    int index = Common::getInsertionIndex(d->preferredSwitchOrder, currentItems, sw->name());
    if (index == -1) {
        //Add it at the end
        ui->quickSwitchLayout->addWidget(item);
        d->switchItems.append({sw, item});
    } else {
        //Add this switch at the correct index
        ui->quickSwitchLayout->insertWidget(index, item);
        d->switchItems.insert(index, {sw, item});
    }

}

void StatusCenter::removeSwitch(QuickSwitch* sw) {
    for (int i = 0; i < d->switchItems.count(); i++) {
        if (d->switchItems.at(i).first == sw) {
            ui->quickSwitchLayout->removeWidget(d->switchItems.at(i).second);
            d->switchItems.at(i).second->deleteLater();
            d->switchItems.removeAt(i);
            return;
        }
    }
}
