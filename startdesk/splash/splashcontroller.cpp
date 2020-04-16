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
#include "splashcontroller.h"

#include <QFile>
#include <QProcess>
#include <QApplication>
#include <QLocalServer>
#include <QStandardPaths>
#include <QRandomGenerator>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointer>
#include <QDir>
#include <application.h>
#include <the-libs_global.h>
#include "splashwindow.h"

struct SplashControllerPrivate {
    SplashController* instance = nullptr;

    SplashWindow* splash;
    QProcess* process = nullptr;
    QProcess* wm = nullptr;

    QLocalServer* server;
    QPointer<QLocalSocket> socket;
    QString serverPath;

    bool autostartDone = false;
};

SplashControllerPrivate* SplashController::d = new SplashControllerPrivate();

SplashController::SplashController(QObject* parent) : QObject(parent) {
    this->initSession();
}

void SplashController::socketDataAvailable() {
    QByteArray data = d->socket->readAll();
    QJsonDocument doc = QJsonDocument::fromBinaryData(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("type")) {
            QString type = obj.value("type").toString();
            if (type == "hideSplash") {
                emit hideSplashes();
            } else if (type == "showSplash") {
                emit starting();
            } else if (type == "autoStart") {
                //Run Autostart apps
                this->runAutostart();
            } else if (type == "question") {
                //TODO
            }
        }
    }
}

SplashController::~SplashController() {

}

SplashController* SplashController::instance() {
    if (!d->instance) {
        d->instance = new SplashController();
        d->splash = new SplashWindow();
        d->splash->showFullScreen();
    }
    return d->instance;
}

void SplashController::initSession() {
    //Start the server
    d->serverPath = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/startdesk-" + QString::number(QRandomGenerator::system()->generate64());

    QLocalServer::removeServer(d->serverPath);

    d->server = new QLocalServer(this);
    connect(d->server, &QLocalServer::newConnection, this, [ = ] {
        d->socket = d->server->nextPendingConnection();
        connect(d->socket, &QLocalSocket::readyRead, this, &SplashController::socketDataAvailable);
        connect(d->socket, &QLocalSocket::disconnected, d->socket, &QLocalSocket::deleteLater);
        d->server->close();
    });

    //Start the window manager
    this->startWM();
}

void SplashController::runAutostart() {
    if (d->autostartDone) return;

    QStringList searchPaths = {
        qEnvironmentVariable("XDG_CONFIG_HOME", QDir::homePath() + "/.config") + "/autostart",
        qEnvironmentVariable("XDG_CONFIG_DIRS", "/etc/xdg") + "/autostart"
    };

    for (QString desktopEntry : Application::allApplications(searchPaths)) {
        ApplicationPointer app(new Application(desktopEntry, searchPaths));
        if (app->getProperty("Hidden", "false").toString() == "true") continue; //Ignore this autostart entry
        if (!app->getStringList("OnlyShowIn", {"thedesk"}).contains("thedesk")) continue;
        if (app->getStringList("NotShowIn").contains("thedesk")) continue;

        if (app->hasProperty("TryExec")) {
            QString tryExecPath = app->getProperty("TryExec").toString();
            if (tryExecPath.startsWith("/")) {
                //TryExec is an absolute path
                QFile testFile(tryExecPath);
                if (!testFile.exists() | testFile.permissions() & ~QFile::ExeUser) continue;
            } else {
                //TryExec should be searched for in the PATH
                if (theLibsGlobal::searchInPath(tryExecPath).isEmpty()) continue;
            }
        }

        //Autostart this app
        app->launch();
    }

    d->autostartDone = true;
}

void SplashController::startWM() {
    if (!d->wm) {
        d->wm = new QProcess(this);
        d->wm->start("kwin_x11 --replace"); //TODO: make this a configurable setting
    }
}

void SplashController::startDE() {
    if (d->process) return;

    emit starting();
    d->server->listen(d->serverPath);

    QString thedeskPath = qEnvironmentVariable("THEDESK_PATH", "/usr/bin/thedesk");
    d->process = new QProcess(this);
    d->process->setProcessChannelMode(QProcess::ForwardedChannels);
    d->process->setProgram(thedeskPath);
    d->process->setArguments({
        "--sessionserver", d->serverPath
    });
    d->process->start();

    connect(d->process, &QProcess::errorOccurred, this, [ = ](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            emit startFail();
        } else if (error == QProcess::Crashed) {
            emit crash();
        }
    });
    connect(d->process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [ = ](int exitCode, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit) {
            if (exitCode == 0) {
                this->logout();
            } else {
                emit crash();
            }
        }

        d->process->deleteLater();
        d->process = nullptr;

        d->server->close();
        if (d->socket) d->socket->close();
    });
}

void SplashController::logout() {
    if (d->wm) d->wm->kill();
    QApplication::exit();
}
