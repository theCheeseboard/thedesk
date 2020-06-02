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
#include "wificonnectioneditorpane.h"
#include "ui_wificonnectioneditorpane.h"

#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessSetting>
#include <QtConcurrent>

struct WifiConnectionEditorPanePrivate {
    NetworkManager::WirelessSetting::Ptr setting;
};

WifiConnectionEditorPane::WifiConnectionEditorPane(NetworkManager::Setting::Ptr setting, QWidget* parent) :
    ConnectionEditorPane(setting, parent),
    ui(new Ui::WifiConnectionEditorPane) {
    ui->setupUi(this);

    d = new WifiConnectionEditorPanePrivate();

    this->reload(setting);
}

WifiConnectionEditorPane::~WifiConnectionEditorPane() {
    delete ui;
    delete d;
}

QString WifiConnectionEditorPane::displayName() {
    return tr("Wi-Fi");
}

void WifiConnectionEditorPane::reload(NetworkManager::Setting::Ptr setting) {
    d->setting = setting.staticCast<NetworkManager::WirelessSetting>();
    d->setting->setInitialized(true);
    ui->ssidLineEdit->setText(d->setting->ssid());
    ui->modeBox->setCurrentIndex(d->setting->mode());
}

NetworkManager::Setting::SettingType WifiConnectionEditorPane::type() {
    return NetworkManager::Setting::Wireless;
}

void WifiConnectionEditorPane::on_ssidLineEdit_textChanged(const QString& arg1) {
    d->setting->setSsid(arg1.toUtf8());

    QString newId = arg1;
    int number = 0;
    bool loopAgain = true;

    auto name = [ & ] {
        if (number == 0) return arg1;
        return newId + " " + QString::number(number);
    };

    NetworkManager::Connection::List connections = NetworkManager::listConnections();
    while (loopAgain) {
        loopAgain = false;
        for (NetworkManager::Connection::Ptr connection : connections) {
            if (connection->settings()->id() == name()) {
                number++;
                loopAgain = true;
            }
        }
    }
    emit setConnectionName(name());
    emit changed();
}

void WifiConnectionEditorPane::on_modeBox_currentIndexChanged(int index) {
    d->setting->setMode(static_cast<NetworkManager::WirelessSetting::NetworkMode>(index));
    emit changed();
}
