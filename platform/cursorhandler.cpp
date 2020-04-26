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
#include "cursorhandler.h"

#include <tsettings.h>
#include <QLibrary>

#ifdef HAVE_X11
    #include <QX11Info>
#endif


struct CursorHandlerPrivate {
    tSettings* settings;
};

CursorHandler::CursorHandler(QObject* parent) : QObject(parent) {
    d = new CursorHandlerPrivate();
    d->settings = new tSettings("theSuite", "theDesk.platform", this);

    connect(d->settings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
        if (key == "Platform/cursor") {
            this->updateApplicationCursor();
        }
    });
    this->updateApplicationCursor();
}

CursorHandler::~CursorHandler() {
    delete d;
}

void CursorHandler::updateApplicationCursor() {
#ifdef HAVE_X11
    if (QX11Info::isPlatformX11()) {
        //Load cursor library
        QLibrary xc("/usr/lib/libXcursor");

        if (xc.load()) {
            typedef int (*setThemeFunc) (Display*, const char*);
            typedef int (*setSizeFunc) (Display*, int);

            auto XcursorSetTheme = reinterpret_cast<setThemeFunc>(xc.resolve("XcursorSetTheme"));
            auto XcursorSetDefaultSize = reinterpret_cast<setSizeFunc>(xc.resolve("XcursorSetDefaultSize"));

            //Set cursors
            XcursorSetTheme(QX11Info::display(), qPrintable(d->settings->value("Platform/cursor").toString()));
            XcursorSetDefaultSize(QX11Info::display(), 24);
        }
    }
#endif
}
