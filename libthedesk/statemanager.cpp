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
#include "statemanager.h"

#include "barmanager.h"
#include "powermanager.h"
#include "statuscentermanager.h"
#include "localemanager.h"
#include "hudmanager.h"
#include "gatewaymanager.h"
#include "onboardingmanager.h"

struct StateManagerPrivate {
    StateManager* instance = nullptr;
    BarManager* barManager;
    GatewayManager* gatewayManager;
    StatusCenterManager* statusCenterManager;
    PowerManager* powerManager;
    LocaleManager* localeManager;
    HudManager* hudManager;
    OnboardingManager* onboardingManager;
};

StateManagerPrivate* StateManager::d = new StateManagerPrivate();

StateManager::StateManager() : QObject(nullptr) {
    d->barManager = new BarManager(this);
    d->gatewayManager = new GatewayManager(this);
    d->statusCenterManager = new StatusCenterManager(this);
    d->powerManager = new PowerManager(this);
    d->localeManager = new LocaleManager(this);
    d->hudManager = new HudManager(this);
    d->onboardingManager = new OnboardingManager(this);
}

StateManager* StateManager::instance() {
    if (d->instance == nullptr) d->instance = new StateManager();
    return d->instance;
}

BarManager* StateManager::barManager() {
    return d->barManager;
}

GatewayManager* StateManager::gatewayManager() {
    return d->gatewayManager;
}

PowerManager* StateManager::powerManager() {
    return d->powerManager;
}

StatusCenterManager* StateManager::statusCenterManager() {
    return d->statusCenterManager;
}

LocaleManager* StateManager::localeManager() {
    return d->localeManager;
}

HudManager* StateManager::hudManager() {
    return d->hudManager;
}

OnboardingManager* StateManager::onboardingManager() {
    return d->onboardingManager;
}
