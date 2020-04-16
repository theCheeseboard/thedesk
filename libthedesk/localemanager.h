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
#ifndef LOCALEMANAGER_H
#define LOCALEMANAGER_H

#include <QLocale>

struct LocaleManagerPrivate;
class LocaleManager : public QObject {
        Q_OBJECT
    public:
        explicit LocaleManager(QObject* parent = nullptr);
        ~LocaleManager();

        int addTranslationSet(QStringList searchPaths);
        void removeTranslationSet(int translationSet);

        QLocale showLocaleSelector(QWidget* parent, bool* ok = nullptr);
        void addLocale(QLocale locale);
        void prependLocale(QLocale locale);
        void removeLocale(QLocale locale);
        QList<QLocale> locales();

        QLocale::Country formatCountry();
        void setFormatCountry(QLocale::Country country);

    signals:


    private:
        LocaleManagerPrivate* d;

        void updateTranslator(int id);
        void updateLocales();
        QString glibName(QLocale locale, QString encoding = "UTF-8");
};

#endif // LOCALEMANAGER_H
