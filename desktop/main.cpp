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

#include <QDir>
#include <QTimer>

#include <Screens/screendaemon.h>
#include <Wm/desktopwm.h>
#include <gatewaymanager.h>
#include <localemanager.h>
#include <powermanager.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tapplication.h>
#include <tstylemanager.h>

#include "background/background.h"
#include "bar/barwindow.h"
#include "cli/commandline.h"
#include "crash/crashhandling.h"
#include "plugins/pluginmanager.h"
#include "run/rundialog.h"
#include "server/sessionserver.h"
#include "session/endsession.h"
#include <onboarding/onboardingcontroller.h>
#include <tsettings.h>

#include "gateway/appsearchprovider.h"

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);
    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("vicr123.com");
    a.setApplicationName("theDesk");
    a.setApplicationVersion("2.0");
    a.setCopyrightYear("2024");
    a.setApplicationShareDir("thedesk");

    StateManager::instance();
    StateManager::localeManager()->addTranslationSet({a.applicationDirPath() + "/translations",
        "/usr/share/thedesk/translations"});

    tSettings::registerDefaults(QDir::cleanPath(qApp->applicationDirPath() + "/../desktop/thedesk.conf"));
    tSettings::registerDefaults("/usr/share/defaults/thedesk.conf");

    CrashHandling::prepareCrashHandler();

    // Parse command line arguments
    int parseResult = CommandLine::parse(a.arguments());
    if (parseResult != -1) {
        // Stop running right here
        return parseResult;
    }

    if (a.queryKeyboardModifiers() & Qt::ControlModifier && !PluginManager::instance()->isSafeMode()) {
        tPromiseResults<bool> results = SessionServer::instance()->askQuestion(QApplication::translate("main", "Safe Mode"), QApplication::translate("main", "You're holding the CTRL key. Start theDesk in Safe Mode?"))->await();
        if (results.result) PluginManager::instance()->setSafeMode(true);
    }

    DesktopWm::instance()->registerAsPrimaryProvider();
    PluginManager::instance()->scanPlugins();

    QObject::connect(StateManager::instance()->powerManager(), &PowerManager::powerOffConfirmationRequested, [=](PowerManager::PowerOperation operation, QString message, QStringList flags, tPromiseFunctions<void>::SuccessFunction cb) {
        EndSession::showDialog(operation, message, flags)->then(cb);
    });

    // Perform onboarding if required
    if (!OnboardingController::performOnboarding(false)) {
        // Exit now because onboarding failed (probably the user chose to log out)
        return 0;
    }

    // Prepare the run dialog
    RunDialog::initialise();

    // Prepare the background
    Background::reconfigureBackgrounds();

    StateManager::gatewayManager()->registerSearchProvider(new AppSearchProvider());

    BarWindow w;
    w.show();

    QTimer::singleShot(0, [=] {
        SessionServer::instance()->hideSplashes();
        SessionServer::instance()->performAutostart();
    });

    return a.exec();
}
