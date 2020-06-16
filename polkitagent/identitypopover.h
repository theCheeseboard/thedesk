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
#ifndef IDENTITYPOPOVER_H
#define IDENTITYPOPOVER_H

#include <QWidget>
#include <PolkitQt1/Identity>

namespace Ui {
    class IdentityPopover;
}

class QListWidgetItem;
class IdentityPopover : public QWidget {
        Q_OBJECT

    public:
        explicit IdentityPopover(PolkitQt1::Identity::List identities, QWidget* parent = nullptr);
        ~IdentityPopover();

    signals:
        void selectIdentity(PolkitQt1::Identity identity);
        void done();

    private slots:
        void on_identityList_itemActivated(QListWidgetItem* item);

        void on_titleLabel_backButtonClicked();

    private:
        Ui::IdentityPopover* ui;
};

#endif // IDENTITYPOPOVER_H
