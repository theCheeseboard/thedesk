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
#ifndef CELLULARPANE_H
#define CELLULARPANE_H

#include <QWidget>
#include "abstractdevicepane.h"

namespace Ui {
    class CellularPane;
}

struct CellularPanePrivate;
class CellularPane : public AbstractDevicePane {
        Q_OBJECT

    public:
        explicit CellularPane(QString uni, QWidget* parent = nullptr);
        ~CellularPane();

        void updateState();

        QString operatorName();

    private:
        Ui::CellularPane* ui;
        CellularPanePrivate* d;

        // AbstractDevicePane interface
    public:
        QListWidgetItem* leftPaneItem();
    private slots:
        void on_disconnectButton_clicked();
        void on_simSettingsButton_clicked();
};

#endif // CELLULARPANE_H
