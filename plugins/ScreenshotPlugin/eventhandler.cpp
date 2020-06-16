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
#include <QScreen>
#include <QKeySequence>
#include <keygrab.h>
#include "screenshotwindow.h"

struct EventHandlerPrivate {
    KeyGrab* prtScr;
    KeyGrab* screenshotAlt;
};

EventHandler::EventHandler(QObject* parent) : QObject(parent) {
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

void EventHandler::takeScreenshot() {
    ScreenshotWindow::take(QApplication::screenAt(QCursor::pos()));
}
