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
#include "notificationappgroup.h"
#include "notificationtracker.h"

#include <quietmodemanager.h>
#include <statemanager.h>
#include <statuscentermanager.h>

#include <SystemJob/systemjobcontroller.h>

#include <QPushButton>

struct NotificationsStatusCenterPanePrivate {
        NotificationTracker* tracker;
        SystemJobController* jobController;
        QMap<QString, NotificationAppGroup*> groups;
};

NotificationsStatusCenterPane::NotificationsStatusCenterPane(NotificationTracker* tracker, SystemJobController* jobController) :
    StatusCenterPane(),
    ui(new Ui::NotificationsStatusCenterPane) {
    ui->setupUi(this);

    d = new NotificationsStatusCenterPanePrivate();
    d->tracker = tracker;
    d->jobController = jobController;

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->notificationsWidget->setFixedWidth(contentWidth);
    ui->quietModeWidget->setFixedWidth(contentWidth);

    connect(d->tracker, &NotificationTracker::newNotification, this, [=](NotificationPtr notification) {
        auto application = notification->application();
        QString desktopEntry;
        if (application) {
            desktopEntry = application->desktopEntry();
        }

        auto group = appGroupForDesktopEntry(desktopEntry, application);
        group->pushNotification(notification);

        ui->stackedWidget->setCurrentWidget(ui->notificationsPage);
    });

    connect(d->jobController, &SystemJobController::newJob, this, &NotificationsStatusCenterPane::registerJob);
    for (const auto& job : d->jobController->jobs()) {
        registerJob(job);
    }

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->notificationSplash->setPixmap(QIcon::fromTheme("notifications").pixmap(QSize(128, 128)));

    for (QuietModeManagerTd::QuietMode mode : StateManager::quietModeManager()->availableQuietModes()) {
        QuietModeManagerTd::QuietMode m = mode;
        QPushButton* button = new QPushButton(this);
        button->setText(StateManager::quietModeManager()->name(m));
        button->setIcon(QIcon::fromTheme(StateManager::quietModeManager()->icon(m)));
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setChecked(StateManager::quietModeManager()->currentMode() == m);
        connect(button, &QPushButton::toggled, this, [=](bool checked) {
            if (checked) {
                StateManager::quietModeManager()->setQuietMode(m);
            }
        });
        connect(StateManager::quietModeManager(), &QuietModeManagerTd::quietModeChanged, this, [=](QuietModeManagerTd::QuietMode newMode, QuietModeManagerTd::QuietMode oldMode) {
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

NotificationAppGroup* NotificationsStatusCenterPane::appGroupForDesktopEntry(QString desktopEntry, ApplicationPointer application) {
    if (d->groups.contains(desktopEntry)) {
        return d->groups.value(desktopEntry);
    } else {
        auto group = new NotificationAppGroup(application, this);
        connect(
            group, &NotificationAppGroup::destroyed, this, [=] {
                ui->notificationsLayout->removeWidget(group);
                d->groups.remove(desktopEntry);

                if (d->groups.isEmpty()) ui->stackedWidget->setCurrentWidget(ui->noNotificationsPage);
            },
            Qt::QueuedConnection);
        ui->notificationsLayout->insertWidget(0, group);
        d->groups.insert(desktopEntry, group);

        return group;
    }
}

void NotificationsStatusCenterPane::registerJob(SystemJobPtr job) {
    auto desktopEntry = d->jobController->desktopEntryForService(job->service());
    auto app = ApplicationPointer(new Application(desktopEntry));

    auto group = appGroupForDesktopEntry(desktopEntry, app);
    group->pushJob(job);

    ui->stackedWidget->setCurrentWidget(ui->notificationsPage);
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

void NotificationsStatusCenterPane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}
