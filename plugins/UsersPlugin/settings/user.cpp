/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
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
#include "user.h"

#include <unistd.h>
#include <QDBusInterface>

struct UserPrivate {
    QDBusInterface* interface;

    qulonglong uid;
    QString realName;
    QString userName;
    QString displayName;
    User::UserType userType;

    bool locked;
};

User::User(QDBusObjectPath path, QObject *parent) : QObject(parent)
{
    d = new UserPrivate();
    d->interface = new QDBusInterface("org.freedesktop.Accounts", path.path(), "org.freedesktop.Accounts.User", QDBusConnection::systemBus());

    QDBusConnection::systemBus().connect("org.freedesktop.Accounts", path.path(), "org.freedesktop.Accounts.User", "Changed", this, SLOT(changed()));

    this->update();
}

User::~User()
{
    delete d;
}

QDBusObjectPath User::path()
{
    return QDBusObjectPath(d->interface->path());
}

bool User::isCurrentUser()
{
    return d->uid == geteuid();
}

bool User::isLocked()
{
    return d->locked;
}

QString User::displayName()
{
    return d->displayName;
}

QString User::userName()
{
    return d->userName;
}

User::UserType User::userType()
{
    return d->userType;
}

tPromise<void>* User::setPassword(QString password, QString hint)
{
    return new tPromise<void>([=](std::function<void()> res, std::function<void(QString)> rej) {
        //Crypt password
        QRandomGenerator* rand = QRandomGenerator::global();
        QByteArray characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVXYZabcdefghijklmnopqrstuvxyz./";
        QByteArray salt = "$6$";
        for (int i = 0; i < 16; i++) {
            salt.append(characters.at(rand->bounded(characters.length())));
        }
        QString crypted = QString::fromLatin1(crypt(password.toUtf8(), salt.constData()));

        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->interface->asyncCall("SetPassword", crypted, hint));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>* User::setPasswordMode(User::PasswordMode mode)
{
    return new tPromise<void>([=](std::function<void()> res, std::function<void(QString)> rej) {
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->interface->asyncCall("SetPasswordMode", static_cast<int>(mode)));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>*User::setUserType(User::UserType type)
{
    return new tPromise<void>([=](std::function<void()> res, std::function<void(QString)> rej) {
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->interface->asyncCall("SetAccountType", static_cast<int>(type)));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>*User::setRealName(QString realName)
{
    return new tPromise<void>([=](std::function<void()> res, std::function<void(QString)> rej) {
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->interface->asyncCall("SetRealName", realName));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>*User::setLocked(bool locked)
{
    return new tPromise<void>([=](std::function<void()> res, std::function<void(QString)> rej) {
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->interface->asyncCall("SetLocked", locked));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>*User::deleteUser(bool removeFiles)
{
    return new tPromise<void>([=](std::function<void()> res, std::function<void(QString)> rej) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.Accounts", "/org/freedesktop/Accounts", "org.freedesktop.Accounts", "DeleteUser");
        message.setArguments({
            static_cast<qint64>(d->uid),
            removeFiles
        });

        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

void User::changed()
{
    this->update();
    emit dataUpdated();
}

void User::update()
{
    d->uid = d->interface->property("Uid").toULongLong();
    d->realName = d->interface->property("RealName").toString();
    d->userName = d->interface->property("UserName").toString();
    d->locked = d->interface->property("Locked").toBool();
    d->userType = static_cast<UserType>(d->interface->property("AccountType").toInt());

    QString displayName = d->realName;
    if (displayName.isEmpty()) displayName = d->userName;
    d->displayName = displayName;
}
