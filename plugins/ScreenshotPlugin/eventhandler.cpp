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

#include <QApplication>
#include <QCoroDBusPendingCall>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusPendingCall>
#include <QKeySequence>
#include <QScreen>
#include <keygrab.h>
#include <tlogger.h>
#include <tnotification.h>

struct EventHandlerPrivate {
        KeyGrab* prtScr;
        KeyGrab* screenshotAlt;
};

EventHandler::EventHandler(QObject* parent) :
    QObject(parent) {
    d = new EventHandlerPrivate();
    d->prtScr = new KeyGrab(QKeySequence(Qt::Key_Print), "screenshot");
    d->screenshotAlt = new KeyGrab(QKeySequence(Qt::MetaModifier | Qt::AltModifier | Qt::Key_P), "screenshotAlt");

    connect(d->prtScr, &KeyGrab::activated, this, &EventHandler::takeScreenshot);
    connect(d->screenshotAlt, &KeyGrab::activated, this, &EventHandler::takeScreenshot);
}

EventHandler::~EventHandler() {
    d->prtScr->deleteLater();
    d->screenshotAlt->deleteLater();
    delete d;
}

QCoro::Task<> EventHandler::takeScreenshot() {
    // Delegate screenshot taking to the portal
    auto message = QDBusMessage::createMethodCall("org.freedesktop.impl.portal.desktop.thedesk", "/org/freedesktop/portal/desktop", "org.freedesktop.impl.portal.Screenshot", "Screenshot");
    message.setArguments({
        QDBusObjectPath("/"), "com.vicr123.thedesk", "", QVariantMap{{"interactive", true}, {"x-thedesk-screenshot", true}}
    });
    auto reply = co_await QDBusConnection::sessionBus().asyncCall(message, 300000);
    if (reply.type() != QDBusMessage::ReplyMessage) {
        tWarn("EventHandler") << "Unable to take screenshot";
        tWarn("EventHandler") << message.errorMessage();

        // Send a notification
        auto notification = new tNotification();
        notification->setSummary(tr("Unable to take screenshot"));
        notification->setText(tr("Sorry, the screenshot was unable to be taken."));
        notification->post();
    }
}
