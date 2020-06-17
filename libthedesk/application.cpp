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
#include "application.h"

#include "qsettingsformats.h"
#include <QDir>
#include <QDirIterator>
#include <QLocale>
#include <QSet>
#include <QFileSystemWatcher>
#include <QProcess>

struct ApplicationPrivate {
    QSettings* appSettings = nullptr;
    QVariantMap details;

    bool useSettings = false;
    bool isValid = false;
    QString desktopEntry;

    static const QStringList searchPaths;
};

const QStringList ApplicationPrivate::searchPaths = {
    QDir::homePath() + "/.local/share/applications",
    "/usr/share/applications"
};
ApplicationDaemon* ApplicationDaemon::d = nullptr;

Application::Application() {
    d = new ApplicationPrivate();
}

Application::Application(QString desktopEntry, QStringList searchPaths) {
    d = new ApplicationPrivate();

    if (searchPaths.isEmpty()) searchPaths = ApplicationPrivate::searchPaths;

    for (QString searchPath : searchPaths) {
        QDirIterator iterator(searchPath, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();
            if (desktopEntry == iterator.fileInfo().completeBaseName()) {
                //We found the file
                d->appSettings = new QSettings(iterator.filePath(), QSettingsFormats::desktopFormat());
                d->desktopEntry = desktopEntry;
                d->useSettings = true;
                d->isValid = true;
                return;
            }
        }
    }
}

Application::Application(QVariantMap details) {
    d = new ApplicationPrivate();

    d->details = details;
    d->useSettings = false;
    d->isValid = true;
}

Application::~Application() {
    delete d;
}

bool Application::isValid() {
    return d->isValid;
}

bool Application::hasProperty(QString propertyName) const {
    if (!d->isValid) return false;
    QLocale locale;
    if (d->useSettings) d->appSettings->beginGroup("Desktop Entry");

    //Check to see if there's a localised version of the property name
    QString usedKey = propertyName;
    for (QString trialKey : {
            "[" + locale.name() + "]", "[" + locale.name().split("_").first() + "]", QString("")
        }) {
        QString test = propertyName + trialKey;
        if ((d->useSettings && d->appSettings->contains(test)) || (!d->useSettings && d->details.contains(test))) {
            if (d->useSettings) d->appSettings->endGroup();
            return true;
        }
    }

    if (d->useSettings) d->appSettings->endGroup();
    return false;
}

QVariant Application::getProperty(QString propertyName, QVariant defaultValue) const {
    if (!d->isValid) return QVariant();

    QLocale locale;
    if (d->useSettings) d->appSettings->beginGroup("Desktop Entry");

    //Check to see if there's a localised version of the property name
    QString usedKey = propertyName;
    for (QString trialKey : {
            "[" + locale.name() + "]", "[" + locale.name().split("_").first() + "]"
        }) {
        QString test = propertyName + trialKey;
        if ((d->useSettings && d->appSettings->contains(test)) || (!d->useSettings && d->details.contains(test))) {
            usedKey = test;
            break;
        }
    }

    //Return the property
    QVariant retval = d->useSettings ? d->appSettings->value(usedKey, defaultValue) : d->details.value(usedKey, defaultValue);
    if (d->useSettings) d->appSettings->endGroup();
    return retval;
}

QVariant Application::getActionProperty(QString action, QString propertyName, QVariant defaultValue) const {
    if (!d->isValid) return QVariant();

    QLocale locale;
    if (d->useSettings) d->appSettings->beginGroup("Desktop Action " + action);

    //Check to see if there's a localised version of the property name
    QString usedKey = propertyName;
    for (QString trialKey : {
            "[" + locale.name() + "]", "[" + locale.name().split("_").first() + "]"
        }) {
        QString test = propertyName + trialKey;
        if ((d->useSettings && d->appSettings->contains(test)) || (!d->useSettings && d->details.contains(test))) {
            usedKey = test;
            break;
        }
    }

    //Return the property
    QVariant retval = d->useSettings ? d->appSettings->value(usedKey, defaultValue) : d->details.value(usedKey, defaultValue);
    if (d->useSettings) d->appSettings->endGroup();
    return retval;
}

QStringList Application::getStringList(QString propertyName, QStringList defaultValue) const {
    if (!d->isValid) return QStringList();

    QString property = getProperty(propertyName, defaultValue).toString();
    return property.split(";", Qt::SkipEmptyParts);
}

QStringList Application::allApplications(QStringList searchPaths) {
    if (searchPaths.isEmpty()) searchPaths = ApplicationPrivate::searchPaths;

    QSet<QString> stringSet;
    for (QString searchPath : searchPaths) {
        QDirIterator iterator(searchPath, {"*.desktop"}, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();
            stringSet.insert(iterator.fileInfo().completeBaseName());
        }
    }
    return stringSet.values();
}

QString Application::desktopEntry() const {
    if (!d->isValid) return "";

    return d->desktopEntry;
}

void Application::launch() {
    QString command = this->getProperty("Exec").toString().remove("%u");
    command.remove("env ");
    QProcess* process = new QProcess();
    QStringList environment = process->environment();
    QStringList commandSpace = command.split(" ");
    for (QString part : commandSpace) {
        if (part.contains("=")) {
            environment.append(part);
            commandSpace.removeOne(part);
        }
    }
    commandSpace.removeAll("");

    commandSpace.removeAll("%f");
    commandSpace.removeAll("%F");
    commandSpace.removeAll("%u");
    commandSpace.removeAll("%U");
    if (commandSpace.contains("%i")) {
        if (this->hasProperty("Icon")) {
            commandSpace.replaceInStrings("%i", "--icon " + this->getProperty("Icon").toString());
        } else {
            commandSpace.removeAll("%i");
        }
    }
    commandSpace.replaceInStrings("%c", this->getProperty("Name").toString());
    commandSpace.replaceInStrings("%%", "%");

    QString finalCommand = commandSpace.join(" ");
    process->start(finalCommand, QStringList());
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [ = ](int exitCode, QProcess::ExitStatus exitStatus) {
        process->deleteLater();
    });
    QObject::connect(process, &QProcess::readyReadStandardError, [ = ] {
        process->readAllStandardError(); //Discard stderr
    });
    QObject::connect(process, &QProcess::readyReadStandardOutput, [ = ] {
        process->readAllStandardOutput(); //Discard stdout
    });
}

ApplicationDaemon::ApplicationDaemon() : QObject(nullptr) {
    QFileSystemWatcher* watcher = new QFileSystemWatcher();
    watcher->addPaths(ApplicationPrivate::searchPaths);
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &ApplicationDaemon::appsUpdateRequired);
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &ApplicationDaemon::appsUpdateRequired);
}

ApplicationDaemon* ApplicationDaemon::instance() {
    if (d == nullptr) d = new ApplicationDaemon();
    return d;
}
