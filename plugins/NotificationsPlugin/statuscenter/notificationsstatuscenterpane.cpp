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
#include "notificationsstatuscenterpane.h"
#include "ui_notificationsstatuscenterpane.h"

#include "notification.h"
#include "notificationtracker.h"
#include "notificationappgroup.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <quietmodemanager.h>

#include <QPushButton>

struct NotificationsStatusCenterPanePrivate {
    NotificationTracker* tracker;
    QMap<QString, NotificationAppGroup*> groups;
};

NotificationsStatusCenterPane::NotificationsStatusCenterPane(NotificationTracker* tracker) :
    StatusCenterPane(),
    ui(new Ui::NotificationsStatusCenterPane) {
    ui->setupUi(this);

    d = new NotificationsStatusCenterPanePrivate();
    d->tracker = tracker;

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->notificationsWidget->setFixedWidth(contentWidth);
    ui->quietModeWidget->setFixedWidth(contentWidth);

    connect(d->tracker, &NotificationTracker::newNotification, this, [ = ](NotificationPtr notification) {
        ApplicationPointer application = notification->application();
        QString desktopEntry;
        if (application) {
            desktopEntry = application->desktopEntry();
        }

        NotificationAppGroup* group;
        if (d->groups.contains(desktopEntry)) {
            group = d->groups.value(desktopEntry);
        } else {
            group = new NotificationAppGroup(application, this);
            connect(group, &NotificationAppGroup::destroyed, this, [ = ] {
                ui->notificationsLayout->removeWidget(group);
                d->groups.remove(desktopEntry);

                if (d->groups.isEmpty()) ui->stackedWidget->setCurrentWidget(ui->noNotificationsPage);
            });
            ui->notificationsLayout->insertWidget(0, group);
            d->groups.insert(desktopEntry, group);
        }

        group->pushNotification(notification);

        ui->stackedWidget->setCurrentWidget(ui->notificationsPage);
    });

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->notificationSplash->setPixmap(QIcon::fromTheme("notifications").pixmap(SC_DPI_T(QSize(128, 128), QSize)));

    for (QuietModeManager::QuietMode mode : StateManager::quietModeManager()->availableQuietModes()) {
        QuietModeManager::QuietMode m = mode;
        QPushButton* button = new QPushButton(this);
        button->setText(StateManager::quietModeManager()->name(m));
        button->setIcon(QIcon::fromTheme(StateManager::quietModeManager()->icon(m)));
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setChecked(StateManager::quietModeManager()->currentMode() == m);
        connect(button, &QPushButton::toggled, this, [ = ](bool checked) {
            if (checked) {
                StateManager::quietModeManager()->setQuietMode(m);
            }
        });
        connect(StateManager::quietModeManager(), &QuietModeManager::quietModeChanged, this, [ = ](QuietModeManager::QuietMode newMode, QuietModeManager::QuietMode oldMode) {
            Q_UNUSED(oldMode);
            button->setChecked(newMode == m);
        });
        ui->quietModesLayout->addWidget(button);
    }
}

NotificationsStatusCenterPane::~NotificationsStatusCenterPane() {
    delete d;
    delete ui;
}

void NotificationsStatusCenterPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

QString NotificationsStatusCenterPane::name() {
    return "NotificationsPane";
}

QString NotificationsStatusCenterPane::displayName() {
    return tr("Notifications");
}

QIcon NotificationsStatusCenterPane::icon() {
    return QIcon::fromTheme("thedesk-notifications");
}

QWidget* NotificationsStatusCenterPane::leftPane() {
    return nullptr;
}
