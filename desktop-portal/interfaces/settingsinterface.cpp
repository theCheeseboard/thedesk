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
#include "settingsinterface.h"
#include <tsettings.h>

SettingsInterface::SettingsInterface(QObject* parent) : QDBusAbstractAdaptor(parent) {

}

uint SettingsInterface::version() {
    return 1;
}

QMap<QString, QVariantMap> SettingsInterface::ReadAll(QStringList namespaces) {
    return {
        {
            "org.freedesktop.appearance", {
                {"color-scheme", QVariant(0u)}
            }
        }
    };
}

QVariant SettingsInterface::Read(QString ns, QString key) {
    if (ns == "org.freedesktop.appearance" && key == "color-scheme") {
        tSettings themeSettings("theDesk.platform");

        QString baseColor = themeSettings.value("Palette/base").toString();
        if (baseColor == "dark") {
            return 1u;
        } else if (baseColor == "light") {
            return 2u;
        }
    }
    return QVariant();
}
