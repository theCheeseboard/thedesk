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
#ifndef BTAGENT_H
#define BTAGENT_H

#include <BluezQt/Agent>

struct BtAgentPrivate;
class PairPopover;
class BtAgent : public BluezQt::Agent {
        Q_OBJECT
    public:
        explicit BtAgent(QObject* parent = nullptr);
        ~BtAgent();

        void capturePairRequests(BluezQt::DevicePtr device, PairPopover* popover);

    signals:

    private:
        BtAgentPrivate* d;

        // Agent interface
    public:
        QDBusObjectPath objectPath() const;
        Capability capability() const;
        void requestPinCode(BluezQt::DevicePtr device, const BluezQt::Request<QString>& request);
        void displayPinCode(BluezQt::DevicePtr device, const QString& pinCode);
        void requestPasskey(BluezQt::DevicePtr device, const BluezQt::Request<quint32>& request);
        void displayPasskey(BluezQt::DevicePtr device, const QString& passkey, const QString& entered);
        void requestConfirmation(BluezQt::DevicePtr device, const QString& passkey, const BluezQt::Request<>& request);
        void requestAuthorization(BluezQt::DevicePtr device, const BluezQt::Request<>& request);
        void authorizeService(BluezQt::DevicePtr device, const QString& uuid, const BluezQt::Request<>& request);
        void cancel();
};

#endif // BTAGENT_H
