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
    QuietModeManagerTd::QuietMode quietMode = QuietModeManagerTd::Sound;
};

QuietModeManagerTd::QuietModeManagerTd(QObject* parent) : QObject(parent) {
    d = new QuietModeManagerPrivate();
}

QuietModeManagerTd::~QuietModeManagerTd() {
    delete d;
}

void QuietModeManagerTd::setQuietMode(QuietModeManagerTd::QuietMode quietMode) {
    QuietModeManagerTd::QuietMode oldMode = d->quietMode;
    d->quietMode = quietMode;
    emit quietModeChanged(quietMode, oldMode);
}

QuietModeManagerTd::QuietMode QuietModeManagerTd::currentMode() {
    return d->quietMode;
}

QuietModeManagerTd::QuietMode QuietModeManagerTd::nextQuietMode() const {
    switch (d->quietMode) {
        case QuietModeManagerTd::Sound:
            //If we're in onboarding, mute the audio if the user hits mute
            return StateManager::onboardingManager()->isOnboardingRunning() ? Mute : CriticalOnly;
        case QuietModeManagerTd::CriticalOnly:
            //If we're in onboarding, mute the audio if the user hits mute
            return StateManager::onboardingManager()->isOnboardingRunning() ? Mute : NoNotifications;
        case QuietModeManagerTd::NoNotifications:
            return Mute;
        case QuietModeManagerTd::Mute:
            return Sound;
        default:
            return QuietModeManagerTd::Sound;
    }
}

QString QuietModeManagerTd::name(QuietModeManagerTd::QuietMode quietMode) {
    switch (quietMode) {
        case QuietModeManagerTd::Sound:
            return tr("Sound");
        case QuietModeManagerTd::CriticalOnly:
            return tr("Critical Only");
        case QuietModeManagerTd::NoNotifications:
            return tr("No Notifications");
        case QuietModeManagerTd::Mute:
            return tr("Mute");
        default:
            return "";
    }
}

QString QuietModeManagerTd::description(QuietModeManagerTd::QuietMode quietMode) {
    switch (quietMode) {
        case QuietModeManagerTd::Sound:
            return tr("Allows all sounds");
        case QuietModeManagerTd::CriticalOnly:
            return tr("Hides non-critical notifications");
        case QuietModeManagerTd::NoNotifications:
            return tr("Hides all notifications");
        case QuietModeManagerTd::Mute:
            return tr("Mutes all sound altogether");
        default:
            return "";
    }
}

QString QuietModeManagerTd::icon(QuietModeManagerTd::QuietMode quietMode) {
    switch (quietMode) {
        case QuietModeManagerTd::Sound:
            return "audio-volume-high";
        case QuietModeManagerTd::CriticalOnly:
            return "quiet-mode-critical-only";
        case QuietModeManagerTd::NoNotifications:
            return "quiet-mode";
        case QuietModeManagerTd::Mute:
            return "audio-volume-muted";
        default:
            return "";
    }
}

QList<QuietModeManagerTd::QuietMode> QuietModeManagerTd::availableQuietModes() const {
    QList<QuietMode> modes;
    for (QuietMode mode = Sound; mode != LastQuietMode; mode = static_cast<QuietMode>(mode + 1)) {
        modes.append(mode);
    }
    return modes;
}
