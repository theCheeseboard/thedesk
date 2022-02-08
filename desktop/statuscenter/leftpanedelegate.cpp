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
#include "leftpanedelegate.h"

#include <QPainter>
#include <tpaintcalculator.h>
#include <the-libs_global.h>

LeftPaneDelegate::LeftPaneDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}


void LeftPaneDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyledItemDelegate::paint(painter, option, index);
    if (index.data(Qt::UserRole).toBool()) {
        //Draw an indicator arrow
        tPaintCalculator calculator;
        calculator.setDrawBounds(option.rect);
        calculator.setLayoutDirection(option.direction);

        QRect iconRect;
        iconRect.setSize(SC_DPI_T(QSize(16, 16), QSize));
        iconRect.moveCenter(option.rect.center());
        iconRect.moveRight(option.rect.right() - SC_DPI(6));

        calculator.addRect(iconRect, [ = ](QRectF drawBounds) {
            painter->drawPixmap(drawBounds.toRect(), QIcon::fromTheme("arrow-right").pixmap(iconRect.size()));
        });
        calculator.performPaint();
    }
}
