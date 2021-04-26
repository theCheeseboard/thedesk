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
#include "common.h"

#include <Port>
#include <QVariantMap>

Common::DevicePort Common::portForSink(PulseAudioQt::Sink* sink) {
    QVariantMap properties = sink->properties();
    QString api = properties.value("device.api").toString();
    if (api == QStringLiteral("bluez")) {
        return Bluetooth;
    } else {
        if (sink->ports().count() <= sink->activePortIndex()) return Unknown;

        PulseAudioQt::Port* port = sink->ports().at(sink->activePortIndex());
        if (port->availability() == PulseAudioQt::Port::Unavailable) {
            //Weird thing? Use a workaround here
            QList<PulseAudioQt::Port*> availablePorts;
            for (PulseAudioQt::Port* port : sink->ports()) {
                if (port->availability() != PulseAudioQt::Port::Unavailable) availablePorts.append(port);
            }

            if (availablePorts.count() == 1) {
                port = availablePorts.first();
            } else {
                port = nullptr;
            }
        }


        if (port != nullptr) {
            QString newPort;
            if (port->name().contains("headphones", Qt::CaseInsensitive)) {
                return Headphones;
            } else if (port->name().contains("speaker", Qt::CaseInsensitive)) {
                return Speakers;
            }   else if (port->name().contains("lineout", Qt::CaseInsensitive)) {
                return LineOut;
            }
        }
        return Unknown;
    }
}

QString Common::nameForSink(PulseAudioQt::Sink* sink) {
    QVariantMap properties = sink->properties();
    QString api = properties.value("device.api").toString();
    if (api == QStringLiteral("alsa")) {
        return properties.value("device.product.name", properties.value("node.nick")).toString();
    } else if (api == QStringLiteral("bluez")) {
        return properties.value("bluez.alias").toString();
    } else {
        return sink->description();
    }
}
