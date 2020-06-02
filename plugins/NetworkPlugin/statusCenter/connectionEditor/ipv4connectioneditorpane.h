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
#ifndef IPV4CONNECTIONEDITORPANE_H
#define IPV4CONNECTIONEDITORPANE_H

#include "connectioneditorpane.h"

namespace Ui {
    class IPv4ConnectionEditorPane;
}

struct IPv4ConnectionEditorPanePrivate;
class IPv4ConnectionEditorPane : public ConnectionEditorPane {
        Q_OBJECT

    public:
        explicit IPv4ConnectionEditorPane(NetworkManager::Setting::Ptr setting, QWidget* parent = nullptr);
        ~IPv4ConnectionEditorPane();

    private:
        Ui::IPv4ConnectionEditorPane* ui;
        IPv4ConnectionEditorPanePrivate* d;

        void updateDns();
        void setAddresses();

        // ConnectionEditorPane interface
    public:
        QString displayName();
        void reload(NetworkManager::Setting::Ptr setting);
        NetworkManager::Setting::SettingType type();
        bool prepareSave();

    private slots:
        void on_configurationMethod_currentIndexChanged(int index);
        void on_requireIPv4Switch_toggled(bool checked);
        void on_addDnsServerButton_clicked();
        void on_dnsList_customContextMenuRequested(const QPoint& pos);
        void on_ipAddress_editingFinished();
        void on_ipSubnet_valueChanged(int arg1);
        void on_ipGateway_editingFinished();
};

#endif // IPV4CONNECTIONEDITORPANE_H
