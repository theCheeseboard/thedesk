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
#ifndef BTOBEX_H
#define BTOBEX_H

#include <BluezQt/ObexAgent>

struct BtObexPrivate;
class BtObex : public BluezQt::ObexAgent {
        Q_OBJECT
    public:
        explicit BtObex(BluezQt::ManagerPtr manager, QObject* parent = nullptr);
        ~BtObex();

    signals:

    private:
        BtObexPrivate* d;

        // ObexAgent interface
    public:
        QDBusObjectPath objectPath() const;
        void authorizePush(BluezQt::ObexTransferPtr transfer, BluezQt::ObexSessionPtr session, const BluezQt::Request<QString>& request);
        void cancel();
        void release();
};

#endif // BTOBEX_H
