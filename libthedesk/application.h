/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QSettings>
#include <QSharedPointer>

struct ApplicationPrivate;
class Application {
    public:
        Application();
        Application(QString desktopEntry);
        Application(QVariantMap details);
        ~Application();

        bool isValid();
        bool hasProperty(QString propertyName) const;
        QVariant getProperty(QString propertyName, QVariant defaultValue = QVariant()) const;
        QStringList getStringList(QString propertyName, QStringList defaultValue = QStringList()) const;

        QVariant getActionProperty(QString action, QString propertyName, QVariant defaultValue = QVariant()) const;

        QString desktopEntry() const;
        void launch();

        static QStringList allApplications();

    private:
        ApplicationPrivate* d;
};
typedef QSharedPointer<Application> ApplicationPointer;
Q_DECLARE_METATYPE(ApplicationPointer)

class ApplicationDaemon : public QObject {
        Q_OBJECT
    public:
        static ApplicationDaemon* instance();

    signals:
        void appsUpdateRequired();

    private:
        ApplicationDaemon();
        static ApplicationDaemon* d;
};

#endif // APPLICATION_H
