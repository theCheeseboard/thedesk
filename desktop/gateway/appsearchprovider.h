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
#ifndef APPSEARCHPROVIDER_H
#define APPSEARCHPROVIDER_H

#include <gatewaysearchprovider.h>

struct AppSearchProviderPrivate;
class AppSearchProvider : public GatewaySearchProvider {
        Q_OBJECT
    public:
        explicit AppSearchProvider(QObject* parent = nullptr);
        ~AppSearchProvider();

        tPromise<QList<QVariantMap>>* searchResults(QString query);
        void launch(QVariantMap data);
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariantMap data) const;
        QSize sizeHint(const QStyleOptionViewItem& option, const QVariantMap data) const;

    signals:

    private:
        AppSearchProviderPrivate* d;

};

#endif // APPSEARCHPROVIDER_H
