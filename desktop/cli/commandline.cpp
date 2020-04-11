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
#include "commandline.h"

#include <QCommandLineParser>
#include <QTextStream>
#include "server/sessionserver.h"

CommandLine::CommandLine(QObject* parent) : QObject(parent) {

}

#include <QDebug>

int CommandLine::parse(QStringList args) {
    QTextStream stream(stdout);

    QCommandLineParser parser;
    QCommandLineOption serverOption("sessionserver", tr("Internal use; the path to a local socket to communicate with the session manager"), tr("path"));
    parser.addOption(serverOption);

    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(args)) {
        stream << parser.errorText() << "\n";
        return 1;
    }

    if (parser.isSet(helpOption)) {
        stream << parser.helpText();
        return 0;
    }

    if (parser.isSet(versionOption)) {
        parser.showVersion();
    }

    if (!parser.value(serverOption).isEmpty()) {
        //Connect to the session server
        SessionServer::instance()->setServerPath(parser.value(serverOption));
    }

    return -1;
}
