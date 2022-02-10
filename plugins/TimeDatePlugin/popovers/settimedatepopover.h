/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#ifndef SETTIMEDATEPOPOVER_H
#define SETTIMEDATEPOPOVER_H

#include <QWidget>

namespace Ui {
    class SetTimeDatePopover;
}

struct SetTimeDatePopoverPrivate;
class SetTimeDatePopover : public QWidget {
        Q_OBJECT

    public:
        explicit SetTimeDatePopover(QWidget* parent = nullptr);
        ~SetTimeDatePopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_applyButton_clicked();

    private:
        Ui::SetTimeDatePopover* ui;
        SetTimeDatePopoverPrivate* d;
};

#endif // SETTIMEDATEPOPOVER_H
