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
#ifndef DEVICEPOPOVER_H
#define DEVICEPOPOVER_H

#include <QWidget>
#include <BluezQt/Device>

namespace Ui {
    class DevicePopover;
}

struct DevicePopoverPrivate;
class DevicePopover : public QWidget {
        Q_OBJECT

    public:
        explicit DevicePopover(BluezQt::DevicePtr device, QWidget* parent = nullptr);
        ~DevicePopover();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_removeButton_clicked();

        void on_connectButton_clicked();

        void on_disconnectButton_clicked();

    signals:
        void done();

    private:
        Ui::DevicePopover* ui;
        DevicePopoverPrivate* d;

        void updateDevice();
};

#endif // DEVICEPOPOVER_H
