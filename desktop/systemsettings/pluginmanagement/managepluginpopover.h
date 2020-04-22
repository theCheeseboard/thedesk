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
#ifndef MANAGEPLUGINPOPOVER_H
#define MANAGEPLUGINPOPOVER_H

#include <QWidget>

namespace Ui {
    class ManagePluginPopover;
}

struct ManagePluginPopoverPrivate;
class ManagePluginPopover : public QWidget {
        Q_OBJECT

    public:
        explicit ManagePluginPopover(QUuid uuid, QWidget* parent = nullptr);
        ~ManagePluginPopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_activateButton_clicked();

        void on_deactivateButton_clicked();

        void on_blacklistButton_clicked();

        void on_removeBlacklistButton_clicked();

        void on_performBlacklistButton_clicked();

        void on_blacklistTitle_backButtonClicked();

    private:
        Ui::ManagePluginPopover* ui;
        ManagePluginPopoverPrivate* d;

        void updateState();
};

#endif // MANAGEPLUGINPOPOVER_H
