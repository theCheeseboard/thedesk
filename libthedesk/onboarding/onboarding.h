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
#ifndef ONBOARDING_H
#define ONBOARDING_H

#include <QDialog>

namespace Ui {
    class Onboarding;
}

struct OnboardingPrivate;
class Onboarding : public QDialog {
        Q_OBJECT

    public:
        explicit Onboarding(QWidget* parent = nullptr);
        ~Onboarding();

        void startOnboarding();

        void writeAudio();

    private slots:
        void on_stackedWidget_currentChanged(int arg1);

        void on_bar_closeClicked();

    private:
        Ui::Onboarding* ui;
        OnboardingPrivate* d;

        void resizeEvent(QResizeEvent* event);
        void paintEvent(QPaintEvent* event);
        void changeEvent(QEvent* event);
        void keyPressEvent(QKeyEvent* event);
        bool eventFilter(QObject* watched, QEvent* event);

        void completeOnboarding();

        void showBar();
        void hideBar();
        void updateBarVisiblity();
};

#endif // ONBOARDING_H
