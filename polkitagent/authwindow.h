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
#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QWidget>
#include <PolkitQt1/Identity>
#include <PolkitQt1/Agent/Session>

namespace Ui {
    class AuthWindow;
}

struct AuthWindowPrivate;
class AuthWindow : public QWidget {
        Q_OBJECT

    public:
        explicit AuthWindow(QWidget* parent = nullptr);
        ~AuthWindow();

        void setMessage(QString message);
        void setIdentities(PolkitQt1::Identity::List identities);
        void setCallback(PolkitQt1::Agent::AsyncResult* result);
        void setCookie(QString cookie);
        void showAuthWindow();

        void cancel();

    signals:
        void dismiss();

    private slots:
        void on_okPasswordButton_clicked();

        void on_changeIdentity_clicked();

        void on_customChangeIdentity_clicked();

        void on_okResponseButton_clicked();

    private:
        Ui::AuthWindow* ui;
        AuthWindowPrivate* d;

        void initiateSession(PolkitQt1::Identity identity);
};

#endif // AUTHWINDOW_H
