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
#include <tpromise.h>

class EndSession;
class EventHandler;
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
            SwitchUsers,
            Hibernate,
            All
        };

        explicit PowerManager(QObject* parent = nullptr);
        ~PowerManager();

        tPromise<void>* showPowerOffConfirmation(PowerOperation operation = All, QString message = "", QStringList flags = {});

    private slots:
        void logindRequestLock();
        void logindRequestUnlock();

    signals:
        void powerOffConfirmationRequested(PowerOperation operation, QString message, QStringList flags, tPromiseFunctions<void>::SuccessFunction cb);
        void powerOffOperationCommencing(PowerOperation operation);

    protected:
        friend EndSession;
        friend EventHandler;
        void performPowerOperation(PowerOperation operation, QStringList flags = {});

    private:
        PowerManagerPrivate* d;
};

#endif // POWERMANAGER_H
