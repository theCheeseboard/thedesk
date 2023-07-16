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
#include "onboardingcontroller.h"

#include "onboarding.h"
#include "server/sessionserver.h"
#include <QCoroCore>
#include <QScreen>
#include <onboardingmanager.h>
#include <statemanager.h>
#include <tsettings.h>

#include "onboardingfinal.h"
#include "onboardingvideo.h"
#include "onboardingwelcome.h"

OnboardingController::OnboardingController(QObject* parent) :
    QObject(parent) {
}

bool OnboardingController::performOnboarding(bool isSystemOnboarding) {
    tSettings settings;
    if (settings.value("Onboarding/lastOnboarding").toInt() < 1) {
        OnboardingManager* manager = StateManager::onboardingManager();
        manager->setIsSystemOnboarding(isSystemOnboarding);
        manager->addOnboardingStep(new OnboardingWelcome);
        manager->addOnboardingStep(new OnboardingFinal);
        emit manager->onboardingRequired();

        Onboarding o;

        QList<OnboardingVideo*> videoScreens;
        if (settings.value("Onboarding/onboardingVideo").toBool()) {
            for (QScreen* screen : qApp->screens()) {
                OnboardingVideo* video = new OnboardingVideo();
                video->setGeometry(screen->geometry());
                video->showFullScreen();
                videoScreens.append(video);
            }
        }

        o.setGeometry(qApp->primaryScreen()->geometry());
        o.showFullScreen();

        if (videoScreens.count() > 0) {
            connect(videoScreens.first(), &OnboardingVideo::startOnboarding, &o, &Onboarding::startOnboarding);
            connect(videoScreens.first(), &OnboardingVideo::playAudio, &o, &Onboarding::writeAudio);
        }

        // Hide the splashes if needed
        SessionServer::instance()->hideSplashes();

        auto result = QCoro::waitFor(qCoro(&o, &Onboarding::onboardingCompleted));
        for (OnboardingVideo* video : videoScreens) {
            video->close();
            video->deleteLater();
        }

        if (result) {
            // Show the splashes again
            SessionServer::instance()->showSplashes();

            // Set the onboarding flag
            settings.setValue("Onboarding/lastOnboarding", 1);
            settings.sync();

            return true;
        } else {
            return false;
        }
    }
    return true;
}
