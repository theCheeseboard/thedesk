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
#ifndef FILECHOOSERINTERFACE_H
#define FILECHOOSERINTERFACE_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusMessage>

class FileChooserInterface : public QDBusAbstractAdaptor {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.FileChooser")

    public:
        explicit FileChooserInterface(QObject* parent = nullptr);

    public slots:
        Q_SCRIPTABLE uint OpenFile(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results);
        Q_SCRIPTABLE uint SaveFile(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results);
        Q_SCRIPTABLE uint SaveFiles(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results);

    signals:

};

#endif // FILECHOOSERINTERFACE_H
