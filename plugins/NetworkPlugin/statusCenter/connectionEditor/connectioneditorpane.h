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
#ifndef CONNECTIONEDITORPANE_H
#define CONNECTIONEDITORPANE_H

#include <QWidget>
#include <NetworkManagerQt/Setting>

class ConnectionEditorPane : public QWidget {
        Q_OBJECT
    public:
        explicit ConnectionEditorPane(NetworkManager::Setting::Ptr setting, QWidget* parent = nullptr);

        virtual QString displayName() = 0;
        virtual void reload(NetworkManager::Setting::Ptr setting) = 0;
        virtual NetworkManager::Setting::SettingType type() = 0;
        virtual bool prepareSave();

    signals:
        void setConnectionName(QString connectionName);
        void changed();
};

#endif // CONNECTIONEDITORPANE_H
