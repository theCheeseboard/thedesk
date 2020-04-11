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

Plugin::Plugin() {
    qDebug() << "Construct";
}

Plugin::~Plugin() {
    qDebug() << "Destruct";
}

void Plugin::activate() {
    StateManager::localeManager()->addTranslationSet({
        QDir::cleanPath(qApp->applicationDirPath() + "/../plugins/OverviewPlugin/translations"),
        "/usr/share/thedesk/OverviewPlugin/translations"
    });

    BarManager* barManager = StateManager::barManager();
    barManager->addChunk(new ClockChunk());

    StatusCenterManager* scManager = StateManager::statusCenterManager();
    scManager->addPane(new OverviewPane());
}

void Plugin::deactivate() {

}
