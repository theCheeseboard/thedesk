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
#include "btobex.h"

#include <QStandardPaths>
#include <QDBusObjectPath>
#include <QFile>
#include <QDir>
#include <QPointer>
#include <tnotification.h>

#include <BluezQt/Manager>
#include <BluezQt/Device>
#include <BluezQt/InitObexManagerJob>
#include <BluezQt/ObexManager>
#include <BluezQt/ObexTransfer>
#include <BluezQt/ObexSession>
#include <BluezQt/PendingCall>

struct BtObexPrivate {
    BluezQt::ManagerPtr manager;
    BluezQt::ObexManagerPtr obexManager;
    QPointer<tNotification> notification;
};

BtObex::BtObex(BluezQt::ManagerPtr manager, QObject* parent) : BluezQt::ObexAgent(parent) {
    d = new BtObexPrivate();
    d->manager = manager;
    d->obexManager = BluezQt::ObexManagerPtr(new BluezQt::ObexManager());

    BluezQt::PendingCall* startCall = BluezQt::ObexManager::startService();
    connect(startCall, &BluezQt::PendingCall::finished, this, [ = ] {
        BluezQt::InitObexManagerJob* initObexManagerJob = d->obexManager->init();
        connect(initObexManagerJob, &BluezQt::InitObexManagerJob::result, this, [ = ] {
            d->obexManager->registerAgent(this);
        });

        initObexManagerJob->start();
    });
}

BtObex::~BtObex() {
    delete d;
}


QDBusObjectPath BtObex::objectPath() const {
    return QDBusObjectPath("/com/vicr123/theDesk/BluetoothPlugin/ObexAgent");
}

void BtObex::authorizePush(BluezQt::ObexTransferPtr transfer, BluezQt::ObexSessionPtr session, const BluezQt::Request<QString>& request) {
    tNotification* notification = new tNotification();
    notification->setAppIcon("preferences-system-bluetooth");
    notification->setAppName(tr("Bluetooth"));
    notification->setSummary(tr("Incoming file"));
    notification->setText(tr("%1 is sending you the file %2.").arg(QLocale().quoteString(d->manager->deviceForAddress(session->destination())->name()), QLocale().quoteString(transfer->name())));
    notification->insertAction("1reject", tr("Decline"));
    notification->insertAction("2accept", tr("Accept"));
    notification->setTimeout(0);
    connect(notification, &tNotification::actionClicked, this, [ = ](QString key) {
        if (key == "1reject") {
            request.reject();
        } else if (key == "2accept") {
            QString transferName = transfer->name();

            QString cachePath = QFile::decodeName(qgetenv("XDG_CACHE_HOME"));
            if (cachePath == "") cachePath = QDir::home().absoluteFilePath(".cache");
            cachePath += "/obexd/";

            QString temporaryPath = cachePath + transferName;
            int number = 0;
            while (QFile::exists(temporaryPath)) {
                temporaryPath = cachePath + transferName + "_" + QString::number(number);
                number++;
            }

            connect(transfer.data(), &BluezQt::ObexTransfer::statusChanged, this, [ = ](BluezQt::ObexTransfer::Status status) {
                if (status == BluezQt::ObexTransfer::Complete) {
                    //Move the file to a good place
                    QString downloadsDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
                    if (!QDir(downloadsDir).exists()) QDir::root().mkpath(downloadsDir);

                    QString permanantPath = QDir(downloadsDir).absoluteFilePath(transferName);
                    int number = 0;
                    while (QFile::exists(permanantPath)) {
                        permanantPath = QDir(downloadsDir).absoluteFilePath(QStringLiteral("%1 (%2)").arg(transferName, number));
                        number++;
                    }

                    QFile::rename(temporaryPath, permanantPath);

                    tNotification* notification = new tNotification();
                    notification->setAppIcon("preferences-system-bluetooth");
                    notification->setAppName(tr("Bluetooth"));
                    notification->setSummary(tr("File received"));
                    notification->setText(tr("The file %1 has been received and can be found in your Downloads folder.").arg(QLocale().quoteString(transferName)));
                    notification->post();
                } else if (status == BluezQt::ObexTransfer::Error) {
                    tNotification* notification = new tNotification();
                    notification->setAppIcon("preferences-system-bluetooth");
                    notification->setAppName(tr("Bluetooth"));
                    notification->setSummary(tr("File transfer failed"));
                    notification->setText(tr("The file %1 was unable to be received.").arg(QLocale().quoteString(transferName)));
                    notification->post();
                }
            });

            request.accept(temporaryPath);
        }
    });
    connect(notification, &tNotification::dismissed, this, [ = ] {
        request.reject();
    });
    notification->post();

    d->notification = notification;
}

void BtObex::cancel() {
    if (d->notification) d->notification->dismiss();
}

void BtObex::release() {
}
