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
#include "notificationsdrawer.h"
#include "ui_notificationsdrawer.h"

#include <QScreen>
#include <the-libs_global.h>
#include <Wm/desktopwm.h>
#include <statemanager.h>
#include <barmanager.h>
#include "notificationtracker.h"
#include "notificationsdrawerwidget.h"

struct NotificationsDrawerPrivate {
    NotificationTracker* tracker;
    QList<NotificationsDrawerWidget*> widgets;

    QScreen* oldPrimaryScreen = nullptr;
};

NotificationsDrawer::NotificationsDrawer(NotificationTracker* tracker) :
    QDialog(nullptr),
    ui(new Ui::NotificationsDrawer) {
    ui->setupUi(this);

    d = new NotificationsDrawerPrivate();
    d->tracker = tracker;
    connect(tracker, &NotificationTracker::destroyed, this, &NotificationsDrawer::deleteLater);
    connect(tracker, &NotificationTracker::newNotification, this, &NotificationsDrawer::showNotification);

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_ShowWithoutActivating);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    DesktopWm::setSystemWindow(this);

    connect(qApp, &QApplication::primaryScreenChanged, this, &NotificationsDrawer::updateGeometry);
    updateGeometry();
}

NotificationsDrawer::~NotificationsDrawer() {
    delete ui;
}

void NotificationsDrawer::updateGeometry() {
    if (d->oldPrimaryScreen) {
        disconnect(d->oldPrimaryScreen, &QScreen::geometryChanged, this, &NotificationsDrawer::updateGeometry);
    }

    QScreen* primaryScreen = qApp->primaryScreen();
    connect(primaryScreen, &QScreen::geometryChanged, this, &NotificationsDrawer::updateGeometry);
    d->oldPrimaryScreen = primaryScreen;

    this->setFixedWidth(SC_DPI(500));
    this->setFixedHeight(this->sizeHint().height());
    this->move(primaryScreen->availableGeometry().topLeft());
}

void NotificationsDrawer::showNotification(NotificationPtr notification) {
    NotificationsDrawerWidget* w = new NotificationsDrawerWidget(notification, this);
    d->widgets.append(w);
    ui->notificationsLayout->addWidget(w);

    this->show();

    connect(w, &NotificationsDrawerWidget::dismiss, this, [ = ] {
        d->widgets.removeOne(w);
        ui->notificationsLayout->removeWidget(w);
        w->deleteLater();

        if (d->widgets.count() == 0) {
            this->hide();
        } else {
            this->updateGeometry();
        }
    });

    w->show();
    this->updateGeometry();
}
