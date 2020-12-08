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
#include "common.h"

#include <Modem3Gpp>

QString Common::stateChangeReasonToString(NetworkManager::Device::StateChangeReason reason) {

    switch (reason) {
        case NetworkManager::Device::NowUnmanagedReason:
            return tr("the device is no longer managed by NetworkManager");
        case NetworkManager::Device::ConfigFailedReason:
            return tr("configuration of the device failed");
        case NetworkManager::Device::ConfigUnavailableReason:
            return tr("configuration of the device is unavailable");
        case NetworkManager::Device::ConfigExpiredReason:
            return tr("configuration of the device expired");
        case NetworkManager::Device::NoSecretsReason:
            return tr("required secrets were not provided");
        case NetworkManager::Device::AuthSupplicantDisconnectReason:
            return tr("the authentication supplicant disconnected");
        case NetworkManager::Device::AuthSupplicantConfigFailedReason:
            return tr("configuration of the authentication supplicant failed");
        case NetworkManager::Device::AuthSupplicantFailedReason:
            return tr("the authentication supplicant failed");
        case NetworkManager::Device::AuthSupplicantTimeoutReason:
            return tr("the authentication supplicant timed out");
        case NetworkManager::Device::PppStartFailedReason:
            return tr("the PPP network failed to start");
        case NetworkManager::Device::PppDisconnectReason:
            return tr("the PPP network was disconnected");
        case NetworkManager::Device::PppFailedReason:
            return tr("the PPP network failed");
        case NetworkManager::Device::DhcpStartFailedReason:
            return tr("DHCP configuration failed to start");
        case NetworkManager::Device::DhcpErrorReason:
            return tr("DHCP configuration had an error");
        case NetworkManager::Device::DhcpFailedReason:
            return tr("DHCP configuration failed");
        case NetworkManager::Device::SharedStartFailedReason:
            break;
        case NetworkManager::Device::SharedFailedReason:
            break;
        case NetworkManager::Device::AutoIpStartFailedReason:
            break;
        case NetworkManager::Device::AutoIpErrorReason:
            break;
        case NetworkManager::Device::AutoIpFailedReason:
            break;
        case NetworkManager::Device::ModemBusyReason:
            return tr("the modem is busy");
        case NetworkManager::Device::ModemNoDialToneReason:
            return tr("there is no dial tone on the modem");
        case NetworkManager::Device::ModemNoCarrierReason:
            return tr("no carrier is available on the modem");
        case NetworkManager::Device::ModemDialTimeoutReason:
            return tr("dialling the number on the modem timed out");
        case NetworkManager::Device::ModemDialFailedReason:
            return tr("dialling the number on the modem failed");
        case NetworkManager::Device::ModemInitFailedReason:
            return tr("initialising the modem failed");
        case NetworkManager::Device::GsmApnSelectFailedReason:
            return tr("APN selection failed");
        case NetworkManager::Device::GsmNotSearchingReason:
            return tr("the modem is not searching");
        case NetworkManager::Device::GsmRegistrationDeniedReason:
            return tr("the cell tower denied registration of the modem");
        case NetworkManager::Device::GsmRegistrationTimeoutReason:
            return tr("registration of the modem on the cell tower timed out");
        case NetworkManager::Device::GsmRegistrationFailedReason:
            return tr("registration of the modem on the cell tower failed");
        case NetworkManager::Device::GsmPinCheckFailedReason:
            break;
        case NetworkManager::Device::FirmwareMissingReason:
            return tr("the device firmware is missing");
        case NetworkManager::Device::DeviceRemovedReason:
            return tr("the device was removed");
        case NetworkManager::Device::SleepingReason:
            return tr("the device is sleeping");
        case NetworkManager::Device::ConnectionRemovedReason:
            return tr("the connection was removed");
        case NetworkManager::Device::UserRequestedReason:
            return tr("you requested the action");
        case NetworkManager::Device::CarrierReason:
            break;
        case NetworkManager::Device::ConnectionAssumedReason:
            break;
        case NetworkManager::Device::SupplicantAvailableReason:
            break;
        case NetworkManager::Device::ModemNotFoundReason:
            return tr("the modem was not found");
        case NetworkManager::Device::BluetoothFailedReason:
            return tr("Bluetooth is not available");
        case NetworkManager::Device::GsmSimNotInserted:
            return tr("the SIM card is not inserted");
        case NetworkManager::Device::GsmSimPinRequired:
            return tr("the SIM PIN is required");
        case NetworkManager::Device::GsmSimPukRequired:
            return tr("the SIM PUK is required");
        case NetworkManager::Device::GsmSimWrong:
        case NetworkManager::Device::SimPinIncorrect:
            return tr("the SIM PUK is exhausted");
        case NetworkManager::Device::InfiniBandMode:
            break;
        case NetworkManager::Device::DependencyFailed:
            break;
        case NetworkManager::Device::Br2684Failed:
            break;
        case NetworkManager::Device::ModemManagerUnavailable:
            return tr("ModemManager is not operational on this device");
        case NetworkManager::Device::SsidNotFound:
            return tr("the wireless network wasn't found");
        case NetworkManager::Device::SecondaryConnectionFailed:
            break;
        case NetworkManager::Device::DcbFcoeFailed:
            break;
        case NetworkManager::Device::TeamdControlFailed:
            break;
        case NetworkManager::Device::ModemFailed:
            break;
        case NetworkManager::Device::ModemAvailable:
            break;
        case NetworkManager::Device::NewActivation:
            break;
        case NetworkManager::Device::ParentChanged:
            break;
        case NetworkManager::Device::ParentManagedChanged:
            break;
        case NetworkManager::Device::Reserved:
        case NetworkManager::Device::UnknownReason:
        case NetworkManager::Device::NoReason:
        case NetworkManager::Device::NowManagedReason:
            break;
    }

    return tr("an unknown error occurred");
}

QString Common::iconForSignalStrength(int strength, Common::WirelessType type) {
    QString iconString = "network-";

    switch (type) {
        case Common::WiFi:
            iconString += "wireless-connected-";
            break;
        case Common::WiFiError:
            iconString += "wireless-error-";
            break;
        case Common::Cellular:
            iconString += "cellular-connected-";
            break;
        case Common::CellularError:
            iconString += "cellular-error-";
            break;
    }

    if (strength < 15) {
        iconString += "00";
    } else if (strength < 35) {
        iconString += "25";
    } else if (strength < 65) {
        iconString += "50";
    } else if (strength < 85) {
        iconString += "75";
    } else {
        iconString += "100";
    }

    return iconString;
}

QString Common::operatorNameForModem(ModemManager::ModemDevice::Ptr device) {
    if (device->sim() && !device->sim()->operatorName().isEmpty()) {
        return device->sim()->operatorName();
    }

    ModemManager::Modem3gpp::Ptr modem3gpp(new ModemManager::Modem3gpp(device->uni()));
    if (!modem3gpp->operatorName().isEmpty()) {
        return modem3gpp->operatorName();
    }

    return tr("Cellular");
}
