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

#include "crash/backtracedialog.h"
#include "splashwindow.h"
#include <Applications/application.h>
#include <QApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QPointer>
#include <QProcess>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QTimer>
#include <Screens/screendaemon.h>
#include <Wm/desktopwm.h>
#include <libcontemporary_global.h>
#include <tnotification.h>
#include <tsettings.h>

struct SplashControllerPrivate {
        SplashController* instance = nullptr;

        SplashWindow* splash;
        QProcess* process = nullptr;
        QProcess* wm = nullptr;

        QLocalServer* server;
        QPointer<QLocalSocket> socket;
        QString serverPath;

        QByteArray buffer;

        tSettings* settings;

        bool autostartDone = false;
        bool startedSuccessfully = false;
        bool performingAutoRecovery = false;
        QElapsedTimer timeSinceSuccessfulStart;
        QStringList currentBacktrace;
        QStringList lastBacktrace;
};

SplashControllerPrivate* SplashController::d = new SplashControllerPrivate();

SplashController::SplashController(QObject* parent) :
    QObject(parent) {
    d->settings = new tSettings();
    this->initSession();
}

void SplashController::socketDataAvailable() {
    d->buffer.append(d->socket->readAll());

    int bracket = 0;
    for (int i = 0; i < d->buffer.count(); i++) {
        if (d->buffer.at(i) == '{') {
            bracket++;
        } else if (d->buffer.at(i) == '}') {
            bracket--;
        } else if (bracket == 0) {
            // Error
            d->buffer = d->buffer.mid(i + 1);
            i = -1;
        }

        if (bracket == 0) {
            QByteArray data = d->buffer.left(i + 1);
            d->buffer = d->buffer.mid(i + 1);
            i = -1;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject obj = doc.object();
            if (obj.contains("type")) {
                QString type = obj.value("type").toString();
                if (type == "hideSplash") {
                    emit hideSplashes();
                    d->startedSuccessfully = true;
                    d->timeSinceSuccessfulStart.restart();

                    if (d->performingAutoRecovery) {
                        tNotification* notification = new tNotification();
                        notification->setAppIcon("com.vicr123.thedesk");
                        notification->setAppName("theDesk");
                        notification->setSummary(tr("Oh, Bonkers!"));
                        notification->setText(tr("theDesk had a problem and was restarted."));
                        notification->setTransient(true);
                        notification->insertAction("details", tr("View Details"));
                        notification->post();
                        connect(notification, &tNotification::actionClicked, this, [=](QString key) {
                            if (key == "details") {
                                BacktraceDialog* dialog = new BacktraceDialog();
                                connect(dialog, &BacktraceDialog::finished, dialog, &BacktraceDialog::deleteLater);
                                dialog->show();
                            }
                        });
                    }
                } else if (type == "showSplash") {
                    emit starting();
                } else if (type == "autoStart") {
                    // Run Autostart apps
                    this->runAutostart();
                } else if (type == "question") {
                    emit question(obj.value("title").toString(), obj.value("question").toString());
                }
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
    // Start the server
    d->serverPath = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/startdesk-" + QString::number(QRandomGenerator::system()->generate64());

    QLocalServer::removeServer(d->serverPath);

    d->server = new QLocalServer(this);
    connect(d->server, &QLocalServer::newConnection, this, [=] {
        d->socket = d->server->nextPendingConnection();
        connect(d->socket, &QLocalSocket::readyRead, this, &SplashController::socketDataAvailable);
        connect(d->socket, &QLocalSocket::disconnected, d->socket, &QLocalSocket::deleteLater);
        d->server->close();
    });

    // Start the window manager
    this->startWM();
}

void SplashController::runAutostart() {
    if (d->autostartDone) return;
    if (qEnvironmentVariable("THEDESK_NO_AUTOSTART") == "true") return;

    // Autostart the Polkit agent
    QString pkPath = QStringLiteral(SYSTEM_LIBRARY_DIRECTORY).append("/td-polkitagent");
    if (QFile::exists(pkPath)) {
        auto pkProc = new QProcess(this);
        pkProc->start(pkPath);
    }

    QStringList searchPaths = {
        qEnvironmentVariable("XDG_CONFIG_HOME", QDir::homePath() + "/.config") + "/autostart",
        qEnvironmentVariable("XDG_CONFIG_DIRS", "/etc/xdg") + "/autostart"};

    for (QString desktopEntry : Application::allApplications(searchPaths)) {
        ApplicationPointer app(new Application(desktopEntry, searchPaths));
        if (app->getProperty("Hidden", "false").toString() == "true") continue; // Ignore this autostart entry
        if (!app->getStringList("OnlyShowIn", {"thedesk"}).contains("thedesk")) continue;
        if (app->getStringList("NotShowIn").contains("thedesk")) continue;

        // Blacklist Touchegg
        if (desktopEntry == "touchegg") continue;

        if (app->hasProperty("TryExec")) {
            QString tryExecPath = app->getProperty("TryExec").toString();
            if (tryExecPath.startsWith("/")) {
                // TryExec is an absolute path
                QFile testFile(tryExecPath);
                if (!testFile.exists() | testFile.permissions() & ~QFile::ExeUser) continue;
            } else {
                // TryExec should be searched for in the PATH
                if (libContemporaryCommon::searchInPath(tryExecPath).isEmpty()) continue;
            }
        }

        // Autostart this app
        app->launch();
    }

    d->autostartDone = true;
}

void SplashController::startWM() {
    if (!d->wm && QApplication::platformName() != "wayland") {
        d->wm = new QProcess(this);
        QString wmgr = d->settings->value("Session/WindowManager").toString();
        d->wm->start(d->settings->value("Session/WindowManager").toString(), d->settings->delimitedList("Session/WindowManagerArguments")); // TODO: make this a configurable setting
    }
}

void SplashController::startDE() {
    if (d->process) return;

    emit starting();
    d->server->listen(d->serverPath);
    d->startedSuccessfully = false;

    // Set correct DPI
    tSettings settings;
    DesktopWm::instance()->registerAsPrimaryProvider();
    ScreenDaemon::instance()->setDpi(settings.value("Display/dpi").toInt());

    QString thedeskPath = qEnvironmentVariable("THEDESK_PATH", "/usr/bin/thedesk");
    d->process = new QProcess(this);
    //    d->process->setProcessChannelMode(QProcess::ForwardedChannels);
    d->process->setProgram(thedeskPath);
    d->process->setArguments({"--sessionserver", d->serverPath});
    d->process->start();

    connect(d->process, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart || !d->startedSuccessfully) {
            emit startFail();
        } else if (error == QProcess::Crashed) {
            d->lastBacktrace = d->currentBacktrace;
            d->currentBacktrace.clear();

            // Check to see if we should crash or not
            if (d->timeSinceSuccessfulStart.hasExpired(60000)) {
                // Attempt to recover
                QTimer::singleShot(1000, this, [=] {
                    d->performingAutoRecovery = true;
                    this->startDE();
                });
            } else {
                d->performingAutoRecovery = false;
                emit crash();
            }
        }
    });
    connect(d->process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit) {
            if (exitCode == 0) {
                this->logout();
            }
        }

        d->process->deleteLater();
        d->process = nullptr;

        d->server->close();
        if (d->socket) d->socket->close();
    });
    connect(d->process, &QProcess::readyReadStandardOutput, this, [=] {
        QString data = d->process->readAllStandardOutput();

        QTextStream output(stdout);
        output << data;
    });
    connect(d->process, &QProcess::readyReadStandardError, this, [=] {
        QString data = d->process->readAllStandardError();

        for (QString line : data.split("\n")) {
            if (line.startsWith("THEDESK-TRAP:")) {
                d->currentBacktrace.append(line.mid(14));
            }
        }

        QTextStream output(stderr);
        output << data;
    });
}

void SplashController::logout() {
    if (d->wm) d->wm->kill();
    QApplication::exit();
}

void SplashController::respond(bool answer) {
    d->socket->write(QJsonDocument(QJsonObject({
                                       {"type",     "questionResponse"},
                                       {"response", answer            }
    }))
                         .toJson());
    d->socket->flush();
}

QStringList SplashController::lastBacktrace() {
    return d->lastBacktrace;
}
