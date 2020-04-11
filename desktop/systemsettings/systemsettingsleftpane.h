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
#ifndef SYSTEMSETTINGSLEFTPANE_H
#define SYSTEMSETTINGSLEFTPANE_H

#include <QWidget>

namespace Ui {
    class SystemSettingsLeftPane;
}

class QListWidgetItem;
class SystemSettingsLeftPane : public QWidget {
        Q_OBJECT

    public:
        explicit SystemSettingsLeftPane(QWidget* parent = nullptr);
        ~SystemSettingsLeftPane();

        void appendItem(QListWidgetItem* item);
        void insertItem(int index, QListWidgetItem* item);
        void removeItem(QListWidgetItem* item);

    signals:
        void indexChanged(int index);

    private slots:
        void on_titleLabel_backButtonClicked();

    private:
        Ui::SystemSettingsLeftPane* ui;
};

#endif // SYSTEMSETTINGSLEFTPANE_H
