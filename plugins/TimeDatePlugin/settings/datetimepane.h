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
#ifndef DATETIMEPANE_H
#define DATETIMEPANE_H

#include <QVariantMap>
#include <statuscenterpane.h>

namespace Ui {
    class DateTimePane;
}

struct DateTimePanePrivate;
class DateTimePane : public StatusCenterPane {
        Q_OBJECT

    public:
        explicit DateTimePane();
        ~DateTimePane();

    private:
        Ui::DateTimePane* ui;
        DateTimePanePrivate* d;

        void forceUpdate();

        // StatusCenterPane interface
    public:
        QString name();
        QString displayName();
        QIcon icon();
        QWidget* leftPane();

    private slots:
        void on_titleLabel_backButtonClicked();
        void on_ntpSwitch_toggled(bool checked);
        void propertiesChanged(QString interfaceName, QVariantMap changedProperties, QStringList invalidatedProperties);
        void on_changeTimezoneButton_clicked();
};

#endif // DATETIMEPANE_H
