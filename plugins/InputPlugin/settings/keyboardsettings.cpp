/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#include "keyboardsettings.h"
#include "ui_keyboardsettings.h"

#include <tsettings.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <localemanager.h>
#include <QMenu>

#include "models/selectedkeyboardlayoutsmodel.h"

struct KeyboardSettingsPrivate {
    SelectedKeyboardLayoutsModel* layoutsModel;
};

KeyboardSettings::KeyboardSettings(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::KeyboardSettings) {
    ui->setupUi(this);

    d = new KeyboardSettingsPrivate();

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->layoutsWidget->setFixedWidth(contentWidth);

    d->layoutsModel = new SelectedKeyboardLayoutsModel();
    ui->layoutsList->setModel(d->layoutsModel);
}

KeyboardSettings::~KeyboardSettings() {
    delete d;
    delete ui;
}

void KeyboardSettings::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

void KeyboardSettings::on_addLayoutButton_clicked() {
    bool ok;
    QString newLayout = StateManager::localeManager()->showKeyboardLayoutSelector(this->window(), &ok);
    if (!ok) return;

    d->layoutsModel->addLayout(newLayout);
}


void KeyboardSettings::on_layoutsList_customContextMenuRequested(const QPoint& pos) {
    if (d->layoutsModel->rowCount() == 1) return;

    QModelIndex index = ui->layoutsList->indexAt(pos);
    if (index.isValid()) {
        QMenu* menu = new QMenu();
        menu->addSection(tr("For this layout"));
        if (index.row() != 0) menu->addAction(QIcon::fromTheme("go-up"), tr("Move Up"), [ = ] {
            d->layoutsModel->moveUp(index.data(Qt::UserRole).toString());
        });
        if (index.row() != d->layoutsModel->rowCount() - 1) menu->addAction(QIcon::fromTheme("go-down"), tr("Move Down"), [ = ] {
            d->layoutsModel->moveDown(index.data(Qt::UserRole).toString());
        });
        menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove"), [ = ] {
            d->layoutsModel->removeLayout(index.data(Qt::UserRole).toString());
        });
        connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
        menu->popup(ui->layoutsList->mapToGlobal(pos));
    }
}

