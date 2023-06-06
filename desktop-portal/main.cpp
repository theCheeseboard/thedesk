/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include <tapplication.h>

#include "interfaces/accessinterface.h"
#include "interfaces/accountinterface.h"
#include "interfaces/filechooserinterface.h"
#include "interfaces/screenshotinterface.h"
#include "interfaces/settingsinterface.h"

#include <QDBusConnection>

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QDBusConnection::sessionBus().registerService("org.freedesktop.impl.portal.desktop.thedesk");

    QObject* rootDbusObject = new QObject();
    new FileChooserInterface(rootDbusObject);
    new SettingsInterface(rootDbusObject);
    new AccessInterface(rootDbusObject);
    new AccountInterface(rootDbusObject);
    new ScreenshotInterface(rootDbusObject);

    QDBusConnection::sessionBus().registerObject("/org/freedesktop/portal/desktop", rootDbusObject, QDBusConnection::ExportAdaptors);

    return a.exec();
}
