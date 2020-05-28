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
#ifndef NETWORKCONNECTIONEDITOR_H
#define NETWORKCONNECTIONEDITOR_H

#include <QWidget>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Connection>

namespace Ui {
    class NetworkConnectionEditor;
}

struct NetworkConnectionEditorPrivate;
class NetworkConnectionEditor : public QWidget {
        Q_OBJECT

    public:
        explicit NetworkConnectionEditor(NetworkManager::ConnectionSettings::Ptr connectionSettings, QWidget* parent = nullptr);
        explicit NetworkConnectionEditor(NetworkManager::Connection::Ptr connection, QWidget* parent = nullptr);
        ~NetworkConnectionEditor();

        static int preferredPopoverWidth();

        void setSaveButtonText(QString text);

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_connectionName_textChanged(const QString& arg1);

        void on_saveButton_clicked();

    signals:
        void rejected();
        void accepted(NetworkManager::Connection::Ptr connection);

    private:
        Ui::NetworkConnectionEditor* ui;
        NetworkConnectionEditorPrivate* d;

        void init();
        void populate();
};

#endif // NETWORKCONNECTIONEDITOR_H
