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

#include "splash/splashcontroller.h"
#include <tapplication.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QProcess>
#include <Screens/screendaemon.h>
#include <tsettings.h>

int main(int argc, char* argv[]) {
    // Put environment variables
    qputenv("QT_QPA_PLATFORMTHEME", "thedesk-platform");

    // Turn off flatpak forwarding for ourselves
    qputenv("TD_PLATFORM_NO_FLATPAK_FORWARD", "1");

    QString oldPath = qgetenv("PATH");
    qputenv("PATH", ("/usr/bin/thedesk-xdg-utils:" + oldPath).toUtf8());

    tApplication a(argc, argv);
    a.setApplicationName("theDesk");
    a.setOrganizationName("theSuite");
    a.setApplicationDisplayName("theDesk");
    a.setQuitOnLastWindowClosed(false);

    qunsetenv("TD_PLATFORM_NO_FLATPAK_FORWARD");

    tSettings::registerDefaults(a.applicationDirPath() + "/defaults.conf");
    tSettings::registerDefaults("/etc/theSuite/theDesk/defaults.conf");

    tSettings* settings = new tSettings();

    // Check for initialisation script
    if (settings->value("Session/UseInitializationScript").toBool()) {
        QProcess process;
        process.start(settings->value("Session/InitializationScript").toString(), QStringList());
        process.waitForFinished();
    }

    // Ask systemd to import environment variables
    QDBusMessage setEnvironmentMessage = QDBusMessage::createMethodCall("org.freedesktop.systemd1", "/org/freedesktop/systemd1", "org.freedesktop.systemd1.Manager", "SetEnvironment");
    setEnvironmentMessage.setArguments({QStringList({QStringLiteral("XDG_CURRENT_DESKTOP=%1").arg(qEnvironmentVariable("XDG_CURRENT_DESKTOP")),
        QStringLiteral("QT_QPA_PLATFORMTHEME=%1").arg(qEnvironmentVariable("QT_QPA_PLATFORMTHEME")),
        QStringLiteral("PATH=%1").arg(qEnvironmentVariable("PATH"))})});
    QDBusConnection::sessionBus().asyncCall(setEnvironmentMessage);

    SplashController::instance()->startDE();

    return a.exec();
}
