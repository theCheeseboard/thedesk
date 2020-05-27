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
#ifndef USER_H
#define USER_H

#include <QObject>
#include <QDBusObjectPath>
#include <QSharedPointer>
#include <tpromise.h>

struct UserPrivate;
class User : public QObject
{
        Q_OBJECT
    public:
        explicit User(QDBusObjectPath path, QObject *parent = nullptr);
        ~User();

        enum PasswordMode {
            SetPassword = 0,
            SetAtLogin = 1,
            NoPassword = 2
        };

        enum UserType {
            StandardUser = 0,
            Administrator = 1
        };

        QDBusObjectPath path();

        bool isCurrentUser();
        bool isLocked();
        QString displayName();
        QString userName();
        UserType userType();

    signals:
        void dataUpdated();

    public slots:
        tPromise<void>* setPassword(QString password, QString hint);
        tPromise<void>* setPasswordMode(PasswordMode mode);
        tPromise<void>* setUserType(UserType type);
        tPromise<void>* setRealName(QString realName);
        tPromise<void>* setLocked(bool locked);
        tPromise<void>* deleteUser(bool removeFiles);

    private slots:
        void changed();

    private:
        UserPrivate* d;

        void update();
};
typedef QSharedPointer<User> UserPtr;
Q_DECLARE_METATYPE(UserPtr);

#endif // USER_H
