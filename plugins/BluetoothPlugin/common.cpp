/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "common.h"

Common::Common(QObject* parent) : QObject(parent) {

}

QString Common::stringForDeviceType(BluezQt::Device::Type type) {
    switch (type) {
        case BluezQt::Device::Phone:
            return tr("Phone");
        case BluezQt::Device::Modem:
            return tr("Modem");
        case BluezQt::Device::Computer:
            return tr("Computer");
        case BluezQt::Device::Network:
            return tr("Networking");
        case BluezQt::Device::Headset:
            return tr("Headset");
        case BluezQt::Device::Headphones:
            return tr("Headphones");
        case BluezQt::Device::AudioVideo:
            return tr("Audio/Video");
        case BluezQt::Device::Keyboard:
            return tr("Keyboard");
        case BluezQt::Device::Mouse:
            return tr("Mouse");
        case BluezQt::Device::Joypad:
            return tr("Gamepad");
        case BluezQt::Device::Tablet:
            return tr("Drawing Tablet");
        case BluezQt::Device::Peripheral:
            return tr("Peripheral");
        case BluezQt::Device::Camera:
            return tr("Camera");
        case BluezQt::Device::Printer:
            return tr("Printer");
        case BluezQt::Device::Imaging:
            return tr("Imaging");
        case BluezQt::Device::Wearable:
            return tr("Wearable");
        case BluezQt::Device::Toy:
            return tr("Toy");
        case BluezQt::Device::Health:
            return tr("Health");
        default:
            return tr("Unknown");
    }
}
