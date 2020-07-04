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
#include "eventhandler.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusUnixFileDescriptor>
#include <QDBusPendingCallWatcher>
#include <QDebug>
#include <QKeySequence>

#include <QTimer>

#include <statemanager.h>
#include <powermanager.h>
#include <hudmanager.h>
#include <keygrab.h>

#include <Wm/desktopwm.h>

#include <tsettings.h>

struct EventHandlerPrivate {
    KeyGrab* powerKey;
    QDBusUnixFileDescriptor buttonInhibitor;

    tSettings settings;
    quint64 oldIdleTimer = 0;

    bool screenOffActionPerformed = false;
    bool suspendNotificationShown = false;
    bool suspendActionPerformed = false;

    const static QMap<QString, int> timeoutFactors;
};

const QMap<QString, int> EventHandlerPrivate::timeoutFactors = {
    {"sec", 1000},
    {"min", 60000},
    {"hr", 360000},
    {"never", 0}
};

EventHandler::EventHandler(QObject* parent) : QObject(parent) {
    d = new EventHandlerPrivate();

    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Inhibit");
    message.setArguments(QList<QVariant>() << "handle-power-key:idle" << "theDesk" << "theDesk handles hardware power keys and idling" << "block");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        if (watcher->isError()) {
            qWarning() << tr("Unable to inhibit logind power management");
        } else {
            d->buttonInhibitor = watcher->reply().arguments().first().value<QDBusUnixFileDescriptor>();
        }
    });

    d->powerKey = new KeyGrab(QKeySequence(Qt::Key_PowerOff));
    connect(d->powerKey, &KeyGrab::activated, this, [ = ] {
        StateManager::powerManager()->showPowerOffConfirmation();
    });

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &EventHandler::checkIdleTimer);
    timer->start();
}

EventHandler::~EventHandler() {
    d->powerKey->deleteLater();
    delete d;
}

void EventHandler::checkIdleTimer() {
    quint64 idleTimer = DesktopWm::msecsIdle();
    if (idleTimer < d->oldIdleTimer) {
        if (d->suspendNotificationShown) {
            //Hide the HUD
            StateManager::instance()->hudManager()->hideHud();
        }

        //Reset all variables
        d->screenOffActionPerformed = false;
        d->suspendNotificationShown = false;
        d->suspendActionPerformed = false;
    }
    d->oldIdleTimer = idleTimer;

    if (!d->screenOffActionPerformed) {
        //See if we need to turn the screen off
        quint64 timeout = d->settings.value("Power/timeouts.screenoff.value").toInt() * d->timeoutFactors.value(d->settings.value("Power/timeouts.screenoff.unit").toString(), 0);
        if (idleTimer > timeout && timeout != 0) {
            //Turn the screen off now
            StateManager::powerManager()->performPowerOperation(PowerManager::TurnOffScreen);
            d->screenOffActionPerformed = true;
        }
    }

    if (!d->suspendActionPerformed) {
        //See if we need to suspend
        quint64 notificationTimeout = d->settings.value("Power/timeouts.suspend.value").toInt() * d->timeoutFactors.value(d->settings.value("Power/timeouts.suspend.unit").toString(), 0);
        quint64 timeout = notificationTimeout + 15000; //Give the user 15 seconds notification
        if (idleTimer > notificationTimeout && !d->suspendNotificationShown && notificationTimeout != 0) {
            //Notify the user about the impending suspension
            d->suspendNotificationShown = true;

            StateManager::instance()->hudManager()->showHud({
                {"icon", "system-suspend"},
                {"title", tr("Suspend")},
                {"text", tr("The system has been idle for some time")},
                {"timeout", 15000}
            });
        } else if (idleTimer > timeout && notificationTimeout != 0) {
            //Suspend now
            StateManager::powerManager()->performPowerOperation(PowerManager::Suspend);
            d->suspendActionPerformed = true;
        }
    }
}
