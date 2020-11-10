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
#ifndef SIMSETTINGSPOPOVER_H
#define SIMSETTINGSPOPOVER_H

#include <QWidget>
#include <ModemDevice>

namespace Ui {
    class SimSettingsPopover;
}

struct SimSettingsPopoverPrivate;
class SimSettingsPopover : public QWidget {
        Q_OBJECT

    public:
        explicit SimSettingsPopover(ModemManager::ModemDevice::Ptr modem, QWidget* parent = nullptr);
        ~SimSettingsPopover();

    private slots:
        void on_currentSimPinAcceptButton_clicked();

        void on_titleLabel_backButtonClicked();

        void on_enableSimPinButton_clicked();

        void on_disableSimPinButton_clicked();

        void on_currentPinTitleLabel_backButtonClicked();

        void on_changeSimPinTitleLabel_backButtonClicked();

        void on_changeSimPinAcceptButton_clicked();

        void on_changeSimPinButton_clicked();

        void on_enableCallWaitingSwitch_toggled(bool checked);

        void on_callWaitingTitleLabel_backButtonClicked();

        void on_callWaitingButton_clicked();

    signals:
        void dismissed();

    private:
        Ui::SimSettingsPopover* ui;
        SimSettingsPopoverPrivate* d;

        void prepareCurrentPinPage();
};

#endif // SIMSETTINGSPOPOVER_H
