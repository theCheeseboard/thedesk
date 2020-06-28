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
#include "appselectionmodel.h"

#include <tpromise.h>
#include <Applications/application.h>
#include <the-libs_global.h>

struct AppSelectionModelPrivate {
    QString currentQuery;

    QList<ApplicationPointer> apps;
    QList<ApplicationPointer> appsShown;
    QMap<QString, QPixmap> appIcons;
};

AppSelectionModel::AppSelectionModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new AppSelectionModelPrivate();
    connect(ApplicationDaemon::instance(), &ApplicationDaemon::appsUpdateRequired, this, &AppSelectionModel::updateData);
    updateData();
}

AppSelectionModel::~AppSelectionModel() {
    delete d;
}

int AppSelectionModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())  return 0;

    return d->appsShown.count();
}

QVariant AppSelectionModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    ApplicationPointer a = d->appsShown.at(index.row());
    if (d->appsShown.count() > index.row()) {
        if (role == Qt::DisplayRole) {
            return a->getProperty("Name", a->desktopEntry());
        } else if (role == Qt::DecorationRole) {
            //Cache the icon for smooth scrolling
            if (!d->appIcons.contains(a->desktopEntry())) d->appIcons.insert(a->desktopEntry(), QIcon::fromTheme(a->getProperty("Icon").toString()).pixmap(SC_DPI_T(QSize(32, 32), QSize)));
            return d->appIcons.value(a->desktopEntry());
        } else if (role == Qt::UserRole) { //Description
            return a->getProperty("GenericName", tr("Application"));
        } else if (role == Qt::UserRole + 1) { //Pinned
//            return d->pinnedAppsList.contains(a->desktopEntry());
            return false;
        } else if (role == Qt::UserRole + 2) { //Desktop Entry
            return a->desktopEntry();
        } else if (role == Qt::UserRole + 3) { //App
            return QVariant::fromValue(a);
        }
    }

    return QVariant();
}

void AppSelectionModel::search(QString query) {
    d->currentQuery = query;
    d->appsShown.clear();

    //If there is no current search query, show all apps
    if (query == "") {
        d->appsShown.append(d->apps);
        emit dataChanged(index(0), index(rowCount()));
        return;
    }

    //TODO: Run the search query past search plugins

    for (ApplicationPointer app : d->apps) {
        QStringList possibleWords;
        possibleWords.append(app->getProperty("Name").toString());
        possibleWords.append(app->getProperty("GenericName").toString());
        possibleWords.append(app->getStringList("Keywords"));

        for (QString s : possibleWords) {
            if (s.contains(query, Qt::CaseInsensitive)) {
                d->appsShown.append(app);
                break;
            }
        }
    }

    if (theLibsGlobal::searchInPath(query.split(" ")[0]).count() > 0) {
        d->appsShown.append(ApplicationPointer(new Application({
            {"Name", query},
            {"Exec", query},
            {"GenericName", tr("Run Command")},
            {"Icon", "system-run"}
        })));
    }

    emit dataChanged(index(0), index(rowCount()));
}

void AppSelectionModel::updateData() {
    emit loading();
    d->apps.clear();
    d->appIcons.clear();

    struct ApplicationListReturnValue {
        QList<ApplicationPointer> apps;
        QMap<ApplicationPointer, QPixmap> appIcons;
    };

    (new tPromise<QList<ApplicationPointer>>([ = ](QString & error) {
        QList<ApplicationPointer> apps;

        QList<ApplicationPointer> normalApps;
        for (QString desktopEntry : Application::allApplications()) {
            ApplicationPointer a(new Application(desktopEntry));

            //Make sure this app is good to be shown
            if (a->getProperty("Type", "").toString() != "Application") continue;
            if (a->getProperty("NoDisplay", false).toBool()) continue;
            if (!a->getStringList("OnlyShowIn", {"thedesk"}).contains("thedesk")) continue;
            if (a->getStringList("NotShowIn").contains("thedesk")) continue;
            normalApps.append(a);
        }

        std::sort(normalApps.begin(), normalApps.end(), [](const ApplicationPointer & a, const ApplicationPointer & b) -> bool {
            if (a->getProperty("Name").toString().localeAwareCompare(b->getProperty("Name").toString()) < 0) {
                return true;
            } else {
                return false;
            }
        });
        apps.append(normalApps);

        return apps;
    }))->then([ = ](QList<ApplicationPointer> apps) {
        d->apps = apps;

        //Perform a search to initialize the list
        search(d->currentQuery);
        emit ready();
    });
}
