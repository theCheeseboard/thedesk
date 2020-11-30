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
#ifndef ONBOARDINGUSERS_H
#define ONBOARDINGUSERS_H

#include <onboardingpage.h>

namespace Ui {
    class OnboardingUsers;
}

struct OnboardingUsersPrivate;
class OnboardingUsers : public OnboardingPage {
        Q_OBJECT

    public:
        explicit OnboardingUsers(QWidget* parent = nullptr);
        ~OnboardingUsers();

    private slots:
        void on_addUserTitleLabel_backButtonClicked();

        void on_addUserCompleteButton_clicked();

        void on_addUserButton_clicked();

        void on_nextButton_clicked();

        void on_titleLabel_backButtonClicked();

        void on_fullNameBox_textChanged(const QString& arg1);

        void on_administratorButton_clicked();

        void on_standardUserButton_clicked();

    private:
        Ui::OnboardingUsers* ui;
        OnboardingUsersPrivate* d;

        void resetAddUserForm();

        // OnboardingPage interface
    public:
        QString name();
        QString displayName();
};

#endif // ONBOARDINGUSERS_H
