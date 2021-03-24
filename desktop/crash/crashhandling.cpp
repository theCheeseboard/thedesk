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
#include "crashhandling.h"

#include <tapplication.h>
#include <QTextStream>
#include <signal.h>
#include <string.h>
#include "plugins/pluginmanager.h"

namespace CrashHandling {
    void catchSignal(int sig);
}

void CrashHandling::prepareCrashHandler() {
    signal(SIGSEGV, CrashHandling::catchSignal);
    signal(SIGBUS, CrashHandling::catchSignal);
    signal(SIGFPE, CrashHandling::catchSignal);
}

void CrashHandling::catchSignal(int sig) {
    const QMap<int, QString> signalNames = {
        {SIGSEGV, "SIGSEGV"},
        {SIGBUS, "SIGBUS"},
        {SIGFPE, "SIGFPE"}
    };

    QTextStream output(stderr);
    output << "----- BEGIN CRASH HANDLER -----\n";
    output << "THEDESK-TRAP: Fatal Signal: " << signalNames.value(sig) << "\n";
    output << "THEDESK-TRAP: \n";

    output << "THEDESK-TRAP: --- LOADED PLUGINS ---\n";
    for (QUuid plugin : PluginManager::instance()->loadedPlugins()) {
        output << "THEDESK-TRAP: " << plugin.toString() << "\n";
    }

    output << "THEDESK-TRAP: \n";
    output << "THEDESK-TRAP: --- BACKTRACE ---\n";

    QStringList backtrace = tApplication::exportBacktrace();
    for (QString line : backtrace) {
        output << "THEDESK-TRAP: " << line << "\n";
    }

    output.flush();

    signal(sig, SIG_DFL);
    raise(sig);
}
