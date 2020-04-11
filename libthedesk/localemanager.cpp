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
#include "localemanager.h"

#include <QMap>
#include <QTranslator>
#include <QApplication>
#include <QDir>

struct LocaleManagerPrivate {
    QMap<int, QTranslator*> translators;
    QMap<int, QStringList> searchPaths;

    int current = 0;
};

LocaleManager::LocaleManager(QObject* parent) : QObject(parent) {
    d = new LocaleManagerPrivate();

    this->addTranslationSet({
        QDir::cleanPath(qApp->applicationDirPath() + "/../libthedesk/translations"),
        "/usr/share/thedesk/libthedesk/translations"
    });
}

LocaleManager::~LocaleManager() {
    delete d;
}

int LocaleManager::addTranslationSet(QStringList searchPaths) {
    int id = d->current;
    d->current++;

    QTranslator* translator = new QTranslator(this);
    qApp->installTranslator(translator);

    d->translators.insert(id, translator);
    d->searchPaths.insert(id, searchPaths);

    updateTranslator(id);

    return id;
}

void LocaleManager::removeTranslationSet(int translationSet) {
    if (!d->translators.contains(translationSet)) return;

    QTranslator* translator = d->translators.value(translationSet);
    qApp->removeTranslator(translator);
    translator->deleteLater();

    d->translators.remove(translationSet);
    d->searchPaths.remove(translationSet);
}

void LocaleManager::updateTranslator(int id) {
    QTranslator* translator = d->translators.value(id);
    QStringList searchPaths = d->searchPaths.value(id);
    for (QString path : searchPaths) {
        if (translator->load(QLocale(), "", "", path, ".qm")) return; //Success!
    }
}
