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
#include "localemanager.h"

#include <QMap>
#include <QTranslator>
#include <QApplication>
#include <QDir>
#include "private/localeselector.h"
#include <tpopover.h>
#include <tpromise.h>
#include <tsettings.h>

struct LocaleManagerPrivate {
    QMap<int, QTranslator*> translators;
    QMap<int, QStringList> searchPaths;

    tSettings settings;
    QStringList preferredLocales;
    QString formats;

    int current = 0;
};

LocaleManager::LocaleManager(QObject* parent) : QObject(parent) {
    d = new LocaleManagerPrivate();

    d->preferredLocales = d->settings.delimitedList("Locale/locales");
    if (d->preferredLocales.count() == 1 && d->preferredLocales.first() == "") d->preferredLocales = QStringList({"C"});
    d->formats = d->settings.value("Locale/formats").toString();

    this->updateLocales();
    this->addTranslationSet({
        QDir::cleanPath(qApp->applicationDirPath() + "/../libthedesk/translations"),
        "/usr/share/thedesk/libthedesk/translations"
    });

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
        if (key == "Locale/locales") {
            d->preferredLocales = d->settings.delimitedList("Locale/locales");
            if (d->preferredLocales.count() == 1 && d->preferredLocales.first() == "") d->preferredLocales = QStringList({"C"});
            this->updateLocales();
        } else if (key == "Locale/formats") {
            d->formats = value.toString();
            this->updateLocales();
        }
    });
}

LocaleManager::~LocaleManager() {
    delete d;
}

int LocaleManager::addTranslationSet(QStringList searchPaths) {
    int id = d->current;
    d->current++;

    QTranslator* translator = new QTranslator(this);
    qApp->installTranslator(translator);

    d->translators.insert(id, translator);
    d->searchPaths.insert(id, searchPaths);

    updateTranslator(id);

    return id;
}

void LocaleManager::removeTranslationSet(int translationSet) {
    if (!d->translators.contains(translationSet)) return;

    QTranslator* translator = d->translators.value(translationSet);
    qApp->removeTranslator(translator);
    translator->deleteLater();

    d->translators.remove(translationSet);
    d->searchPaths.remove(translationSet);
}

QLocale LocaleManager::showLocaleSelector(QWidget* parent, bool* ok) {
    if (ok) *ok = true;

    tPromiseResults<QLocale> results = tPromise<QLocale>::runOnSameThread([ = ](tPromiseFunctions<QLocale>::SuccessFunction res, tPromiseFunctions<QLocale>::FailureFunction rej) {
        bool* stillValid = new bool(true);

        LocaleSelector* selector = new LocaleSelector();
        tPopover* popover = new tPopover(selector);
        popover->setPopoverWidth(SC_DPI(500));
        connect(selector, &LocaleSelector::rejected, this, [ = ] {
            rej("");
            *stillValid = false;
            popover->dismiss();
        });
        connect(selector, &LocaleSelector::accepted, this, [ = ](QLocale locale) {
            res(locale);
            *stillValid = false;
            popover->dismiss();
        });
        connect(popover, &tPopover::dismissed, this, [ = ] {
            if (*stillValid) rej("");
            popover->deleteLater();
            selector->deleteLater();
            delete stillValid;
        });
        popover->show(parent);
    })->await();

    if (!results.error.isEmpty() && ok) *ok = false;
    return results.result;
}

void LocaleManager::addLocale(QLocale locale) {
    if (!d->preferredLocales.contains(locale.bcp47Name())) {
        d->preferredLocales.append(locale.bcp47Name());
        if (d->preferredLocales.contains("C")) d->preferredLocales.removeAll("C");
        d->settings.setDelimitedList("Locale/locales", d->preferredLocales);
    }
}

void LocaleManager::prependLocale(QLocale locale) {
    if (!d->preferredLocales.contains(locale.bcp47Name())) {
        d->preferredLocales.prepend(locale.bcp47Name());
        if (d->preferredLocales.contains("C")) d->preferredLocales.removeAll("C");
        d->settings.setDelimitedList("Locale/locales", d->preferredLocales);
    }
}

void LocaleManager::removeLocale(QLocale locale) {
    d->preferredLocales.removeAll(locale.bcp47Name());
    d->settings.setDelimitedList("Locale/locales", d->preferredLocales);
}

QList<QLocale> LocaleManager::locales() {
    QList<QLocale> locales;
    for (QString locale : d->preferredLocales) {
        locales.append(QLocale(locale));
    }
    return locales;
}

QLocale::Country LocaleManager::formatCountry() {
    return QLocale(d->formats).country();
}

void LocaleManager::setFormatCountry(QLocale::Country country) {
    //Choose the first locale
    QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, country);
    if (locales.isEmpty()) locales.append(QLocale());
    d->settings.setValue("Locale/formats", locales.first().name());
}

void LocaleManager::updateTranslator(int id) {
    QTranslator* translator = d->translators.value(id);
    QStringList searchPaths = d->searchPaths.value(id);
    for (QString path : searchPaths) {
        if (translator->load(QLocale(), "", "", path, ".qm")) return; //Success!
    }
}

void LocaleManager::updateLocales() {
    if (d->preferredLocales.isEmpty()) d->preferredLocales.append("C");
    QLocale locale(d->preferredLocales.first());
    QLocale::setDefault(locale);

    QLocale formatsTryLocale(d->formats);
    QLocale formatsLocale(locale.language(), formatsTryLocale.country());
    if (formatsLocale.country() != formatsTryLocale.country()) formatsLocale = formatsTryLocale;

    QByteArray mainLocale = glibName(locale).toUtf8();
    QByteArray formatsLocaleStr = glibName(formatsLocale).toUtf8();
    qputenv("LANG", mainLocale);
    qputenv("LC_CTYPE", mainLocale);
    qputenv("LC_COLLATE", mainLocale);
    qputenv("LC_TIME", formatsLocaleStr);
    qputenv("LC_NUMERIC", formatsLocaleStr);
    qputenv("LC_MONETARY", formatsLocaleStr);
    qputenv("LC_MESSAGES", mainLocale);
    qputenv("LC_PAPER", formatsLocaleStr);
    qputenv("LC_MEASUREMENT", formatsLocaleStr);
    qputenv("LC_NAME", formatsLocaleStr);
    qputenv("LC_ADDRESS", formatsLocaleStr);
    qputenv("LC_TELEPHONE", formatsLocaleStr);
//    qputenv("LC_ALL", glibName(locale).toUtf8());

    QStringList languages;
    for (QString locale : d->preferredLocales) {
        languages.append(glibName(QLocale(locale)));
    }
    qputenv("LANGUAGE", languages.join(":").toUtf8());

    //Update all the translators
    for (int id : d->translators.keys()) {
        updateTranslator(id);
    }
}

QString LocaleManager::glibName(QLocale locale, QString encoding) {
    return locale.name() + "." + encoding;
}
