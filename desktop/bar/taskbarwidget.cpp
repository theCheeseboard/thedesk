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
#include <QTimer>
#include <Wm/desktopwm.h>
#include <the-libs_global.h>
#include <Applications/application.h>
#include "taskbardesktopwidget.h"

struct TaskbarWidgetPrivate {
    QList<TaskbarDesktopWidget*> desktopWidgets;
};

TaskbarWidget::TaskbarWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::TaskbarWidget) {
    ui->setupUi(this);

    d = new TaskbarWidgetPrivate();

//    connect(DesktopWm::instance(), &DesktopWm::windowAdded, this, &TaskbarWidget::addWindow);
//    connect(DesktopWm::instance(), &DesktopWm::windowRemoved, this, &TaskbarWidget::removeWindow);
//    connect(DesktopWm::instance(), &DesktopWm::activeWindowChanged, this, &TaskbarWidget::activeWindowChanged);

//    for (DesktopWmWindowPtr window : DesktopWm::openWindows()) {
//        this->addWindow(window);
//    }

    connect(DesktopWm::instance(), &DesktopWm::currentDesktopChanged, this, &TaskbarWidget::updateDesktop);
    connect(DesktopWm::instance(), &DesktopWm::desktopCountChanged, this, &TaskbarWidget::updateDesktop);

    connect(DesktopWm::instance(), &DesktopWm::currentDesktopChanged, this, &TaskbarWidget::normaliseDesktops);
    connect(DesktopWm::instance(), &DesktopWm::desktopCountChanged, this, &TaskbarWidget::normaliseDesktops);
    connect(DesktopWm::instance(), &DesktopWm::windowAdded, this, &TaskbarWidget::addWindow);
    connect(DesktopWm::instance(), &DesktopWm::windowRemoved, this, &TaskbarWidget::removeWindow);

    normaliseDesktops();

    updateDesktop();
}

TaskbarWidget::~TaskbarWidget() {
    delete d;
    delete ui;
    d = nullptr;
}

void TaskbarWidget::addWindow(DesktopWmWindowPtr window) {
    connect(window, &DesktopWmWindow::desktopChanged, this, &TaskbarWidget::normaliseDesktops);

    //Race condition?
    QTimer::singleShot(500, this, [ = ] {
        normaliseDesktops();
    });
}

void TaskbarWidget::removeWindow(DesktopWmWindowPtr window) {
    //Race condition?
    QTimer::singleShot(500, this, [ = ] {
        normaliseDesktops();
    });
}

void TaskbarWidget::activeWindowChanged() {

}

void TaskbarWidget::normaliseDesktops() {
    //Ensure there is a taskbar desktop widget for every desktop - 1
    for (int i = 0; i < DesktopWm::desktops().count() - 1; i++) {
        if (d->desktopWidgets.count() == i) {
            //Add another widget
            TaskbarDesktopWidget* widget = new TaskbarDesktopWidget(i);
            ui->desktopsLayout->addWidget(widget);
            d->desktopWidgets.append(widget);
        }
    }

    //Remove any extraenous desktop widgets
    while (d->desktopWidgets.count() > DesktopWm::desktops().count() - 1) {
        TaskbarDesktopWidget* widget = d->desktopWidgets.takeLast();
        ui->desktopsLayout->removeWidget(widget);
        widget->deleteLater();
    }

    //Go through each desktop to find any that need to be removed
    for (int i = 0; i < DesktopWm::desktops().count() - 1; i++) {
        if (DesktopWm::currentDesktop() == static_cast<uint>(i)) continue;
        if (!DesktopWm::windowsOnDesktop(i).isEmpty()) continue;

        //This desktop needs to be removed
        //Shuffle all of the windows from the desktops in front down
        for (int j = i + 1; j < DesktopWm::desktops().count() - 1; j++) {
            d->desktopWidgets.at(j)->moveDesktop(j - 1);
        }

        //Remove the last desktop
        DesktopWm::setNumDesktops(DesktopWm::desktops().count() - 1);

        //Normalise and check again
        normaliseDesktops();
        return;
    }

    //Ensure that there is an empty desktop at the end
    if (!DesktopWm::windowsOnDesktop(DesktopWm::desktops().count() - 1).isEmpty()) {
        //Add an extra desktop
        DesktopWm::setNumDesktops(DesktopWm::desktops().count() + 1);

        //Normalise and check again
        normaliseDesktops();
        return;
    }
}

void TaskbarWidget::updateDesktop() {
    ui->lastDesktopButton->setChecked(DesktopWm::currentDesktop() == DesktopWm::desktops().count() - 1);
}

void TaskbarWidget::on_lastDesktopButton_clicked() {
    DesktopWm::setCurrentDesktop(DesktopWm::desktops().count() - 1);
}

void TaskbarWidget::resizeEvent(QResizeEvent* event) {
    ui->lastDesktopButton->setFixedWidth(this->height());
}
