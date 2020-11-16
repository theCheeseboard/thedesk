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
#include "changehostnamepopover.h"
#include "ui_changehostnamepopover.h"

#include <QDBusInterface>
#include <QHostInfo>

ChangeHostnamePopover::ChangeHostnamePopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ChangeHostnamePopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);

    QDBusInterface hostnamed("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.hostname1", QDBusConnection::systemBus());
    QString hostname = hostnamed.property("PrettyHostname").toString();
    if (hostname.isEmpty()) hostname = hostnamed.property("Hostname").toString();
    if (hostname.isEmpty()) hostname = QHostInfo::localHostName();

    ui->hostnameEdit->setText(hostname);
}

ChangeHostnamePopover::~ChangeHostnamePopover() {
    delete ui;
}

void ChangeHostnamePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void ChangeHostnamePopover::on_renameButton_clicked() {
    QDBusMessage prettyMessage = QDBusMessage::createMethodCall("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.hostname1", "SetPrettyHostname");
    prettyMessage.setArguments({ui->hostnameEdit->text(), true});
    QDBusConnection::systemBus().asyncCall(prettyMessage);


    QDBusMessage staticMessage = QDBusMessage::createMethodCall("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.hostname1", "SetStaticHostname");
    staticMessage.setArguments({generateStaticHostname(ui->hostnameEdit->text()), true});
    QDBusConnection::systemBus().asyncCall(staticMessage);

    emit done();
}

void ChangeHostnamePopover::on_hostnameEdit_textChanged(const QString& arg1) {
    ui->networkCompatibleName->setText(generateStaticHostname(arg1));
}

QString ChangeHostnamePopover::generateStaticHostname(QString hostname) {
    QString allowedCharacters = QStringLiteral("abcdefghijklmnopqrstuvwxyz0123456789-");
    QString ignoredCharacters = QStringLiteral("'\"");
    QString staticHostname;
    bool allowHyphen = false;
    for (QChar c : hostname) {
        c = c.toLower();

        if (ignoredCharacters.contains(c)) continue;

        if (allowedCharacters.contains(c)) {
            staticHostname.append(c);
            allowHyphen = true;
            continue;
        }

        bool haveCharacter = false;
        for (QChar ch : c.decomposition()) {
            if (allowedCharacters.contains(ch)) {
                staticHostname.append(ch);
                allowHyphen = true;
                haveCharacter = true;
                continue;
            }
        }
        if (haveCharacter) continue;

        if (allowHyphen) {
            allowHyphen = false;
            staticHostname.append("-");
        }
    }

    if (staticHostname.endsWith("-")) staticHostname.remove(staticHostname.length() - 1, 1);
    staticHostname.truncate(63);
    if (staticHostname.isEmpty()) return QStringLiteral("localhost");
    return staticHostname;
}
