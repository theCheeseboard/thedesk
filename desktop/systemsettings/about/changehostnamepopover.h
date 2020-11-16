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
#ifndef CHANGEHOSTNAMEPOPOVER_H
#define CHANGEHOSTNAMEPOPOVER_H

#include <QWidget>

namespace Ui {
    class ChangeHostnamePopover;
}

class ChangeHostnamePopover : public QWidget {
        Q_OBJECT

    public:
        explicit ChangeHostnamePopover(QWidget* parent = nullptr);
        ~ChangeHostnamePopover();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_renameButton_clicked();

        void on_hostnameEdit_textChanged(const QString& arg1);

    signals:
        void done();

    private:
        Ui::ChangeHostnamePopover* ui;

        QString generateStaticHostname(QString hostname);
};

#endif // CHANGEHOSTNAMEPOPOVER_H
