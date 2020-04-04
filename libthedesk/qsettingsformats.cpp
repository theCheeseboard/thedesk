/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
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
#include "qsettingsformats.h"

#include <QIODevice>

struct QSettingsFormatsPrivate {
    QSettings::Format desktop = QSettings::InvalidFormat;
};

QSettingsFormatsPrivate* QSettingsFormats::d = new QSettingsFormatsPrivate();

QSettingsFormats::QSettingsFormats()
{

}

QSettings::Format QSettingsFormats::desktopFormat() {
    if (d->desktop == QSettings::InvalidFormat) {
        d->desktop = QSettings::registerFormat("desktop", [](QIODevice &device, QSettings::SettingsMap &map) -> bool {
            QString group;
            while (!device.atEnd()) {
                QString line = device.readLine().trimmed();
                if (line.startsWith("#")) continue;
                if (line.startsWith("[") && line.endsWith("]")) {
                    group = line.mid(1, line.length() - 2);
                } else {
                    QString key = line.left(line.indexOf("="));
                    QString value = line.mid(line.indexOf("=") + 1);
                    map.insert(group + "/" + key, value);
                }
            }
            return true;
        }, [](QIODevice &device, const QSettings::SettingsMap &map) -> bool {
            return false;
        }, Qt::CaseInsensitive);
    }

    return d->desktop;
}
