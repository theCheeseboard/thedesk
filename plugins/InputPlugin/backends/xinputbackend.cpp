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
#include "xinputbackend.h"

#include <QVariant>
#include <QX11Info>
#include <libinput-properties.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XInput.h>

#undef Bool

XInputBackend::XInputBackend(QObject* parent) : SettingsBackend(parent) {

}

void XInputBackend::writeXiSetting(const char* atom, QVariantList value, WriteFor writeFor) {
    int devices;
    XDeviceInfo* info = XListInputDevices(QX11Info::display(), &devices);
    if (info == nullptr) return;

    Atom mouseAtom = XInternAtom(QX11Info::display(), XI_MOUSE, True);
    Atom touchpadAtom = XInternAtom(QX11Info::display(), XI_TOUCHPAD, True);
    Atom trackballAtom = XInternAtom(QX11Info::display(), XI_TRACKBALL, True);
    for (int i = 0; i < devices; i++) {
        XDeviceInfo* d = info + i;
        if ((d->type == mouseAtom && writeFor & Mice)
            || (d->type == touchpadAtom && writeFor & Touchpads)
            || (d->type == trackballAtom && writeFor & Mice)) {
            Atom valAtom = XInternAtom(QX11Info::display(), atom, True);
            Atom type;
            if (value.first().type() == QVariant::Bool || value.first().type() == QVariant::Int || value.first().type() == QVariant::Char) {
                type = XA_INTEGER;
            } else if (value.first().type() == QVariant::Double) {
                type = XInternAtom(QX11Info::display(), "FLOAT", False);
            } else {
                return;
            }

            Atom typeReturn;
            int formatReturn;
            unsigned long itemCount;
            unsigned long bytesAfter;
            unsigned char* data = nullptr;
            Status s = XIGetProperty(QX11Info::display(), d->id, valAtom, 0, 32, False, type, &typeReturn, &formatReturn, &itemCount, &bytesAfter, &data);
            if (s != Success) continue;

            if (typeReturn != type || data == nullptr || itemCount != value.count()) {
                if (data != nullptr) {
                    XFree(data);
                }
                continue;
            }

            if (value.first().type() == QVariant::Bool && formatReturn != 8) {
                XFree(data);
                continue;
            } else if (value.first().type() == QVariant::Double && formatReturn != 32) {
                XFree(data);
                continue;
            } else if (value.first().type() == QVariant::Char && formatReturn != 8) {
                XFree(data);
                continue;
            } else if (value.first().type() == QVariant::Int && formatReturn != 32) {
                XFree(data);
                continue;
            }

            if (value.first().type() == QVariant::Bool) {
                unsigned char v[64];
                for (int i = 0; i < value.count(); i++) {
                    v[i] = value.at(i).toBool() ? 1 : 0;
                }
                XIChangeProperty(QX11Info::display(), d->id, valAtom, type, 8, XIPropModeReplace, v, value.count());
            } else if (value.first().type() == QVariant::Double) {
                float v[64];
                for (int i = 0; i < value.count(); i++) {
                    v[i] = static_cast<float>(value.at(i).toDouble());
                }
                XIChangeProperty(QX11Info::display(), d->id, valAtom, type, 32, XIPropModeReplace, reinterpret_cast<unsigned char*>(v), value.count());
            } else if (value.first().type() == QVariant::Char) {
                unsigned char v[64];
                for (int i = 0; i < value.count(); i++) {
                    v[i] = value.at(i).toChar().toLatin1();
                }
                XIChangeProperty(QX11Info::display(), d->id, valAtom, type, 8, XIPropModeReplace, v, value.count());
            } else if (value.first().type() == QVariant::Int) {
                int v[64];
                for (int i = 0; i < value.count(); i++) {
                    v[i] = value.at(i).toInt();
                }
                XIChangeProperty(QX11Info::display(), d->id, valAtom, type, 32, XIPropModeReplace, reinterpret_cast<unsigned char*>(v), value.count());
            }

            XFree(data);
        }
    }
    XFreeDeviceList(info);
}

void XInputBackend::setPrimaryMouseButton(MousePrimaryButton button) {
    writeXiSetting(LIBINPUT_PROP_LEFT_HANDED, {button == RightMouseButton}, Mice);
}

void XInputBackend::setPrimaryTouchpadButton(MousePrimaryButton button) {
    writeXiSetting(LIBINPUT_PROP_LEFT_HANDED, {button == RightMouseButton}, Touchpads);
}

void XInputBackend::setTapToClick(bool tapToClick) {
    writeXiSetting(LIBINPUT_PROP_TAP, {tapToClick}, Touchpads);
}

void XInputBackend::setNaturalScrolling(bool naturalScrolling) {
    writeXiSetting(LIBINPUT_PROP_NATURAL_SCROLL, {naturalScrolling}, Touchpads);
}
