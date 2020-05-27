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
#ifndef SECURITYEAPPEAP_H
#define SECURITYEAPPEAP_H

#include "securityeap.h"

namespace Ui {
    class SecurityEapPeap;
}

struct SecurityEapPeapPrivate;
class SecurityEapPeap : public SecurityEap {
        Q_OBJECT

    public:
        explicit SecurityEapPeap(QWidget* parent = nullptr);
        ~SecurityEapPeap();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_connectButton_clicked();

        void on_browseCaCertButton_clicked();

    private:
        Ui::SecurityEapPeap* ui;
        SecurityEapPeapPrivate* d;

        // SecurityEap interface
    public:
        void populateSetting(NetworkManager::Security8021xSetting::Ptr setting);
};

#endif // SECURITYEAPPEAP_H
