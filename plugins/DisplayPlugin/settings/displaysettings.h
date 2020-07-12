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
#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

#include <QWidget>
#include <statuscenterpane.h>

namespace Ui {
    class DisplaySettings;
}

struct DisplaySettingsPrivate;
class DisplaySettings : public StatusCenterPane {
        Q_OBJECT

    public:
        explicit DisplaySettings();
        ~DisplaySettings();

    private:
        Ui::DisplaySettings* ui;
        DisplaySettingsPrivate* d;

        void updateSettings();

        // StatusCenterPane interface
    public:
        QString name();
        QString displayName();
        QIcon icon();
        QWidget* leftPane();

    private slots:
        void on_titleLabel_backButtonClicked();
        void on_scheduleRedshiftSwitch_toggled(bool checked);
        void on_followSunlightSwitch_toggled(bool checked);
        void on_redshiftStartTime_userTimeChanged(const QTime& time);
        void on_redshiftEndTime_userTimeChanged(const QTime& time);
        void on_redshiftIntensitySlider_valueChanged(int value);
        void on_arrangeButton_clicked();
};

#endif // DISPLAYSETTINGS_H
