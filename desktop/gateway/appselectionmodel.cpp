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

#include <Applications/application.h>
#include <libcontemporary_global.h>
#include <tpromise.h>

struct AppSelectionModelPrivate {
        QString category;

        QList<ApplicationPointer> apps;
        QList<ApplicationPointer> appsShown;
};

AppSelectionModel::AppSelectionModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new AppSelectionModelPrivate();
    connect(ApplicationDaemon::instance(), &ApplicationDaemon::appsUpdateRequired, this, &AppSelectionModel::updateData);
    updateData();
}

AppSelectionModel::~AppSelectionModel() {
    delete d;
}

int AppSelectionModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->appsShown.count();
}

QVariant AppSelectionModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    ApplicationPointer a = d->appsShown.at(index.row());
    if (d->appsShown.count() > index.row()) {
        if (role == Qt::DisplayRole) {
            return a->getProperty("Name", a->desktopEntry());
        } else if (role == Qt::DecorationRole) {
            // Cache the icon for smooth scrolling
            return a->icon(SC_DPI_T(QSize(32, 32), QSize));
        } else if (role == Qt::UserRole) { // Description
            return a->getProperty("GenericName", tr("Application"));
        } else if (role == Qt::UserRole + 1) { // Pinned
            //            return d->pinnedAppsList.contains(a->desktopEntry());
            return false;
        } else if (role == Qt::UserRole + 2) { // Desktop Entry
            return a->desktopEntry();
        } else if (role == Qt::UserRole + 3) { // App
            return QVariant::fromValue(a);
        }
    }

    return QVariant();
}

void AppSelectionModel::filterCategory(QString category) {
    d->category = category;
    d->appsShown.clear();

    // If there is no current category, show all apps
    if (category == "") {
        d->appsShown.append(d->apps);
        emit dataChanged(index(0), index(rowCount()));
        return;
    }

    for (ApplicationPointer app : qAsConst(d->apps)) {
        QStringList categories = app->getStringList("Categories");

        if (categories.contains(category, Qt::CaseSensitive)) d->appsShown.append(app);
    }

    emit dataChanged(index(0), index(rowCount()));
}

void AppSelectionModel::updateData() {
    emit loading();
    d->apps.clear();

    struct ApplicationListReturnValue {
            QList<ApplicationPointer> apps;
            QMap<ApplicationPointer, QPixmap> appIcons;
    };

    (new tPromise<QList<ApplicationPointer>>([=](QString& error) {
        QList<ApplicationPointer> apps;

        QList<ApplicationPointer> normalApps;
        for (QString desktopEntry : Application::allApplications()) {
            ApplicationPointer a(new Application(desktopEntry));

            // Make sure this app is good to be shown
            if (a->getProperty("Type", "").toString() != "Application") continue;
            if (a->getProperty("NoDisplay", false).toBool()) continue;
            if (!a->getStringList("OnlyShowIn", {"thedesk"}).contains("thedesk")) continue;
            if (a->getStringList("NotShowIn").contains("thedesk")) continue;
            normalApps.append(a);
        }

        std::sort(normalApps.begin(), normalApps.end(), [](const ApplicationPointer& a, const ApplicationPointer& b) -> bool {
            if (a->getProperty("Name").toString().localeAwareCompare(b->getProperty("Name").toString()) < 0) {
                return true;
            } else {
                return false;
            }
        });
        apps.append(normalApps);

        return apps;
    }))->then([=](QList<ApplicationPointer> apps) {
        d->apps = apps;

        // Perform a category search to initialize the list
        filterCategory(d->category);
        emit ready();
    });
}
