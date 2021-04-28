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
#ifndef MOUSESETTINGS_H
#define MOUSESETTINGS_H

#include <QWidget>

namespace Ui {
    class MouseSettings;
}

struct MouseSettingsPrivate;
class MouseSettings : public QWidget {
        Q_OBJECT

    public:
        explicit MouseSettings(QWidget* parent = nullptr);
        ~MouseSettings();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_leftPrimaryButton_toggled(bool checked);

        void on_rightPrimaryButton_toggled(bool checked);

        void on_tapToClickSwitch_toggled(bool checked);

        void on_naturalScrollingSwitch_toggled(bool checked);

    private:
        Ui::MouseSettings* ui;
        MouseSettingsPrivate* d;

        void updateSettings(QString key);
        void updateAllSettings();
};

#endif // MOUSESETTINGS_H
