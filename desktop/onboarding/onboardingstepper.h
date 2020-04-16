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
#ifndef ONBOARDINGSTEPPER_H
#define ONBOARDINGSTEPPER_H

#include <QWidget>

struct OnboardingStepperPrivate;
class OnboardingStepper : public QWidget {
        Q_OBJECT
    public:
        explicit OnboardingStepper(QWidget* parent = nullptr);
        ~OnboardingStepper();

        void setText(QString text);
        void setStep(int step);
        void setCurrentStep(int step);
        void setIsFinal(bool isFinal);

        QSize sizeHint() const;

    signals:

    private:
        OnboardingStepperPrivate* d;

        void paintEvent(QPaintEvent* event);
};

#endif // ONBOARDINGSTEPPER_H
