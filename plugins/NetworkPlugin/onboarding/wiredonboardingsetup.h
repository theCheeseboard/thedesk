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
#ifndef WIREDONBOARDINGSETUP_H
#define WIREDONBOARDINGSETUP_H

#include <QWidget>

namespace Ui {
    class WiredOnboardingSetup;
}

struct WiredOnboardingSetupPrivate;

class WiredOnboardingSetup : public QWidget {
        Q_OBJECT

    public:
        explicit WiredOnboardingSetup(QString device, QWidget* parent = nullptr);
        ~WiredOnboardingSetup();

    private slots:
        void on_connectButton_clicked();

    private:
        Ui::WiredOnboardingSetup* ui;
        WiredOnboardingSetupPrivate* d;

        void updateState();
};

#endif // WIREDONBOARDINGSETUP_H
