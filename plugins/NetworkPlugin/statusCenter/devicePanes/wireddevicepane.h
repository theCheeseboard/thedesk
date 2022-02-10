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
#ifndef WIREDDEVICEPANE_H
#define WIREDDEVICEPANE_H

#include "abstractdevicepane.h"

namespace Ui {
    class WiredDevicePane;
}

struct WiredDevicePanePrivate;
class WiredDevicePane : public AbstractDevicePane {
        Q_OBJECT

    public:
        explicit WiredDevicePane(QString uni, QWidget* parent = nullptr);
        ~WiredDevicePane();

    private:
        Ui::WiredDevicePane* ui;
        WiredDevicePanePrivate* d;

        void updateState();

        // AbstractDevicePane interface
    public:
        QListWidgetItem* leftPaneItem();

    private slots:
        void on_disconnectButton_clicked();
        void on_connectButton_clicked();
        void on_titleLabel_backButtonClicked();

        // QWidget interface
    protected:
        void changeEvent(QEvent* event);
};

#endif // WIREDDEVICEPANE_H
