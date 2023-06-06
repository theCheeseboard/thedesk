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
#include "filechooserinterface.h"

#include "dialogs/filedialog.h"
#include "portalcommon.h"
#include "portalhandle.h"
#include <QCoroCore>
#include <QDBusConnection>
#include <QDBusVariant>
#include <QWindow>
#include <tlogger.h>

FileChooserInterface::FileChooserInterface(QObject* parent) :
    QDBusAbstractAdaptor(parent) {
}

uint FileChooserInterface::OpenFile(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results) {
    tDebug("FileChooserInterface") << "Open File";

    PortalCommon::setupCoro([options, parent_window, handle, title, this](QDBusMessage reply) -> QCoro::Task<> {
        auto* portalHandle = new PortalHandle(handle);

        FileDialog dialog(false, options);
        PortalCommon::reparentWindow(&dialog, parent_window);
        if (title.isEmpty()) {
            dialog.setWindowTitle(tr("Open"));
        } else {
            dialog.setWindowTitle(title);
        }
        connect(portalHandle, &PortalHandle::closed, &dialog, [&dialog] {
            dialog.close();
        });

        dialog.open();

        auto result = co_await qCoro(&dialog, &FileDialog::finished);

        if (result == FileDialog::Accepted) {
            reply.setArguments({
                uint(0), QVariantMap{
                                     {"uris", dialog.uris()},
                                     {"writable", dialog.isWritable()}}
            });
            QDBusConnection::sessionBus().send(reply);
        } else {
            reply.setArguments({uint(1), QVariantMap()});
            QDBusConnection::sessionBus().send(reply);
        }
        portalHandle->deleteLater();
    },
        message);
    return 0;
}

uint FileChooserInterface::SaveFile(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results) {
    PortalCommon::setupCoro([options, parent_window, handle, title, this](QDBusMessage reply) -> QCoro::Task<> {
        auto* portalHandle = new PortalHandle(handle);

        FileDialog dialog(true, options);
        PortalCommon::reparentWindow(&dialog, parent_window);
        if (title.isEmpty()) {
            dialog.setWindowTitle(tr("Save"));
        } else {
            dialog.setWindowTitle(title);
        }
        connect(portalHandle, &PortalHandle::closed, &dialog, [&dialog] {
            dialog.close();
        });

        dialog.open();

        auto result = co_await qCoro(&dialog, &FileDialog::finished);

        if (result == FileDialog::Accepted) {
            reply.setArguments({
                uint(0), QVariantMap{
                                     {"uris", dialog.uris()},
                                     {"writable", dialog.isWritable()}}
            });
            QDBusConnection::sessionBus().send(reply);
        } else {
            reply.setArguments({uint(1), QVariantMap()});
            QDBusConnection::sessionBus().send(reply);
        }
        portalHandle->deleteLater();
    },
        message);
    return 0;
}

uint FileChooserInterface::SaveFiles(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results) {
    return 0;
}
