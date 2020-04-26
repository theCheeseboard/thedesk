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
#include "powermanager.h"

#include <QApplication>
#include <QPointer>
#include <QProcess>
#include <QDBusMessage>
#include <QDBusConnection>

#include "keygrab.h"
#include <QKeySequence>
#include <Wm/desktopwm.h>

#include "statemanager.h"
#include "onboardingmanager.h"

struct PowerManagerPrivate {
    QPointer<QProcess> lockScreenProcess;
};

PowerManager::PowerManager(QObject* parent) : QObject(parent) {
    d = new PowerManagerPrivate();

    connect(new KeyGrab(QKeySequence(Qt::Key_L | Qt::MetaModifier), "lockScreen"), &KeyGrab::activated, this, [ = ] {
        this->performPowerOperation(PowerManager::Lock);
    });
}

PowerManager::~PowerManager() {
    delete d;
}

tPromise<void>* PowerManager::showPowerOffConfirmation(PowerManager::PowerOperation operation, QString message) {
    return tPromise<void>::runOnSameThread([ = ](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        Q_UNUSED(rej)

        emit powerOffConfirmationRequested(operation, message, res);
    });
}

void PowerManager::performPowerOperation(PowerManager::PowerOperation operation) {
    emit powerOffOperationCommencing(operation);

    switch (operation) {
        case PowerManager::PowerOff:
        case PowerManager::Reboot:
        case PowerManager::Suspend:
        case PowerManager::Hibernate: {
            QMap<PowerManager::PowerOperation, QString> methods = {
                {PowerManager::PowerOff, "PowerOff"},
                {PowerManager::Reboot, "Reboot"},
                {PowerManager::Suspend, "Suspend"},
                {PowerManager::Hibernate, "Hibernate"}
            };

            QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", methods.value(operation));
            message.setArguments({true});
            QDBusConnection::systemBus().send(message);
            break;
        }
        case PowerManager::LogOut:
            //Quit theDesk
            qApp->exit();
            break;
        case PowerManager::Lock:
            //Lock the screen
            if (d->lockScreenProcess) return; //Screen is already locked
            if (StateManager::onboardingManager()->isOnboardingRunning()) return; //Onboarding is currently running

            d->lockScreenProcess = new QProcess();
            connect(d->lockScreenProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [ = ] {
                d->lockScreenProcess->deleteLater();
            });
            d->lockScreenProcess->start("/usr/lib/tsscreenlock"); //Lock Screen
            break;
        case PowerManager::TurnOffScreen:
            DesktopWm::setScreenOff(true);
            break;
        case PowerManager::All:
            //Doesn't make sense ???
            break;
    }
}
