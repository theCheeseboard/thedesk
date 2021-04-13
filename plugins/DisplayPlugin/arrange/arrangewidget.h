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
#ifndef ARRANGEWIDGET_H
#define ARRANGEWIDGET_H

#include <QWidget>

namespace Ui {
    class ArrangeWidget;
}

class SystemScreen;
struct ArrangeWidgetPrivate;
class ArrangeWidget : public QWidget {
        Q_OBJECT

    public:
        explicit ArrangeWidget(SystemScreen* screen, QWidget* parent = nullptr);
        ~ArrangeWidget();

        void dismiss();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_refreshRateBox_currentIndexChanged(int index);

        void on_resolutionBox_currentIndexChanged(int index);

        void on_applyButton_clicked();

        void on_primaryDisplaySwitch_toggled(bool checked);

        void on_orientationBox_currentIndexChanged(int index);

        void on_enableDisplaySwitch_toggled(bool checked);

    signals:
        void reject();
        void accept();

    private:
        Ui::ArrangeWidget* ui;
        ArrangeWidgetPrivate* d;

        void updateScreenGeometry();
        void updateAvailableModes();
        void updateRefreshRateBox();
        void updateOrientationBox();
        void updateIsPrimary();
        void updatePowered();

        void updateScreenList();
        void setScreen(SystemScreen* screen);
};

#endif // MAINARRANGEWIDGET_H
