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
#ifndef WIFICONNECTIONEDITORPANE_H
#define WIFICONNECTIONEDITORPANE_H

#include "connectioneditorpane.h"

namespace Ui {
    class WifiConnectionEditorPane;
}

struct WifiConnectionEditorPanePrivate;
class WifiConnectionEditorPane : public ConnectionEditorPane {
        Q_OBJECT

    public:
        explicit WifiConnectionEditorPane(NetworkManager::Setting::Ptr setting, QWidget* parent = nullptr);
        ~WifiConnectionEditorPane();

    private:
        Ui::WifiConnectionEditorPane* ui;
        WifiConnectionEditorPanePrivate* d;

        // ConnectionEditorPane interface
    public:
        QString displayName();
    private slots:
        void on_ssidLineEdit_textChanged(const QString& arg1);
        void on_modeBox_currentIndexChanged(int index);
};

#endif // WIFICONNECTIONEDITORPANE_H
