#include <QCommandLineParser>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFile>
#include <termios.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    QCoreApplication::setSetuidAllowed(true);
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("username", "Username to check the password of", "<username>");
    parser.process(a);

    QFile shadow("/etc/shadow");

    shadow.open(QFile::ReadOnly);
    if (!shadow.isReadable()) {
        QTextStream(stderr) << "Can't read the shadow file.\n";
        QTextStream(stderr) << "This command needs to be run as root.\n";
        return 2;
    }

    if (parser.positionalArguments().count() < 1) {
        QTextStream(stderr) << "Not enough arguments\n";
        return 2;
    }

    // TODO: THIS IS BAD.
    // We should not be reading the shadow file directly.
    // Consider invoking PAM
    QStringList shadowFile(QString(shadow.readAll()).split("\n"));
    for (const QString& user : shadowFile) {
        auto parts = user.split(":");
        if (parts.length() < 2) continue;

        if (parts.constFirst() == parser.positionalArguments().at(0)) {
            QString hashedPassword = parts.at(1);
            QStringList passwordParts = hashedPassword.split("$");
            if (passwordParts.count() == 1) {
                QTextStream(stdout) << "No password.\n";
                return 0;
            } else if (passwordParts.count() != 4) {
                QTextStream(stdout) << "Unable to parse shadow file\n";
                return 2;
            } else {
                QTextStream in(stdin, QIODevice::ReadOnly);

                // Turn off echo
                struct termios term;
                tcgetattr(fileno(stdin), &term);

                term.c_lflag &= ~ECHO;
                tcsetattr(fileno(stdin), 0, &term);

                QTextStream(stderr) << "Password: ";

                const char* characters = (crypt(in.readLine().toUtf8().data(), QString("$" + passwordParts.at(1) + "$" + passwordParts.at(2)).toStdString().c_str()));
                QString encryptedPass(characters);

                QTextStream(stderr) << "\n";

                if (encryptedPass.split("$").at(3) == passwordParts.at(3)) {
                    QTextStream(stdout) << "Password correct\n";
                    return 0;
                } else {
                    QTextStream(stderr) << "Password incorrect\n";
                    return 1;
                }
            }
        }
    }

    QTextStream(stderr) << "Unable to find user in shadow file\n";
    return 2;
}
