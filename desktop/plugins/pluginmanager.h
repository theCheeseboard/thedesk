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
#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>

struct PluginManagerPrivate;
class PluginManager : public QObject {
        Q_OBJECT
    public:
        static PluginManager* instance();

        void setSafeMode(bool safeMode);
        bool isSafeMode();

        void scanPlugins();

        void activatePlugin(QUuid uuid);
        void deactivatePlugin(QUuid uuid);

        void blacklistPlugin(QUuid uuid);
        void removeBlacklistPlugin(QUuid uuid);

        QList<QUuid> availablePlugins();
        QList<QUuid> loadedPlugins();
        QList<QUuid> erroredPlugins();
        QList<QUuid> blacklistedPlugins();
        QJsonValue pluginMetadata(QUuid plugin, QString key);
        QString pluginErrorReason(QUuid plugin);

    signals:
        void pluginsChanged();

    private:
        explicit PluginManager(QObject* parent = nullptr);
        static PluginManagerPrivate* d;

        void updateBlacklistedPlugins();
};

#endif // PLUGINMANAGER_H
