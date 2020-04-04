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
#include "pluginmanager.h"

#include <QSharedPointer>
#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QPluginLoader>
#include <QDebug>
#include <QUuid>
#include "plugininterface.h"

typedef QSharedPointer<QPluginLoader> QPluginLoaderPtr;

struct PluginManagerPrivate {
    PluginManager* instance = nullptr;

    QList<QUuid> loadedPlugins;
    QMap<QUuid, QPluginLoaderPtr> foundPlugins;
};

PluginManagerPrivate* PluginManager::d = new PluginManagerPrivate();

PluginManager* PluginManager::instance() {
    if (!d->instance) d->instance = new PluginManager();
    return d->instance;
}

void PluginManager::activatePlugin(QUuid uuid) {
    if (!d->foundPlugins.contains(uuid)) return;
    QPluginLoaderPtr loader = d->foundPlugins.value(uuid);

    if (!loader->load()) {
        //Error!
        return;
    }

    PluginInterface* instance = qobject_cast<PluginInterface*>(loader->instance());
    if (!instance) {
        //Error!
        loader->unload();
        return;
    }

    instance->activate();
    d->loadedPlugins.append(uuid);
}

void PluginManager::deactivatePlugin(QUuid uuid) {
    if (!d->loadedPlugins.contains(uuid)) return;
    QPluginLoaderPtr loader = d->foundPlugins.value(uuid);
    PluginInterface* instance = qobject_cast<PluginInterface*>(loader->instance());

    instance->deactivate();
    loader->unload();
    d->loadedPlugins.removeOne(uuid);
}

PluginManager::PluginManager(QObject* parent) : QObject(parent) {
    //Load all available plugins
    QStringList searchPaths = {
        QDir::cleanPath(qApp->applicationDirPath() + "/../plugins"),
        "/usr/lib/thedesk/plugins"
    };
    QStringList seenUuids;

    for (QString searchPath : searchPaths) {
        QDirIterator iterator(searchPath, {"*.so"}, QDir::NoFilter, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            QPluginLoaderPtr loader(new QPluginLoader(iterator.next()));
            QJsonObject metadata = loader->metaData().value("MetaData").toObject();
            if (metadata.isEmpty()) continue;

            QUuid uuid = QUuid::fromString(metadata.value("uuid").toString());
            if (d->foundPlugins.contains(uuid)) continue;

            //Register this plugin
            d->foundPlugins.insert(uuid, loader);

            //Load this plugin
            this->activatePlugin(uuid);
        }
    }
}
