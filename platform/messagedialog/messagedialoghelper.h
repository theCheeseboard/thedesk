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
#ifndef MESSAGEDIALOGHELPER_H
#define MESSAGEDIALOGHELPER_H

#include <qpa/qplatformdialoghelper.h>

struct MessageDialogHelperPrivate;
class MessageDialogHelper : public QPlatformMessageDialogHelper {
        Q_OBJECT
    public:
        explicit MessageDialogHelper();
        ~MessageDialogHelper();

    signals:

    private:
        MessageDialogHelperPrivate* d;

        void updateWindowGeometry();

        // QPlatformMessageDialogHelper interface
    public:
        void exec();
        bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow* parent);
        void hide();
        void setOptions(const QSharedPointer<QMessageDialogOptions>& options);
};

#endif // MESSAGEDIALOGHELPER_H
