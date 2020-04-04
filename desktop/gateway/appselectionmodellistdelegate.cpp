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

#include <application.h>
#include <QPainter>
#include <the-libs_global.h>

AppSelectionModelListDelegate::AppSelectionModelListDelegate(QWidget* parent, bool drawArrows) : QStyledItemDelegate(parent) {
    this->drawArrows = drawArrows;
}

void AppSelectionModelListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->setFont(option.font);
    painter->setLayoutDirection(option.direction);

    QRect iconRect;
    iconRect.setLeft(option.rect.left() + SC_DPI(6));
    iconRect.setTop(option.rect.top() + SC_DPI(6));
    iconRect.setBottom(iconRect.top() + SC_DPI(32));
    iconRect.setRight(iconRect.left() + SC_DPI(32));

    QRect textRect;
    textRect.setLeft(iconRect.right() + SC_DPI(6));
    textRect.setTop(option.rect.top() + SC_DPI(6));
    textRect.setBottom(option.rect.top() + option.fontMetrics.height() + SC_DPI(6));
    textRect.setRight(option.rect.right());

    QRect descRect;
    descRect.setLeft(iconRect.right() + SC_DPI(6));
    descRect.setTop(option.rect.top() + option.fontMetrics.height() + SC_DPI(8));
    descRect.setBottom(option.rect.top() + option.fontMetrics.height() * 2 + SC_DPI(6));
    descRect.setRight(option.rect.right());

    if (option.direction == Qt::RightToLeft) {
        iconRect.moveRight(option.rect.right() - SC_DPI(6));
        textRect.moveRight(iconRect.left() - SC_DPI(6));
        descRect.moveRight(iconRect.left() - SC_DPI(6));
    }

    if (option.state & QStyle::State_Selected) {
        painter->setPen(Qt::transparent);
        painter->setBrush(option.palette.color(QPalette::Highlight));
        painter->drawRect(option.rect);
        painter->setBrush(Qt::transparent);
        painter->setPen(option.palette.color(QPalette::HighlightedText));
        painter->drawText(textRect, Qt::AlignLeading, index.data().toString());
        painter->drawText(descRect, Qt::AlignLeading, index.data(Qt::UserRole).toString());
    } else if (option.state & QStyle::State_MouseOver) {
        QColor col = option.palette.color(QPalette::Highlight);
        col.setAlpha(127);
        painter->setBrush(col);
        painter->setPen(Qt::transparent);
        painter->drawRect(option.rect);
        painter->setBrush(Qt::transparent);
        painter->setPen(option.palette.color(QPalette::WindowText));
        painter->drawText(textRect, Qt::AlignLeading, index.data().toString());
        painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
        painter->drawText(descRect, Qt::AlignLeading, index.data(Qt::UserRole).toString());
    } else {
        painter->setPen(option.palette.color(QPalette::WindowText));
        painter->drawText(textRect, Qt::AlignLeading, index.data().toString());
        painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
        painter->drawText(descRect, Qt::AlignLeading, index.data(Qt::UserRole).toString());
    }
    painter->drawPixmap(iconRect, index.data(Qt::DecorationRole).value<QPixmap>());

    if (drawArrows) {
        ApplicationPointer a = index.data(Qt::UserRole + 3).value<ApplicationPointer>();
        if (a->getStringList("Actions").count() > 0) { //Actions included
            QRect actionsRect;
            actionsRect.setWidth(SC_DPI(16));
            actionsRect.setHeight(SC_DPI(16));
            actionsRect.moveRight(option.rect.right() - SC_DPI(9));
            actionsRect.moveTop(option.rect.top() + option.rect.height() / 2 - SC_DPI(8));

            if (option.direction == Qt::RightToLeft) {
                actionsRect.moveLeft(option.rect.left() + SC_DPI(9));
            }

            painter->drawPixmap(actionsRect, QIcon::fromTheme("arrow-right").pixmap(SC_DPI(16), SC_DPI(16)));
        }
    }

//    int pinned = ((AppsListModel*) index.model())->pinnedAppsCount();
//    if (index.row() == pinned - 1 && ((AppsListModel*) index.model())->currentQuery == "") {
//        painter->setPen(option.palette.color(QPalette::WindowText));
//        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
//    }
}

QSize AppSelectionModelListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    int fontHeight = option.fontMetrics.height() * 2 + SC_DPI(14);
    int iconHeight = SC_DPI(46);

    return QSize(option.fontMetrics.horizontalAdvance(index.data().toString()), qMax(fontHeight, iconHeight));
}
