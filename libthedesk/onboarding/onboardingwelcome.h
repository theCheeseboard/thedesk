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
#ifndef ONBOARDINGWELCOME_H
#define ONBOARDINGWELCOME_H

#include <onboardingpage.h>

namespace Ui {
    class OnboardingWelcome;
}

struct OnboardingWelcomePrivate;
class OnboardingWelcome : public OnboardingPage {
        Q_OBJECT

    public:
        explicit OnboardingWelcome(QWidget* parent = nullptr);
        ~OnboardingWelcome();

    private:
        Ui::OnboardingWelcome* ui;
        OnboardingWelcomePrivate* d;

        void changeEvent(QEvent* event);

        // OnboardingPage interface
    public:
        QString name();
        QString displayName();
    private slots:
        void on_nextButton_clicked();
        void on_languageButton_clicked();
        void on_keyboardButton_clicked();

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // ONBOARDINGWELCOME_H
