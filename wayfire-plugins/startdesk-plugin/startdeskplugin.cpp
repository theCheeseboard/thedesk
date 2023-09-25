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
#include "startdeskplugin.h"

#include <iostream>
#include <sys/wait.h>
#include <wayfire/core.hpp>

#include <QPointer>
#include <QProcess>
#include <QTextStream>

StartdeskPlugin::StartdeskPlugin() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QT_QPA_PLATFORM", "wayland");
    env.insert("WAYLAND_DISPLAY", QString::fromStdString(wf::get_core().wayland_display));

    QString x11Display = QString::fromStdString(wf::get_core().get_xwayland_display());
    if (!x11Display.isEmpty()) {
        env.insert("DISPLAY", x11Display);
    }

    QString startdeskCommand = qEnvironmentVariable("TD_WF_STARTDESK", "startdesk");
    QTextStream(stdout) << "Running startdesk command " << startdeskCommand << "\n";

    QPointer<QProcess> deskProcess = new QProcess();
    deskProcess->setProcessEnvironment(env);
    deskProcess->start(startdeskCommand, QStringList());
    deskProcess->waitForStarted();
    QObject::connect(deskProcess.data(), &QProcess::readyReadStandardOutput, [deskProcess] {
        QTextStream(stdout) << deskProcess->readAllStandardOutput();
    });
    QObject::connect(deskProcess.data(), &QProcess::readyReadStandardError, [deskProcess] {
        QTextStream(stderr) << deskProcess->readAllStandardError();
    });
    QObject::connect(deskProcess.data(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=] {
        QTextStream(stdout) << "Finished!\n";
    });

    waiter.set_timeout(1000, [=] {
        if (!deskProcess || deskProcess->waitForFinished(0)) {
            wf::get_core().shutdown();
            return false;
        }
        return true;
    });
}
