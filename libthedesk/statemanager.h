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
#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "libthedesk_global.h"
#include <QObject>

class BarManager;
class GatewayManager;
class PowerManager;
class StatusCenterManager;
class LocaleManager;
class HudManager;
class OnboardingManager;
class QuietModeManagerTd;

struct StateManagerPrivate;
class LIBTHEDESK_EXPORT StateManager : public QObject {
        Q_OBJECT

    public:
        static StateManager* instance();
        static BarManager* barManager();
        static GatewayManager* gatewayManager();
        static PowerManager* powerManager();
        static StatusCenterManager* statusCenterManager();
        static LocaleManager* localeManager();
        static HudManager* hudManager();
        static OnboardingManager* onboardingManager();
        static QuietModeManagerTd* quietModeManager();

    private:
        explicit StateManager();

        static StateManagerPrivate* d;
};

#endif // STATEMANAGER_H
