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
#include "plugin.h"

#include <QDebug>
#include <clockchunk.h>
#include <statemanager.h>
#include <barmanager.h>
#include <statuscentermanager.h>
#include <icontextchunk.h>
#include <localemanager.h>
#include <QIcon>
#include <QApplication>
#include <QDir>
#include "OverviewPane/overviewpane.h"

struct PluginPrivate {
    int translationSet;

    ClockChunk* chunk;
    OverviewPane* overviewPane;
};

Plugin::Plugin() {
    d = new PluginPrivate();
}

Plugin::~Plugin() {
    delete d;
}

void Plugin::activate() {
    d->translationSet = StateManager::localeManager()->addTranslationSet({
        QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/OverviewPlugin/translations"),
        "/usr/share/thedesk/OverviewPlugin/translations"
    });

    d->chunk = new ClockChunk();
    StateManager::barManager()->addChunk(d->chunk);

    d->overviewPane = new OverviewPane();
    StateManager::statusCenterManager()->addPane(d->overviewPane);
}

void Plugin::deactivate() {
    StateManager::statusCenterManager()->removePane(d->overviewPane);
    StateManager::barManager()->removeChunk(d->chunk);

    d->overviewPane->deleteLater();
    d->chunk->deleteLater();

    StateManager::localeManager()->removeTranslationSet(d->translationSet);
}
