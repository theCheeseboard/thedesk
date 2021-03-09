/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "gatewaysearchmodel.h"

#include <statemanager.h>
#include <gatewaymanager.h>
#include <gatewaysearchprovider.h>

struct GatewaySearchModelPrivate {
    QString query;
    QList<QVariantMap> currentItems;
};

GatewaySearchModel::GatewaySearchModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new GatewaySearchModelPrivate();
}

GatewaySearchModel::~GatewaySearchModel() {
    delete d;
}

void GatewaySearchModel::search(QString query) {
    d->currentItems.clear();
    for (GatewaySearchProvider* searchProvider : StateManager::gatewayManager()->searchProviders()) {
        searchProvider->searchResults(query)->then([ = ](QList<QVariantMap> results) {
            QList<QVariantMap> endResults;
            for (QVariantMap result : results) {
                endResults.append({
                    {"provider", QVariant::fromValue(searchProvider)},
                    {"data", result},
                    {"priority", result.value("priority", 1)},
                    {"mainText", result.value("mainText", "")}
                });
            }
            d->currentItems.append(endResults);

            //Sort the items
            std::stable_sort(d->currentItems.begin(), d->currentItems.end(), [ = ](const QVariantMap first, const QVariantMap second) {
                if (first.value("priority") == second.value("priority")) {
                    return first.value("mainText").toString().localeAwareCompare(second.value("mainText").toString()) < 0;
                }
                return first.value("priority").toInt() < second.value("priority").toInt();
            });

            emit dataChanged(index(0), index(rowCount()));
        });
    }
}

int GatewaySearchModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return d->currentItems.count();
}

QVariant GatewaySearchModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    switch (role) {
        case Qt::UserRole:
            return d->currentItems.at(index.row()).value("data");
        case Qt::UserRole + 1:
            return d->currentItems.at(index.row()).value("provider");
    }

    return QVariant();
}

void GatewaySearchModel::launch(const QModelIndex& index) const {
    GatewaySearchProvider* provider = index.data(Qt::UserRole + 1).value<GatewaySearchProvider*>();
    provider->launch(index.data(Qt::UserRole).toMap());
}
