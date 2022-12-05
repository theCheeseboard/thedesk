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
#include "overlaywindow.h"

#include "polkitinterface.h"
#include <PolkitQt1/Subject>
#include <QDebug>
#include <tapplication.h>

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.setApplicationShareDir("thedesk/td-polkitagent");
    a.installTranslators();

    PolkitInterface* interface = new PolkitInterface();
    PolkitQt1::UnixSessionSubject subject(QApplication::applicationPid());

    if (!interface->registerListener(subject, "/com/vicr123/PolkitAuthAgent")) {
        qDebug() << "Can't register authentication agent";
        return 1;
    }

    return a.exec();
}
