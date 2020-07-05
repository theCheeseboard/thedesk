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
#include "accessibilitydaemon.h"

#include <Wm/desktopwm.h>
#include <Wm/desktopaccessibility.h>
#include <tsettings.h>
#include <QDebug>

struct AccessibilityDaemonPrivate {
    tSettings settings;
};

AccessibilityDaemon::AccessibilityDaemon(QObject* parent) : QObject(parent) {
    d = new AccessibilityDaemonPrivate();

    connect(&d->settings, &tSettings::settingChanged, this, &AccessibilityDaemon::updateSetting);
    for (QString setting : {
            "Accessibility/stickykeys.active",
            "Accessibility/mousekeys.active"
        }) {
        updateSetting(setting, d->settings.value(setting));
    }

    connect(DesktopWm::accessibility(), &DesktopAccessibility::accessibilityOptionEnabledChanged, this, [ = ](DesktopAccessibility::AccessibilityOption option, bool enabled) {
        switch (option) {
            case DesktopAccessibility::StickyKeys:
                d->settings.setValue("Accessibility/stickykeys.active", enabled);
                break;
            case DesktopAccessibility::MouseKeys:
                d->settings.setValue("Accessibility/mousekeys.active", enabled);
                break;
            case DesktopAccessibility::LastAccessibilityOption:
                break;
        }
    });

}

AccessibilityDaemon::~AccessibilityDaemon() {
    delete d;
}

void AccessibilityDaemon::updateSetting(QString key, QVariant value) {
    if (key == "Accessibility/stickykeys.active") {
        DesktopWm::accessibility()->setAccessibilityOptionEnabled(DesktopAccessibility::StickyKeys, value.toBool());
    } else if (key == "Accessibility/mousekeys.active") {
        DesktopWm::accessibility()->setAccessibilityOptionEnabled(DesktopAccessibility::MouseKeys, value.toBool());
    }
}
