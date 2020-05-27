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
#ifndef WIRELESSNETWORKSELECTIONPOPOVER_H
#define WIRELESSNETWORKSELECTIONPOPOVER_H

#include <QWidget>

namespace Ui {
    class WirelessNetworkSelectionPopover;
}

struct WirelessNetworkSelectionPopoverPrivate;
class WirelessNetworkSelectionPopover : public QWidget {
        Q_OBJECT

    public:
        explicit WirelessNetworkSelectionPopover(QString deviceUni, QWidget* parent = nullptr);
        ~WirelessNetworkSelectionPopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_knownNetworksListView_activated(const QModelIndex& index);

        void on_knownNetworksListView_customContextMenuRequested(const QPoint& pos);

        void on_keyTitleLabel_backButtonClicked();

        void on_newNetworksListView_activated(const QModelIndex& index);

        void on_securityConnectButton_clicked();

        void on_eapMethodTitleLabel_backButtonClicked();

        void on_eapMethodList_activated(const QModelIndex& index);

    private:
        Ui::WirelessNetworkSelectionPopover* ui;
        WirelessNetworkSelectionPopoverPrivate* d;

        void activateConnection(const QModelIndex& index);
        void createConnection();
};

#endif // WIRELESSNETWORKSELECTIONPOPOVER_H
