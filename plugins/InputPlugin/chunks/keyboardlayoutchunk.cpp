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
#include "keyboardlayoutchunk.h"
#include "ui_keyboardlayoutchunk.h"

#include <Wm/desktopwm.h>
#include <tsettings.h>
#include <QMenu>
#include <statemanager.h>
#include <barmanager.h>

struct KeyboardLayoutChunkPrivate {
    tSettings settings;
    QMenu* layoutSelectMenu;

    QList<QAction*> layoutsActions;
    BarManager::BarLockPtr barLocker;
};

KeyboardLayoutChunk::KeyboardLayoutChunk() :
    Chunk(),
    ui(new Ui::KeyboardLayoutChunk) {
    ui->setupUi(this);
    d = new KeyboardLayoutChunkPrivate();
    d->layoutSelectMenu = new QMenu();

    ui->layoutSelectButton->setIconSize(SC_DPI_T(QSize(16, 16), QSize));

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key) {
        if (key == "Input/keyboard.layouts") updateLayouts();
    });
    updateLayouts();

    connect(DesktopWm::instance(), &DesktopWm::currentKeyboardLayoutChanged, this, &KeyboardLayoutChunk::updateCurrentLayout);
    updateCurrentLayout();

    connect(d->layoutSelectMenu, &QMenu::aboutToShow, this, [ = ] {
        d->barLocker = StateManager::barManager()->acquireLock();
    });
    connect(d->layoutSelectMenu, &QMenu::aboutToHide, this, [ = ] {
        d->barLocker->unlock();
    });
    ui->layoutSelectButton->setMenu(d->layoutSelectMenu);
}

KeyboardLayoutChunk::~KeyboardLayoutChunk() {
    delete ui;
    delete d;
}

void KeyboardLayoutChunk::updateLayouts() {
    for (QAction* action : d->layoutsActions) {
        d->layoutSelectMenu->removeAction(action);
        action->deleteLater();
    }
    d->layoutsActions.clear();

    d->layoutsActions.append(d->layoutSelectMenu->addSection(tr("Select Keyboard Layout")));
    for (QString layout : d->settings.delimitedList("Input/keyboard.layouts")) {
        QAction* action = new QAction(this);

        action->setText(DesktopWm::keyboardLayoutDescription(layout));
        action->setCheckable(true);

        connect(DesktopWm::instance(), &DesktopWm::currentKeyboardLayoutChanged, action, [ = ] {
            action->setChecked(layout == DesktopWm::currentKeyboardLayout());
        });
        action->setChecked(layout == DesktopWm::currentKeyboardLayout());

        connect(action, &QAction::triggered, this, [ = ] {
            DesktopWm::setCurrentKeyboardLayout(layout);
        });

        d->layoutSelectMenu->addAction(action);
        d->layoutsActions.append(action);
    }
}

void KeyboardLayoutChunk::updateCurrentLayout() {
    ui->layoutSelectButton->setText(DesktopWm::currentKeyboardLayout());
}

QString KeyboardLayoutChunk::name() {
    return "keyboardlayout";
}

int KeyboardLayoutChunk::expandedHeight() {
    return this->sizeHint().height();
}

int KeyboardLayoutChunk::statusBarHeight() {
    return SC_DPI(16);
}
