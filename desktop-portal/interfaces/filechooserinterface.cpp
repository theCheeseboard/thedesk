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
#include <QDBusVariant>
#include <QWindow>
#include <tlogger.h>

FileChooserInterface::FileChooserInterface(QObject* parent) :
    QDBusAbstractAdaptor(parent) {
}

uint FileChooserInterface::OpenFile(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results) {
    tDebug("FileChooserInterface") << "Open File";

    PortalHandle* portalHandle = new PortalHandle(handle);

    FileDialog* dialog = new FileDialog(false, options);
    PortalCommon::reparentWindow(dialog, parent_window);
    if (title.isEmpty()) {
        dialog->setWindowTitle(tr("Open"));
    } else {
        dialog->setWindowTitle(title);
    }

    if (dialog->exec() == FileDialog::Accepted) {
        results.insert("uris", dialog->uris());
        //        results.insert("choices", dialog->choices());
        results.insert("writable", dialog->isWritable());

        portalHandle->deleteLater();
        return 0;
    } else {
        portalHandle->deleteLater();
        return 1;
    }

    connect(portalHandle, &PortalHandle::closed, this, [=] {
        dialog->close();
    });
}

uint FileChooserInterface::SaveFile(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results) {
    PortalHandle* portalHandle = new PortalHandle(handle);

    FileDialog* dialog = new FileDialog(true, options);
    PortalCommon::reparentWindow(dialog, parent_window);

    if (title.isEmpty()) {
        dialog->setWindowTitle(tr("Save"));
    } else {
        dialog->setWindowTitle(title);
    }

    if (dialog->exec() == FileDialog::Accepted) {
        results.insert("uris", dialog->uris());
        //        results.insert("choices", dialog->choices());
        results.insert("writable", dialog->isWritable());

        portalHandle->deleteLater();
        return 0;
    } else {
        portalHandle->deleteLater();
        return 1;
    }

    connect(portalHandle, &PortalHandle::closed, this, [=] {
        dialog->close();
    });
    return 0;
}

uint FileChooserInterface::SaveFiles(const QDBusObjectPath& handle, const QString& app_id, const QString& parent_window, const QString& title, const QVariantMap& options, const QDBusMessage& message, QVariantMap& results) {
    return 0;
}
