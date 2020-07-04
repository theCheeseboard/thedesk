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
#include <tvariantanimation.h>

struct EventHandlerPrivate {
    KeyGrab* powerKey;
    KeyGrab* ctrlAltDelKey;
    QDBusUnixFileDescriptor buttonInhibitor;

    tSettings settings;
    quint64 oldIdleTimer = 0;

    bool screenOffActionPerformed = false;
    bool suspendActionPerformed = false;
    tVariantAnimation* suspendNotificationAnimation = new tVariantAnimation();

    const static QMap<QString, int> timeoutFactors;
    const static QMap<QString, int> powerOffActions;
};

const QMap<QString, int> EventHandlerPrivate::timeoutFactors = {
    {"sec", 1000},
    {"min", 60000},
    {"hr", 360000},
    {"never", 0}
};

const QMap<QString, int> EventHandlerPrivate::powerOffActions = {
    {"ask", PowerManager::All},
    {"poweroff", PowerManager::PowerOff},
    {"reboot", PowerManager::Reboot},
    {"suspend", PowerManager::Suspend},
    {"hibernate", PowerManager::Hibernate},
    {"ignore", -1}
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
        int action = d->powerOffActions.value(d->settings.value("Power/actions.powerbutton").toString(), -1);
        if (action == -1) return; //Ignore
        if (action == PowerManager::All) { //Ask
            StateManager::powerManager()->showPowerOffConfirmation();
        } else {
            StateManager::powerManager()->performPowerOperation(static_cast<PowerManager::PowerOperation>(action));
        }
    });
    d->ctrlAltDelKey = new KeyGrab(QKeySequence(Qt::ControlModifier | Qt::AltModifier | Qt::Key_Delete));
    connect(d->ctrlAltDelKey, &KeyGrab::activated, this, [ = ] {
        StateManager::powerManager()->showPowerOffConfirmation();
    });

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &EventHandler::checkIdleTimer);
    timer->start();

    d->suspendNotificationAnimation = new tVariantAnimation(this);
    d->suspendNotificationAnimation->setStartValue(1.0);
    d->suspendNotificationAnimation->setEndValue(0.0);
    d->suspendNotificationAnimation->setForceAnimation(true);
    d->suspendNotificationAnimation->setDuration(15000);
    connect(d->suspendNotificationAnimation, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        StateManager::instance()->hudManager()->showHud({
            {"icon", "system-suspend"},
            {"title", tr("Suspend")},
            {"text", tr("%n seconds", nullptr, (15000 - d->suspendNotificationAnimation->currentTime()) / 1000 + 1)},
            {"value", value.toDouble()},
            {"timeout", 15000}
        });

        //Immediately do an idle timer check if the idle timer has changed
        if (DesktopWm::msecsIdle() < d->oldIdleTimer) checkIdleTimer();
    });
    connect(d->suspendNotificationAnimation, &tVariantAnimation::stateChanged, this, [ = ](tVariantAnimation::State newState, tVariantAnimation::State oldState) {
        if (newState == tVariantAnimation::Stopped) {
            StateManager::instance()->hudManager()->hideHud();
        }
    });
    connect(d->suspendNotificationAnimation, &tVariantAnimation::finished, this, [ = ] {
        //Suspend now
        StateManager::powerManager()->performPowerOperation(PowerManager::Suspend);
    });
}

EventHandler::~EventHandler() {
    d->powerKey->deleteLater();
    d->ctrlAltDelKey->deleteLater();
    delete d;
}

void EventHandler::checkIdleTimer() {
    quint64 idleTimer = DesktopWm::msecsIdle();
    if (idleTimer < d->oldIdleTimer) {
        if (d->suspendNotificationAnimation->state() == tVariantAnimation::Running) {
            d->suspendNotificationAnimation->stop();
        }

        //Reset all variables
        d->screenOffActionPerformed = false;
        d->suspendActionPerformed = false;
    }
    d->oldIdleTimer = idleTimer;

    //Ensure that there are no full screen windows so we don't switch off the screen if someone is watching a video for instance
    for (DesktopWmWindowPtr window : DesktopWm::openWindows()) {
        if (window->isFullScreen()) return;
    }

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
        quint64 timeout = d->settings.value("Power/timeouts.suspend.value").toInt() * d->timeoutFactors.value(d->settings.value("Power/timeouts.suspend.unit").toString(), 0);
        if (idleTimer > timeout && timeout != 0) {
            //Notify the user about the impending suspension and then suspend
            d->suspendActionPerformed = true;
            d->suspendNotificationAnimation->start();
        }
    }
}
