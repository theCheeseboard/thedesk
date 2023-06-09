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
#include "plugin.h"

#include "chunks/mprischunk.h"
#include "dbus/notificationsinterface.h"
#include "drawer/notificationsdrawer.h"
#include "notificationtracker.h"
#include "statuscenter/notificationsstatuscenterpane.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <SystemJob/systemjobcontroller.h>
#include <barmanager.h>
#include <icontextchunk.h>
#include <localemanager.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tsettings.h>

struct PluginPrivate {
        int translationSet;

        NotificationTracker* tracker;
        NotificationsInterface* interface;
        NotificationsDrawer* drawer;
        NotificationsStatusCenterPane* statusCenter;
        SystemJobController* jobController;
        MprisChunk* mprisChunk;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/NotificationsPlugin/translations"),
        "/usr/share/thedesk/NotificationsPlugin/translations"});

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/NotificationsPlugin/thedesk-notifications.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk-notifications.conf");

    d->tracker = new NotificationTracker();
    d->interface = new NotificationsInterface(d->tracker);
    d->drawer = new NotificationsDrawer(d->tracker);
    d->jobController = new SystemJobController(QDBusConnection::sessionBus());
    d->statusCenter = new NotificationsStatusCenterPane(d->tracker, d->jobController);
    StateManager::statusCenterManager()->addPane(d->statusCenter, StatusCenterManager::Informational);

    d->mprisChunk = new MprisChunk();
}

void Plugin::deactivate() {
    StateManager::statusCenterManager()->removePane(d->statusCenter);

    d->tracker->deleteLater();
    d->statusCenter->deleteLater();
    d->mprisChunk->deleteLater();
    d->jobController->deleteLater();

    // Everything else will delete itself once the tracker is gone

    StateManager::localeManager()->removeTranslationSet(d->translationSet);
}
