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
#include <gatewaymanager.h>
#include <quietmodemanager.h>
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
    DesktopWm::setSystemWindow(this, DesktopWm::SystemWindowTypeNotification);

    connect(StateManager::barManager(), &BarManager::barHeightChanged, this, &NotificationsDrawer::updateGeometry);
    connect(StateManager::gatewayManager(), &GatewayManager::gatewayWidthChanged, this, &NotificationsDrawer::updateGeometry);
    connect(ui->hudWidget, &HudWidget::shouldShowChanged, this, &NotificationsDrawer::updateGeometry);

    connect(qApp, &QApplication::primaryScreenChanged, this, &NotificationsDrawer::updateGeometry);
    updateGeometry();
}

NotificationsDrawer::~NotificationsDrawer() {
    delete ui;
}

bool NotificationsDrawer::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::LayoutRequest) {
        this->updateGeometry();
    }
    return false;
}

void NotificationsDrawer::updateGeometry() {
    QScreen* primaryScreen = qApp->primaryScreen();
    if (d->oldPrimaryScreen != primaryScreen && d->oldPrimaryScreen) {
        disconnect(d->oldPrimaryScreen, &QScreen::geometryChanged, this, &NotificationsDrawer::updateGeometry);
    }

    if (!d->oldPrimaryScreen) {
        connect(primaryScreen, &QScreen::geometryChanged, this, &NotificationsDrawer::updateGeometry);
    }
    d->oldPrimaryScreen = primaryScreen;

    this->setFixedWidth(SC_DPI(400));
    this->setFixedHeight(this->sizeHint().height());

    QRect geometry;
    geometry.setTopLeft(primaryScreen->geometry().topLeft() + QPoint(StateManager::gatewayManager()->gatewayWidth(), StateManager::barManager()->barHeight()));
    geometry.setSize(this->size());

    if (geometry.bottom() > primaryScreen->geometry().bottom()) geometry.moveBottom(primaryScreen->geometry().bottom());

    this->move(geometry.topLeft());

    if (d->widgets.count() == 0 && !ui->hudWidget->shouldShow()) {
        this->hide();
    } else {
        this->show();
    }
}

void NotificationsDrawer::showNotification(NotificationPtr notification) {
    switch (StateManager::quietModeManager()->currentMode()) {
        case QuietModeManager::CriticalOnly:
            if (notification->urgency() != Notification::Critical) return;
            break;
        case QuietModeManager::NoNotifications:
        case QuietModeManager::Mute:
            return;
        default:
            break;
    }

    NotificationsDrawerWidget* w = new NotificationsDrawerWidget(notification, d->tracker, this);
    w->installEventFilter(this);
    d->widgets.append(w);
    ui->notificationsLayout->addWidget(w);

    this->updateGeometry();

    connect(w, &NotificationsDrawerWidget::dismiss, this, [ = ] {
        d->widgets.removeOne(w);
        ui->notificationsLayout->removeWidget(w);
        w->deleteLater();

        this->updateGeometry();
    });

    w->show();
    this->updateGeometry();
}
