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
#include "taskbarwidget.h"
#include "ui_taskbarwidget.h"

#include <QPushButton>
#include <Wm/desktopwm.h>
#include <the-libs_global.h>

struct TaskbarWidgetPrivate {
    QMap<DesktopWmWindowPtr, QPushButton*> buttons;
};

TaskbarWidget::TaskbarWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::TaskbarWidget) {
    ui->setupUi(this);

    d = new TaskbarWidgetPrivate();

    connect(DesktopWm::instance(), &DesktopWm::windowAdded, this, &TaskbarWidget::addWindow);
    connect(DesktopWm::instance(), &DesktopWm::windowRemoved, this, &TaskbarWidget::removeWindow);
    connect(DesktopWm::instance(), &DesktopWm::activeWindowChanged, this, &TaskbarWidget::activeWindowChanged);

    for (DesktopWmWindowPtr window : DesktopWm::openWindows()) {
        this->addWindow(window);
    }
}

TaskbarWidget::~TaskbarWidget() {
    delete d;
    delete ui;
    d = nullptr;
}

void TaskbarWidget::addWindow(DesktopWmWindowPtr window) {
    if (!window->shouldShowInTaskbar()) return;

    QPushButton* button = new QPushButton(this);
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    button->setText(window->title());
    button->setIcon(window->icon());
    button->setIconSize(SC_DPI_T(QSize(32, 32), QSize));
    button->setCheckable(true);
    button->setAutoExclusive(true);
    connect(button, &QPushButton::clicked, window, [ = ] {
        window->activate();
    });
    connect(button, &QPushButton::destroyed, this, [ = ] {
        if (d) d->buttons.remove(window);
    });

    auto updateWindow = [ = ] {
        ApplicationPointer app = window->application();
        if (app) {
            button->setText(app->getProperty("Name").toString());
            button->setIcon(QIcon::fromTheme(app->getProperty("Icon").toString()));
        } else {
            button->setText(window->title());
            button->setIcon(window->icon());
        }
    };
    updateWindow();

    connect(window, &DesktopWmWindow::titleChanged, button, updateWindow);
    connect(window, &DesktopWmWindow::iconChanged, button, updateWindow);
    connect(window, &DesktopWmWindow::applicationChanged, button, updateWindow);

    ui->buttonsLayout->addWidget(button);
    d->buttons.insert(window, button);
}

void TaskbarWidget::removeWindow(DesktopWmWindowPtr window) {
    if (d->buttons.contains(window)) {
        ui->buttonsLayout->removeWidget(d->buttons.value(window));
        d->buttons.value(window)->deleteLater();
    }
}

void TaskbarWidget::activeWindowChanged() {
    for (QPushButton* button : d->buttons.values()) {
        button->setChecked(false);
    }

    QPushButton* selectedButton = d->buttons.value(DesktopWm::activeWindow(), nullptr);
    if (selectedButton) selectedButton->setChecked(true);
}
