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
#include "devicedelegate.h"

#include "bluetoothplugincommon.h"
#include <QPainter>
#include <libcontemporary_global.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tpaintcalculator.h>

#include <BluezQt/DevicesModel>

struct DeviceDelegatePrivate {
        bool isPair;

        struct Rects {
                QRect iconRect;
                QRect textRect;
                QRect descRect;
                QRect arrowRect;

                Rects(const QStyleOptionViewItem& option) {
                    QRect newRect = option.rect;
                    QPoint center = newRect.center();
                    newRect.setWidth(StateManager::statusCenterManager()->preferredContentWidth());
                    newRect.moveCenter(center);

                    iconRect.setLeft(newRect.left() + SC_DPI(6));
                    iconRect.setTop(newRect.top() + SC_DPI(6));
                    iconRect.setBottom(iconRect.top() + SC_DPI(32));
                    iconRect.setRight(iconRect.left() + SC_DPI(32));

                    arrowRect.setSize(SC_DPI_T(QSize(16, 16), QSize));
                    arrowRect.moveCenter(newRect.center());
                    arrowRect.moveRight(newRect.right() - SC_DPI(6));

                    textRect.setLeft(iconRect.right() + SC_DPI(6));
                    textRect.setTop(newRect.top() + SC_DPI(6));
                    textRect.setBottom(newRect.top() + option.fontMetrics.height() + SC_DPI(6));
                    textRect.setRight(arrowRect.left() - SC_DPI(6));

                    descRect.setLeft(iconRect.right() + SC_DPI(6));
                    descRect.setTop(newRect.top() + option.fontMetrics.height() + SC_DPI(8));
                    descRect.setBottom(newRect.top() + option.fontMetrics.height() * 2 + SC_DPI(6));
                    descRect.setRight(arrowRect.left() - SC_DPI(6));
                }
        };
};

DeviceDelegate::DeviceDelegate(bool isPair, QObject* parent) :
    QAbstractItemDelegate(parent) {
    d = new DeviceDelegatePrivate();
    d->isPair = isPair;
}

DeviceDelegate::~DeviceDelegate() {
    delete d;
}

void DeviceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->setFont(option.font);

    tPaintCalculator calculator;
    calculator.setPainter(painter);
    calculator.setLayoutDirection(option.direction);
    calculator.setDrawBounds(option.rect);

    DeviceDelegatePrivate::Rects rects(option);
    QString text = index.data().toString();
    QString desc = BluetoothPluginCommon::stringForDeviceType(index.data(BluezQt::DevicesModel::TypeRole).value<BluezQt::Device::Type>());
    QIcon icon = QIcon::fromTheme(index.data(BluezQt::DevicesModel::IconRole).toString());
    bool active = index.data(BluezQt::DevicesModel::ConnectedRole).toBool();

    if (option.state & QStyle::State_Selected) {
        calculator.addRect(option.rect, [=](QRectF drawBounds) {
            painter->setPen(Qt::transparent);
            painter->setBrush(option.palette.color(QPalette::Highlight));
            painter->drawRect(drawBounds);
        });
        calculator.addRect(rects.textRect, [=](QRectF drawBounds) {
            painter->setBrush(Qt::transparent);
            painter->setPen(option.palette.color(QPalette::HighlightedText));

            if (!active && !d->isPair) painter->setOpacity(0.5);
            painter->drawText(drawBounds, Qt::AlignLeading, text);
        });
        calculator.addRect(rects.descRect, [=](QRectF drawBounds) {
            painter->drawText(drawBounds, Qt::AlignLeading, desc);
        });
    } else if (option.state & QStyle::State_MouseOver) {
        calculator.addRect(option.rect, [=](QRectF drawBounds) {
            QColor col = option.palette.color(QPalette::Highlight);
            col.setAlpha(127);
            painter->setBrush(col);
            painter->setPen(Qt::transparent);
            painter->drawRect(drawBounds);
        });

        calculator.addRect(rects.textRect, [=](QRectF drawBounds) {
            if (!active && !d->isPair) painter->setOpacity(0.5);
            painter->setBrush(Qt::transparent);
            painter->setPen(option.palette.color(QPalette::WindowText));
            painter->drawText(drawBounds, Qt::AlignLeading, text);
        });
        calculator.addRect(rects.descRect, [=](QRectF drawBounds) {
            painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
            painter->drawText(drawBounds, Qt::AlignLeading, desc);
        });
    } else {
        calculator.addRect(rects.textRect, [=](QRectF drawBounds) {
            if (!active && !d->isPair) painter->setOpacity(0.5);
            painter->setPen(option.palette.color(QPalette::WindowText));
            painter->drawText(drawBounds, Qt::AlignLeading, text);
        });
        calculator.addRect(rects.descRect, [=](QRectF drawBounds) {
            painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
            painter->drawText(drawBounds, Qt::AlignLeading, desc);
        });
    }

    if (!icon.isNull()) {
        calculator.addRect(rects.iconRect, [=](QRectF drawBounds) {
            painter->drawPixmap(drawBounds.toRect(), icon.pixmap(rects.iconRect.size()));
        });
    }

    calculator.addRect(rects.arrowRect, [=](QRectF drawBounds) {
        painter->setOpacity(1);
        painter->drawPixmap(drawBounds.toRect(), QIcon::fromTheme("go-next").pixmap(rects.arrowRect.size()));
    });

    calculator.performPaint();
}

QSize DeviceDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    DeviceDelegatePrivate::Rects rects(option);

    QRect u = rects.iconRect;
    u = u.united(rects.textRect);
    u = u.united(rects.descRect);
    u.adjust(SC_DPI(-6), SC_DPI(-6), SC_DPI(6), SC_DPI(6));

    return u.size();
}
