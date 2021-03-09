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
#include "appselectionmodellistdelegate.h"

#include <Applications/application.h>
#include <QPainter>
#include <the-libs_global.h>
#include "appsearchprovider.h"

AppSelectionModelListDelegate::AppSelectionModelListDelegate(QWidget* parent, bool drawArrows) : QStyledItemDelegate(parent) {
    this->drawArrows = drawArrows;
}

void AppSelectionModelListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    AppSearchProvider provider;
    return provider.paint(painter, option, {
        {"application", index.data(Qt::UserRole + 2).toString()},
        {"drawArrows", this->drawArrows}
    });
}

QSize AppSelectionModelListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    AppSearchProvider provider;
    return provider.sizeHint(option, {
        {"application", index.data(Qt::UserRole + 2).toString()}
    });
}
