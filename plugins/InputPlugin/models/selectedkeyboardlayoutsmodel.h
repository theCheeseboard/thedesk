/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#ifndef SELECTEDKEYBOARDLAYOUTSMODEL_H
#define SELECTEDKEYBOARDLAYOUTSMODEL_H

#include <QAbstractListModel>

struct SelectedKeyboardLayoutsModelPrivate;
class SelectedKeyboardLayoutsModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit SelectedKeyboardLayoutsModel(QObject* parent = nullptr);
        ~SelectedKeyboardLayoutsModel();

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        void addLayout(QString layout);
        void moveUp(QString layout);
        void moveDown(QString layout);
        void removeLayout(QString layout);

    private:
        SelectedKeyboardLayoutsModelPrivate* d;
};

#endif // SELECTEDKEYBOARDLAYOUTSMODEL_H
