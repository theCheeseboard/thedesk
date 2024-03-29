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
#ifndef UNLOCKMODEMPOPOVER_H
#define UNLOCKMODEMPOPOVER_H

#include <ModemManagerQt/ModemDevice>
#include <QWidget>

namespace Ui {
    class UnlockModemPopover;
}

struct UnlockModemPopoverPrivate;
class UnlockModemPopover : public QWidget {
        Q_OBJECT

    public:
        explicit UnlockModemPopover(ModemManager::ModemDevice::Ptr modem, QWidget* parent = nullptr);
        ~UnlockModemPopover();

    signals:
        void done();

    private slots:
        void on_simPinAcceptButton_clicked();

        void on_simPinTitleLabel_backButtonClicked();

        void on_simPukAcceptButton_clicked();

        void on_simPukTitleLabel_backButtonClicked();

    private:
        Ui::UnlockModemPopover* ui;
        UnlockModemPopoverPrivate* d;

        void updatePage();
};

#endif // UNLOCKMODEMPOPOVER_H
