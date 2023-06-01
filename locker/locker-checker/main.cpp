#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <unistd.h>

int main(int argc, char* argv[]) {
    QCoreApplication::setSetuidAllowed(true);
    QCoreApplication a(argc, argv);

    QFile shadow("/etc/shadow");
    QStringList arguments = a.arguments();
    if (arguments.contains("--help")) {
        qDebug() << "tchkpass - Check User Password";
        qDebug() << "Usage: checkpassword user password";
        qDebug() << "";
        qDebug() << "--help        Shows this help";
        qDebug() << "";
        qDebug() << "This command needs to be run as root.";
        return 2;
    }

    shadow.open(QFile::ReadOnly);
    if (!shadow.isReadable()) {
        qDebug() << "Can't read the shadow file.";
        qDebug() << "This command needs to be run as root.";
        return 2;
    }

    QStringList shadowFile(QString(shadow.readAll()).split("\n"));
    for (QString user : shadowFile) {
        if (user.startsWith(arguments.at(1))) {
            QString hashedPassword = user.split(":").at(1);
            QStringList passwordParts = hashedPassword.split("$");
            if (passwordParts.count() == 1) {
                qDebug() << "No password.";
                return 0;
            } else {
                if (arguments.count() < 3) {
                    qDebug() << "Not enough arguments";
                    return 1;
                }
                // QString tryPassword(QCryptographicHash::hash(arguments.at(2).toUtf8(), QCryptographicHash::Sha512));
                const char* characters = (crypt(arguments.at(2).toStdString().c_str(), QString("$" + passwordParts.at(1) + "$" + passwordParts.at(2)).toStdString().c_str()));
                QString encryptedPass(characters);

                // QString hashedPass(QCryptographicHash::hash(encryptedPass.toUtf8(), QCryptographicHash::Sha512));

                if (encryptedPass.split("$").at(3) == passwordParts.at(3)) {
                    qDebug() << "Password correct";
                    return 0;
                } else {
                    qDebug() << "Password incorrect";
                    return 1;
                }
            }
        }
    }

    return 0;
}
