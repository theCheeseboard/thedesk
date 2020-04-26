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
#include <QMessageBox>

struct SessionServerPrivate {
    QLocalSocket* socket;
    SessionServer* instance = nullptr;

    bool available = false;

    bool havePendingQuestion = false;
    tPromiseFunctions<bool>::SuccessFunction questionRes;
};

SessionServerPrivate* SessionServer::d = new SessionServerPrivate();

SessionServer* SessionServer::instance() {
    if (!d->instance) d->instance = new SessionServer();
    return d->instance;
}

void SessionServer::setServerPath(QString serverPath) {
    d->socket->connectToServer(serverPath);

    //Wait until connection so we can send messages
    d->socket->waitForConnected();
}

void SessionServer::hideSplashes() {
    if (!d->available) return;
    d->socket->write(QJsonDocument(QJsonObject({
        {"type", "hideSplash"}
    })).toBinaryData());
    d->socket->flush();
}

void SessionServer::showSplashes() {
    if (!d->available) return;
    d->socket->write(QJsonDocument(QJsonObject({
        {"type", "showSplash"}
    })).toBinaryData());
    d->socket->flush();
}

void SessionServer::performAutostart() {
    if (!d->available) return;
    d->socket->write(QJsonDocument(QJsonObject({
        {"type", "autoStart"}
    })).toBinaryData());
    d->socket->flush();
}

tPromise<bool>* SessionServer::askQuestion(QString title, QString question) {
    return tPromise<bool>::runOnSameThread([ = ](tPromiseFunctions<bool>::SuccessFunction res, tPromiseFunctions<bool>::FailureFunction rej) {
        if (!d->available) {
            bool answer = QMessageBox::question(nullptr, title, question, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes;
            res(answer);
        } else {
            d->questionRes = [ = ](bool result) {
                d->havePendingQuestion = false;
                res(result);
            };
            d->havePendingQuestion = true;

            d->socket->write(QJsonDocument(QJsonObject({
                {"type", "question"},
                {"title", title},
                {"question", question}
            })).toBinaryData());
            d->socket->flush();
        }
    });
}

SessionServer::SessionServer(QObject* parent) : QObject(parent) {
    d->socket = new QLocalSocket();
    connect(d->socket, &QLocalSocket::connected, this, [ = ] {
        d->available = true;

    });
    connect(d->socket, &QLocalSocket::disconnected, this, [ = ] {
        d->available = false;
    });

    connect(d->socket, &QLocalSocket::readyRead, this, &SessionServer::readData);
}

void SessionServer::readData() {
    QByteArray data = d->socket->readAll();
    QJsonDocument doc = QJsonDocument::fromBinaryData(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("type")) {
            QString type = obj.value("type").toString();
            if (type == "questionResponse" && d->havePendingQuestion) {
                d->questionRes(obj.value("response").toBool());
            }
        }
    }
}
