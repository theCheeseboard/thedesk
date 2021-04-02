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

#include <the-libs_global.h>
#include <Wm/desktopwm.h>

struct TaskbarApplicationWidgetPrivate {
    ApplicationPointer app;
    uint desktop;

    QList<DesktopWmWindowPtr> trackedWindows;
};

TaskbarApplicationWidget::TaskbarApplicationWidget(QString desktopEntry, uint desktop, QWidget* parent) : QPushButton(parent) {
    d = new TaskbarApplicationWidgetPrivate();
    d->app = ApplicationPointer(new Application(desktopEntry));
    d->desktop = desktop;

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    connect(DesktopWm::instance(), &DesktopWm::windowRemoved, this, [ = ](DesktopWmWindowPtr window) {
        removeTrackedWindow(window);
    });

    connect(this, &TaskbarApplicationWidget::clicked, this, [ = ] {
        if (!d->trackedWindows.isEmpty()) d->trackedWindows.first()->activate();
    });
    this->setIconSize(SC_DPI_T(QSize(32, 32), QSize));


    updateIcon();
}

TaskbarApplicationWidget::~TaskbarApplicationWidget() {
    delete d;
}

void TaskbarApplicationWidget::trackWindow(DesktopWmWindowPtr window) {
    if (d->trackedWindows.contains(window)) return;

    d->trackedWindows.append(window);
    updateIcon();
}

void TaskbarApplicationWidget::removeTrackedWindow(DesktopWmWindowPtr window) {
    if (!d->trackedWindows.contains(window)) return;

    window->disconnect(this);
    d->trackedWindows.removeOne(window);
    updateIcon();

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
