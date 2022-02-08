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
#ifndef ONBOARDINGMANAGER_H
#define ONBOARDINGMANAGER_H

#include <QObject>

class Onboarding;
class OnboardingController;
class OnboardingPage;
struct OnboardingManagerPrivate;
class OnboardingManager : public QObject {
        Q_OBJECT
    public:
        explicit OnboardingManager(QObject* parent = nullptr);
        ~OnboardingManager();

        void addOnboardingStep(OnboardingPage* onboardingStep);
        bool isOnboardingRunning();

        void setDateVisible(bool dateVisible);
        bool dateVisible();

        bool isSystemOnboarding();

    signals:
        void onboardingRequired();

        //Call these functions to step forwards/backwards in the onboarding
        void nextStep();
        void previousStep();

        void dateVisibleChanged(bool dateVisible);

    protected:
        friend Onboarding;
        friend OnboardingController;

        OnboardingManagerPrivate* d;
        QList<OnboardingPage*> onboardingSteps();
        void setIsSystemOnboarding(bool isSystemOnboarding);
};

#endif // ONBOARDINGMANAGER_H
