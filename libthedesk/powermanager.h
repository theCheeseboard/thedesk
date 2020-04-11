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
#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <QObject>

class EndSession;
struct PowerManagerPrivate;
class PowerManager : public QObject {
        Q_OBJECT
    public:
        enum PowerOperation {
            PowerOff,
            Reboot,
            LogOut,
            Suspend,
            Lock,
            TurnOffScreen,
            Hibernate,
            All
        };

        explicit PowerManager(QObject* parent = nullptr);
        ~PowerManager();

        void showPowerOffConfirmation(PowerOperation operation = All);

    signals:
        void powerOffConfirmationRequested(PowerOperation operation);

    protected:
        friend EndSession;
        void performPowerOperation(PowerOperation operation);

    private:
        PowerManagerPrivate* d;
};

#endif // POWERMANAGER_H
