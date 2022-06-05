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
#include "settingsbackend.h"

#ifdef HAVE_X11
    #include <tx11info.h>

    #include <X11/Xlib.h>
#endif

#ifdef HAVE_XI
    #include "xinputbackend.h"
    #include <libinput-properties.h>
#endif

SettingsBackend::SettingsBackend(QObject* parent) :
    QObject(parent) {
}

SettingsBackend* SettingsBackend::backendForPlatform() {
#ifdef HAVE_X11
    if (tX11Info::isPlatformX11()) {
    #ifdef HAVE_XI
        if (XInternAtom(tX11Info::display(), LIBINPUT_PROP_ACCEL, True)) {
            return new XInputBackend();
        }
    #endif
    }
#endif
    return nullptr;
}
