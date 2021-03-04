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
#include "quietmodemanager.h"

#include "statemanager.h"
#include "onboardingmanager.h"

struct QuietModeManagerPrivate {
    QuietModeManager::QuietMode quietMode = QuietModeManager::Sound;
};

QuietModeManager::QuietModeManager(QObject* parent) : QObject(parent) {
    d = new QuietModeManagerPrivate();
}

QuietModeManager::~QuietModeManager() {
    delete d;
}

void QuietModeManager::setQuietMode(QuietModeManager::QuietMode quietMode) {
    QuietModeManager::QuietMode oldMode = d->quietMode;
    d->quietMode = quietMode;
    emit quietModeChanged(quietMode, oldMode);
}

QuietModeManager::QuietMode QuietModeManager::currentMode() {
    return d->quietMode;
}

QuietModeManager::QuietMode QuietModeManager::nextQuietMode() const {
    switch (d->quietMode) {
        case QuietModeManager::Sound:
            //If we're in onboarding, mute the audio if the user hits mute
            return StateManager::onboardingManager()->isOnboardingRunning() ? Mute : CriticalOnly;
        case QuietModeManager::CriticalOnly:
            //If we're in onboarding, mute the audio if the user hits mute
            return StateManager::onboardingManager()->isOnboardingRunning() ? Mute : NoNotifications;
        case QuietModeManager::NoNotifications:
            return Mute;
        case QuietModeManager::Mute:
            return Sound;
        default:
            return QuietModeManager::Sound;
    }
}

QString QuietModeManager::name(QuietModeManager::QuietMode quietMode) {
    switch (quietMode) {
        case QuietModeManager::Sound:
            return tr("Sound");
        case QuietModeManager::CriticalOnly:
            return tr("Critical Only");
        case QuietModeManager::NoNotifications:
            return tr("No Notifications");
        case QuietModeManager::Mute:
            return tr("Mute");
        default:
            return "";
    }
}

QString QuietModeManager::description(QuietModeManager::QuietMode quietMode) {
    switch (quietMode) {
        case QuietModeManager::Sound:
            return tr("Allows all sounds");
        case QuietModeManager::CriticalOnly:
            return tr("Hides non-critical notifications");
        case QuietModeManager::NoNotifications:
            return tr("Hides all notifications");
        case QuietModeManager::Mute:
            return tr("Mutes all sound altogether");
        default:
            return "";
    }
}

QString QuietModeManager::icon(QuietModeManager::QuietMode quietMode) {
    switch (quietMode) {
        case QuietModeManager::Sound:
            return "audio-volume-high";
        case QuietModeManager::CriticalOnly:
            return "quiet-mode-critical-only";
        case QuietModeManager::NoNotifications:
            return "quiet-mode";
        case QuietModeManager::Mute:
            return "audio-volume-muted";
        default:
            return "";
    }
}

QList<QuietModeManager::QuietMode> QuietModeManager::availableQuietModes() const {
    QList<QuietMode> modes;
    for (QuietMode mode = Sound; mode != LastQuietMode; mode = static_cast<QuietMode>(mode + 1)) {
        modes.append(mode);
    }
    return modes;
}
