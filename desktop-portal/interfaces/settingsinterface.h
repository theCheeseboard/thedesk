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
#ifndef SETTINGSINTERFACE_H
#define SETTINGSINTERFACE_H

#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QObject>

class SettingsInterface : public QDBusAbstractAdaptor {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Settings");
        Q_PROPERTY(uint version READ version)

    public:
        explicit SettingsInterface(QObject* parent = nullptr);

        uint version();

    public slots:
        Q_SCRIPTABLE QMap<QString, QMap<QString, QDBusVariant> > ReadAll(QStringList namespaces);
        Q_SCRIPTABLE QDBusVariant Read(QString ns, QString key);

    signals:
        Q_SCRIPTABLE void SettingChanged(QString ns, QString key, QDBusVariant value);
};

#endif // SETTINGSINTERFACE_H
