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
#ifndef TIMEZONESMODEL_H
#define TIMEZONESMODEL_H

#include <QAbstractListModel>
#include <QStyledItemDelegate>

struct TimezonesModelPrivate;
class QTimeZone;
class TimezonesModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit TimezonesModel(QObject* parent = nullptr);
        ~TimezonesModel();

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        QModelIndex timezone(QTimeZone timezone);

        void search(QString query);

    private:
        TimezonesModelPrivate* d;
};

class TimezonesModelDelegate : public QStyledItemDelegate {
        Q_OBJECT

    public:
        explicit TimezonesModelDelegate(QObject* parent = nullptr);
        ~TimezonesModelDelegate();

        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // TIMEZONESMODEL_H
