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
#include "taskbarapplicationwidget.h"

#include <Wm/desktopwm.h>
#include <libcontemporary_global.h>

#include <QAction>
#include <actionquickwidget.h>
#include <private/quickwidgetcontainer.h>

struct TaskbarApplicationWidgetPrivate {
        ApplicationPointer app;
        uint desktop;

        ActionQuickWidget* windowListMenu;
        QuickWidgetContainer* windowListMenuContainer;

        QList<DesktopWmWindowPtr> trackedWindows;
        QMap<DesktopWmWindowPtr, QAction*> actions;
};

TaskbarApplicationWidget::TaskbarApplicationWidget(QString desktopEntry, uint desktop, QWidget* parent) :
    QPushButton(parent) {
    d = new TaskbarApplicationWidgetPrivate();
    d->app = ApplicationPointer(new Application(desktopEntry));
    d->desktop = desktop;

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    this->setCheckable(true);

    connect(DesktopWm::instance(), &DesktopWm::windowRemoved, this, [=](DesktopWmWindowPtr window) {
        removeTrackedWindow(window);
    });
    connect(DesktopWm::instance(), &DesktopWm::activeWindowChanged, this, &TaskbarApplicationWidget::updateActive);

    connect(this, &TaskbarApplicationWidget::clicked, this, [=] {
        if (d->trackedWindows.count() == 1) {
            d->trackedWindows.first()->activate();
        } else {
            if (!d->windowListMenuContainer->isShowing()) d->windowListMenuContainer->showContainer();
        }
        updateActive();
    });
    this->setIconSize(SC_DPI_T(QSize(32, 32), QSize));

    d->windowListMenuContainer = new QuickWidgetContainer(this);
    d->windowListMenu = new ActionQuickWidget(d->windowListMenuContainer);
    d->windowListMenuContainer->setQuickWidget(d->windowListMenu);
    //    connect(d->windowListMenu, &ActionQuickWidget::done, d->menuContainer, &QuickWidgetContainer::hideContainer);

    updateIcon();
}

TaskbarApplicationWidget::~TaskbarApplicationWidget() {
    delete d;
}

void TaskbarApplicationWidget::trackWindow(DesktopWmWindowPtr window) {
    if (d->trackedWindows.contains(window)) return;

    QAction* action = new QAction(this);
    action->setText(window->title());

    connect(window, &DesktopWmWindow::titleChanged, action, [=] {
        action->setText(window->title());
    });
    connect(action, &QAction::triggered, window, [=] {
        window->activate();
    });

    d->actions.insert(window, action);
    d->windowListMenu->addAction(action);
    d->trackedWindows.append(window);
    updateIcon();
    updateActive();
}

void TaskbarApplicationWidget::removeTrackedWindow(DesktopWmWindowPtr window) {
    if (!d->trackedWindows.contains(window)) return;

    window->disconnect(this);
    d->trackedWindows.removeOne(window);
    QAction* action = d->actions.take(window);
    d->windowListMenu->removeAction(action);
    action->deleteLater();
    updateIcon();
    updateActive();

    if (d->trackedWindows.isEmpty()) emit windowsRemoved();
}

QList<DesktopWmWindowPtr> TaskbarApplicationWidget::trackedWindows() {
    return d->trackedWindows;
}

void TaskbarApplicationWidget::setColor(QColor color) {
    QPalette pal = this->palette();
    pal.setColor(QPalette::Button, color);
    this->setPalette(pal);
}

void TaskbarApplicationWidget::resizeEvent(QResizeEvent* event) {
    this->setFixedWidth(this->height());
}

void TaskbarApplicationWidget::updateIcon() {
    if (d->app->isValid()) {
        this->setIcon(d->app->icon());
    } else if (!d->trackedWindows.isEmpty()) {
        this->setIcon(d->trackedWindows.first()->icon());
    } else {
        this->setIcon(QIcon::fromTheme("generic-app"));
    }

    emit iconChanged();
}

void TaskbarApplicationWidget::updateActive() {
    this->setChecked(d->trackedWindows.contains(DesktopWm::activeWindow()));
}
