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
#ifndef POLKITINTERFACE_H
#define POLKITINTERFACE_H

#include <QObject>
#include <PolkitQt1/Agent/Listener>
#include <PolkitQt1/Identity>
#include <PolkitQt1/Subject>

struct PolkitInterfacePrivate;
class PolkitInterface : public PolkitQt1::Agent::Listener {
        Q_OBJECT

    public:
        explicit PolkitInterface();
        ~PolkitInterface();

    signals:

    private:
        PolkitInterfacePrivate* d;

        // Listener interface
    public slots:
        void initiateAuthentication(const QString& actionId, const QString& message, const QString& iconName, const PolkitQt1::Details& details, const QString& cookie, const PolkitQt1::Identity::List& identities, PolkitQt1::Agent::AsyncResult* result);
        bool initiateAuthenticationFinish();
        void cancelAuthentication();
};

#endif // POLKITINTERFACE_H
