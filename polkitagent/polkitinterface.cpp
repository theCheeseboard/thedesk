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
#include "polkitinterface.h"

#include <PolkitQt1/Details>
#include <QPointer>
#include <authwindow.h>

struct PolkitInterfacePrivate {
    QPointer<AuthWindow> authWin;
};

PolkitInterface::PolkitInterface() : PolkitQt1::Agent::Listener() {
    d = new PolkitInterfacePrivate();
}

PolkitInterface::~PolkitInterface() {
    delete d;
}

void PolkitInterface::initiateAuthentication(const QString& actionId, const QString& message, const QString& iconName, const PolkitQt1::Details& details, const QString& cookie, const PolkitQt1::Identity::List& identities, PolkitQt1::Agent::AsyncResult* result) {
    d->authWin = new AuthWindow();
    d->authWin->setMessage(message);
    d->authWin->setIdentities(identities);
    d->authWin->setCookie(cookie);
    d->authWin->setCallback(result);
    d->authWin->showAuthWindow();
}

bool PolkitInterface::initiateAuthenticationFinish() {
    return true;
}

void PolkitInterface::cancelAuthentication() {
    if (d->authWin) {
        d->authWin->cancel();
    }
}
