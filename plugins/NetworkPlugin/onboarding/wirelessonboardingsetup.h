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
#ifndef WIRELESSONBOARDINGSETUP_H
#define WIRELESSONBOARDINGSETUP_H

#include <QWidget>

namespace Ui {
    class WirelessOnboardingSetup;
}

struct WirelessOnboardingSetupPrivate;

class WirelessOnboardingSetup : public QWidget {
        Q_OBJECT

    public:
        explicit WirelessOnboardingSetup(QString device, QWidget* parent = nullptr);
        ~WirelessOnboardingSetup();

    private slots:
        void on_selectNetworkButton_clicked();

    private:
        Ui::WirelessOnboardingSetup* ui;
        WirelessOnboardingSetupPrivate* d;

        void updateState();
};

#endif // WIRELESSONBOARDINGSETUP_H
