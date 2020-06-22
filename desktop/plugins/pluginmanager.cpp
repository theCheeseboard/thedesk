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
#include <tsettings.h>
#include "plugininterface.h"

typedef QSharedPointer<QPluginLoader> QPluginLoaderPtr;

struct PluginManagerPrivate {
    PluginManager* instance = nullptr;

    QList<QUuid> loadedPlugins;
    QList<QUuid> erroredPlugins;
    QMap<QUuid, QPluginLoaderPtr> foundPlugins;

    tSettings* settings;
    QList<QUuid> blacklistedPlugins;

    bool safeMode = false;
};

PluginManagerPrivate* PluginManager::d = new PluginManagerPrivate();

PluginManager* PluginManager::instance() {
    if (!d->instance) d->instance = new PluginManager();
    return d->instance;
}

void PluginManager::setSafeMode(bool safeMode) {
    d->safeMode = safeMode;
}

bool PluginManager::isSafeMode() {
    return d->safeMode;
}

void PluginManager::scanPlugins() {
    //Load all available plugins
    QStringList searchPaths = {
        QDir::cleanPath(qApp->applicationDirPath() + "/../plugins"),
        QString(SYSTEM_LIBRARY_DIRECTORY).append("/thedesk/plugins")
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

            if (!d->safeMode) {
                //Load this plugin
                this->activatePlugin(uuid);
            }
        }
    }
}

void PluginManager::activatePlugin(QUuid uuid) {
    if (!d->foundPlugins.contains(uuid)) return;
    if (d->blacklistedPlugins.contains(uuid)) return;
    QPluginLoaderPtr loader = d->foundPlugins.value(uuid);

    d->erroredPlugins.removeAll(uuid);

    if (!loader->load()) {
        //Error!
        d->erroredPlugins.append(uuid);
        emit pluginsChanged();
        return;
    }

    PluginInterface* instance = qobject_cast<PluginInterface*>(loader->instance());
    if (!instance) {
        //Error!
        loader->unload();
        emit pluginsChanged();
        return;
    }

    instance->activate();
    d->loadedPlugins.append(uuid);
    emit pluginsChanged();
}

void PluginManager::deactivatePlugin(QUuid uuid) {
    if (!d->loadedPlugins.contains(uuid)) return;
    QPluginLoaderPtr loader = d->foundPlugins.value(uuid);
    PluginInterface* instance = qobject_cast<PluginInterface*>(loader->instance());

    instance->deactivate();
    loader->unload();
    d->loadedPlugins.removeOne(uuid);
    emit pluginsChanged();
}

void PluginManager::blacklistPlugin(QUuid uuid) {
    if (d->blacklistedPlugins.contains(uuid)) return;

    d->blacklistedPlugins.append(uuid);
    QStringList blacklisted;
    for (QUuid uuid : d->blacklistedPlugins) {
        blacklisted.append(uuid.toString());
    }
    d->settings->setDelimitedList("Plugins/blacklist", blacklisted);
    d->settings->sync();

    //Deactivate the plugin after we add it to the blacklist in case the plugin is having problems deactivating
    //At least if we crash, the plugin will be deactivated
    if (d->loadedPlugins.contains(uuid)) deactivatePlugin(uuid);
}

void PluginManager::removeBlacklistPlugin(QUuid uuid) {
    if (!d->blacklistedPlugins.contains(uuid)) return;

    d->blacklistedPlugins.removeAll(uuid);
    QStringList blacklisted;
    for (QUuid uuid : d->blacklistedPlugins) {
        blacklisted.append(uuid.toString());
    }
    d->settings->setDelimitedList("Plugins/blacklist", blacklisted);
    d->settings->sync();
}

QList<QUuid> PluginManager::availablePlugins() {
    return d->foundPlugins.keys();
}

QList<QUuid> PluginManager::loadedPlugins() {
    return d->loadedPlugins;
}

QList<QUuid> PluginManager::erroredPlugins() {
    return d->erroredPlugins;
}

QList<QUuid> PluginManager::blacklistedPlugins() {
    return d->blacklistedPlugins;
}

QJsonValue PluginManager::pluginMetadata(QUuid plugin, QString key) {
    QPluginLoaderPtr loader = d->foundPlugins.value(plugin);
    QJsonObject metadata = loader->metaData().value("MetaData").toObject();
    QLocale locale;

    QStringList languages = locale.uiLanguages();
    languages.append("");
    for (QString language : languages) {
        QJsonValue root = metadata;
        if (!language.isEmpty()) {
            root = metadata.value(language);
        }

        bool success = true;
        QStringList parts = key.split(".");
        for (QString part : parts) {
            if (root.isObject()) {
                root = root.toObject().value(part);
            } else {
                success = false;
            }
        }

        if (success) return root;
    }

    return QJsonValue();
}

QString PluginManager::pluginErrorReason(QUuid plugin) {
    QPluginLoaderPtr loader = d->foundPlugins.value(plugin);
    return loader->errorString();
}

PluginManager::PluginManager(QObject* parent) : QObject(parent) {
    d->settings = new tSettings();

    //Find out which plugins are blacklisted
    connect(d->settings, &tSettings::settingChanged, this, [ = ](QString key) {
        if (key == "Plugins/blacklist") updateBlacklistedPlugins();
    });
    updateBlacklistedPlugins();
}

void PluginManager::updateBlacklistedPlugins() {
    d->blacklistedPlugins.clear();
    for (QString plugin : d->settings->delimitedList("Plugins/blacklist")) {
        d->blacklistedPlugins.append(QUuid::fromString(plugin));
    }
    emit pluginsChanged();
}
