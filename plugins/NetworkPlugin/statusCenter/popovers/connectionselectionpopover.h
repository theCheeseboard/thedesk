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
#ifndef CONNECTIONSELECTIONPOPOVER_H
#define CONNECTIONSELECTIONPOPOVER_H

#include <QWidget>
#include <NetworkManagerQt/Connection>

namespace Ui {
    class ConnectionSelectionPopover;
}

class QListWidgetItem;
struct ConnectionSelectionPopoverPrivate;
class ConnectionSelectionPopover : public QWidget {
        Q_OBJECT

    public:
        explicit ConnectionSelectionPopover(NetworkManager::Connection::List connectionList, QWidget* parent = nullptr);
        ~ConnectionSelectionPopover();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_connectionList_itemActivated(QListWidgetItem* item);

    signals:
        void reject();
        void accept(NetworkManager::Connection::Ptr connection);

    private:
        Ui::ConnectionSelectionPopover* ui;
        ConnectionSelectionPopoverPrivate* d;
};

#endif // CONNECTIONSELECTIONPOPOVER_H
