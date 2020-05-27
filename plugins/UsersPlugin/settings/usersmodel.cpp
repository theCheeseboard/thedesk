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
#include "usersmodel.h"

#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QDBusArgument>
#include <tpromise.h>
#include <QDBusInterface>
#include "user.h"

struct UsersModelPrivate {
    QList<UserPtr> users;
};

UsersModel::UsersModel(QObject *parent)
    : QAbstractListModel(parent)
{
    d = new UsersModelPrivate();

    QDBusConnection::systemBus().connect("org.freedesktop.Accounts", "/org/freedesktop/Accounts", "org.freedesktop.Accounts", "UserAdded", this, SLOT(userAdded(QDBusObjectPath)));
    QDBusConnection::systemBus().connect("org.freedesktop.Accounts", "/org/freedesktop/Accounts", "org.freedesktop.Accounts", "UserDeleted", this, SLOT(userRemoved(QDBusObjectPath)));

    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.Accounts", "/org/freedesktop/Accounts", "org.freedesktop.Accounts", "ListCachedUsers");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(msg));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        QDBusArgument arg = watcher->reply().arguments().first().value<QDBusArgument>();
        QList<QDBusObjectPath> users;
        arg >> users;
        for (QDBusObjectPath user : users) {
            this->userAdded(user);
        }

        watcher->deleteLater();
        emit dataChanged(index(0), index(rowCount()));
    });
}

UsersModel::~UsersModel()
{
    delete d;
}

int UsersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return d->users.count();
}

QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    UserPtr u = d->users.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return u->displayName();
        case Qt::UserRole:
            return QVariant::fromValue(u);
    }

    return QVariant();
}

void UsersModel::userAdded(QDBusObjectPath path)
{
    UserPtr u(new User(path));
    connect(u.data(), &User::dataUpdated, this, [=] {
        int i = d->users.indexOf(u);
        emit dataChanged(index(i), index(i));
    });
    d->users.append(u);
    emit dataChanged(index(0), index(rowCount()));
}

void UsersModel::userRemoved(QDBusObjectPath path)
{
    for (UserPtr u : d->users) {
        if (u->path() == path) {
            d->users.removeOne(u);
            emit dataChanged(index(0), index(rowCount()));
            return;
        }
    }
}
