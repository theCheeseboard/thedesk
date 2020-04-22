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
#include "pluginmodel.h"

#include <QUuid>
#include <QJsonValue>
#include <QIcon>
#include "plugins/pluginmanager.h"

PluginModel::PluginModel(QObject* parent)
    : QAbstractListModel(parent) {
    connect(PluginManager::instance(), &PluginManager::pluginsChanged, this, [ = ] {
        emit dataChanged(index(0), index(rowCount()));
    });
}

int PluginModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return PluginManager::instance()->availablePlugins().count();
}

QVariant PluginModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    PluginManager* plugins = PluginManager::instance();
    QUuid uuid = plugins->availablePlugins().at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return plugins->pluginMetadata(uuid, "name").toString();
        case Qt::DecorationRole:
            return QIcon::fromTheme(plugins->pluginMetadata(uuid, "icon").toString(), QIcon::fromTheme("generic-app"));
        case Qt::UserRole: {
            if (plugins->loadedPlugins().contains(uuid)) {
                return tr("Loaded");
            } else if (plugins->blacklistedPlugins().contains(uuid)) {
                return tr("Blacklisted");
            } else if (plugins->erroredPlugins().contains(uuid)) {
                return tr("Unavailable");
            } else {
                return tr("Inactive");
            }
        }
        case Qt::UserRole + 1:
            return uuid;
        case Qt::UserRole + 2:
            return plugins->loadedPlugins().contains(uuid);
        case Qt::UserRole + 3:
            return plugins->blacklistedPlugins().contains(uuid);
    }

    return QVariant();
}
