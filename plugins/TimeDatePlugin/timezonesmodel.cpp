/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
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
#include "timezonesmodel.h"

#include <QTimeZone>
#include <QPainter>

struct TimezonesModelPrivate {
    QList<QTimeZone> timezones;
    QList<QTimeZone> shownTimezones;
};

TimezonesModel::TimezonesModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new TimezonesModelPrivate();
    QDateTime now = QDateTime::currentDateTimeUtc();
    for (QByteArray timezone : QTimeZone::availableTimeZoneIds()) {
        QTimeZone tz(timezone);
        if (timezone == tz.displayName(now, QTimeZone::OffsetName)) continue; //Ignore
        d->timezones.append(tz);
    }

    //Sort timezones
    std::sort(d->timezones.begin(), d->timezones.end(), [ = ](const QTimeZone & first, const QTimeZone & second) {
        if (first.offsetFromUtc(now) < second.offsetFromUtc(now)) {
            return true;
        } else if (first.offsetFromUtc(now) > second.offsetFromUtc(now)) {
            return false;
        } else {
            return QString(first.id()).split("/").last().localeAwareCompare(QString(second.id()).split("/").last()) < 0;
        }
    });

    //Initialize the shown items
    search("");
}

TimezonesModel::~TimezonesModel() {
    delete d;
}

int TimezonesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->shownTimezones.count();
}

QVariant TimezonesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    QDateTime now = QDateTime::currentDateTimeUtc();
    QTimeZone tz = d->shownTimezones.at(index.row());
    switch (role) {
        case Qt::DisplayRole: {
            return QString(tz.id()).split("/").last().replace("_", " ");
        }
        case Qt::UserRole:
            return tz.id();
        case Qt::UserRole + 1:
            return tz.displayName(now, QTimeZone::OffsetName);
        case Qt::UserRole + 2:
            return tz.displayName(now, QTimeZone::LongName);
        case Qt::UserRole + 3:
            if (index.row() == 0 || d->shownTimezones.at(index.row() - 1).offsetFromUtc(now) != tz.offsetFromUtc(now)) {
                return true;
            } else {
                return false;
            }
    }
    return QVariant();
}

QModelIndex TimezonesModel::timezone(QTimeZone timezone) {
    for (int i = 0; i < d->timezones.count(); i++) {
        if (d->timezones.at(i) == timezone) return index(i);
    }
    return QModelIndex();
}

void TimezonesModel::search(QString query) {
    d->shownTimezones.clear();
    if (query == "") {
        d->shownTimezones = d->timezones;
    } else {
        QDateTime now = QDateTime::currentDateTime();
        for (QTimeZone tz : d->timezones) {
            bool match = false;
            if (QString(tz.id()).contains(query, Qt::CaseInsensitive)) match = true;
            if (QString(tz.id()).replace("_", " ").contains(query, Qt::CaseInsensitive)) match = true;
            if (tz.displayName(now, QTimeZone::OffsetName).contains(query, Qt::CaseInsensitive)) match = true;
            if (tz.displayName(now, QTimeZone::LongName).contains(query, Qt::CaseInsensitive)) match = true;

            if (match) d->shownTimezones.append(tz);
        }
    }

    emit dataChanged(index(0), index(rowCount()));
}

TimezonesModelDelegate::TimezonesModelDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

TimezonesModelDelegate::~TimezonesModelDelegate() {

}

void TimezonesModelDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QPen textPen, offsetPen;
    if (option.state & QStyle::State_Selected) {
        painter->setBrush(option.palette.brush(QPalette::Highlight));
        textPen = option.palette.color(QPalette::HighlightedText);
        offsetPen = option.palette.color(QPalette::HighlightedText);
    } else if (option.state & QStyle::State_MouseOver) {
        QColor col = option.palette.color(QPalette::Highlight);
        col.setAlpha(127);
        painter->setBrush(col);
        textPen = option.palette.color(QPalette::HighlightedText);
        offsetPen = option.palette.color(QPalette::HighlightedText);
    } else {
        textPen = option.palette.color(QPalette::WindowText);
        offsetPen = option.palette.color(QPalette::Disabled, QPalette::WindowText);
    }
    painter->setPen(Qt::transparent);
    painter->drawRect(option.rect);
    painter->setBrush(Qt::transparent);

    QRect offsetRect = option.rect;
    offsetRect.setLeft(option.rect.left() + 6);
    offsetRect.setWidth(option.fontMetrics.width(QStringLiteral("UTC+12:00")));
    painter->setPen(offsetPen);
    painter->setFont(option.font);
    if (index.data(Qt::UserRole + 3).toBool()) painter->drawText(offsetRect, Qt::AlignRight | Qt::AlignVCenter, index.data(Qt::UserRole + 1).toString());

    QString mainText = index.data(Qt::DisplayRole).toString();
    QRect textRect = option.rect;
    textRect.setLeft(offsetRect.right() + 6);
    textRect.setWidth(option.fontMetrics.width(mainText) + 1);
    painter->setPen(textPen);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, mainText);

    QRect zoneName = option.rect;
    zoneName.setLeft(textRect.right() + 6);
    painter->setPen(offsetPen);
    painter->drawText(zoneName, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::UserRole + 2).toString());
}
