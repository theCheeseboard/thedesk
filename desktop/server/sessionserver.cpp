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
#include "sessionserver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>
#include <QDebug>

struct SessionServerPrivate {
    QLocalSocket* socket;
    SessionServer* instance = nullptr;

    bool available = false;
};

SessionServerPrivate* SessionServer::d = new SessionServerPrivate();

SessionServer* SessionServer::instance() {
    if (!d->instance) d->instance = new SessionServer();
    return d->instance;
}

void SessionServer::setServerPath(QString serverPath) {
    d->socket->connectToServer(serverPath);
}

void SessionServer::hideSplashes() {
    if (!d->available) return;
    d->socket->write(QJsonDocument(QJsonObject({
        {"type", "loadComplete"}
    })).toBinaryData());
    d->socket->flush();
}

SessionServer::SessionServer(QObject* parent) : QObject(parent) {
    d->socket = new QLocalSocket();
    connect(d->socket, &QLocalSocket::connected, this, [ = ] {
        d->available = true;
    });
    connect(d->socket, &QLocalSocket::disconnected, this, [ = ] {
        d->available = false;
    });
}
