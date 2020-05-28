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
#ifndef THEMESETTINGSPANE_H
#define THEMESETTINGSPANE_H

#include <statuscenterpane.h>

namespace Ui {
    class ThemeSettingsPane;
}

struct ThemeSettingsPanePrivate;
class ThemeSettingsPane : public StatusCenterPane {
        Q_OBJECT

    public:
        explicit ThemeSettingsPane();
        ~ThemeSettingsPane();

    private:
        Ui::ThemeSettingsPane* ui;
        ThemeSettingsPanePrivate* d;

        void changeEvent(QEvent* event);

        void updateBaseColour();
        void updateFonts();
        void updateWidgets();
        void setFonts();

        // StatusCenterPane interface
    public:
        QString name();
        QString displayName();
        QIcon icon();
        QWidget* leftPane();
    private slots:
        void on_baseColourComboBox_currentIndexChanged(int index);
        void on_titleLabel_backButtonClicked();
        void on_fixedFont_currentFontChanged(const QFont& f);
        void on_fixedFontSize_valueChanged(double arg1);
        void on_interfaceFont_currentFontChanged(const QFont& f);
        void on_interfaceFontSize_valueChanged(double arg1);
        void on_widgetThemeBox_currentIndexChanged(int index);
};

#endif // THEMESETTINGSPANE_H
