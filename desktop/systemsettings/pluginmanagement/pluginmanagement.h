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
#ifndef PLUIGINMANAGEMENT_H
#define PLUIGINMANAGEMENT_H

#include <statuscenterpane.h>

namespace Ui {
    class PluginManagement;
}

struct PluginManagementPrivate;
class PluginManagement : public StatusCenterPane {
        Q_OBJECT

    public:
        explicit PluginManagement();
        ~PluginManagement();

    private:
        Ui::PluginManagement* ui;
        PluginManagementPrivate* d;

        void changeEvent(QEvent* event);

        // StatusCenterPane interface
    public:
        QString name();
        QString displayName();
        QIcon icon();
        QWidget* leftPane();
    private slots:
        void on_pluginsList_activated(const QModelIndex& index);
        void on_titleLabel_backButtonClicked();
};

#endif // PLUIGINMANAGEMENT_H
