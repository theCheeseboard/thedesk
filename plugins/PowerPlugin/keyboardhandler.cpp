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
#include "keyboardhandler.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusUnixFileDescriptor>
#include <QDBusPendingCallWatcher>
#include <QDebug>
#include <QKeySequence>

#include <statemanager.h>
#include <powermanager.h>
#include <keygrab.h>

struct KeyboardHandlerPrivate {
    KeyGrab* powerKey;
    QDBusUnixFileDescriptor inhibitorFileDescriptor;
};

KeyboardHandler::KeyboardHandler(QObject* parent) : QObject(parent) {
    d = new KeyboardHandlerPrivate();

    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Inhibit");
    message.setArguments(QList<QVariant>() << "handle-power-key" << "theDesk" << "theDesk Handles Hardware Power Keys" << "block");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        if (watcher->isError()) {
            qWarning() << tr("Unable to inhibit logind power management");
        } else {
            d->inhibitorFileDescriptor = watcher->reply().arguments().first().value<QDBusUnixFileDescriptor>();
        }
    });

    d->powerKey = new KeyGrab(QKeySequence(Qt::Key_PowerOff));
    connect(d->powerKey, &KeyGrab::activated, this, [ = ] {
        StateManager::powerManager()->showPowerOffConfirmation();
    });
}

KeyboardHandler::~KeyboardHandler() {
    d->powerKey->deleteLater();
    delete d;
}
