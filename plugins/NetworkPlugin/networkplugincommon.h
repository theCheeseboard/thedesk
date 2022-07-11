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
#ifndef NETWORKPLUGINCOMMON_H
#define NETWORKPLUGINCOMMON_H

#include <ModemManagerQt/ModemDevice>
#include <NetworkManagerQt/Device>
#include <QCoreApplication>

class NetworkPluginCommon {
        Q_DECLARE_TR_FUNCTIONS(Common)
    public:
        enum WirelessType {
            WiFi,
            Cellular,
            WiFiError,
            CellularError
        };

        static QString stateChangeReasonToString(NetworkManager::Device::StateChangeReason reason);
        static QString iconForSignalStrength(int strength, WirelessType type);
        static QString operatorNameForModem(ModemManager::ModemDevice::Ptr device);

        static bool isDeviceConnecting(NetworkManager::Device::Ptr device);
};

#endif // NETWORKPLUGINCOMMON_H
