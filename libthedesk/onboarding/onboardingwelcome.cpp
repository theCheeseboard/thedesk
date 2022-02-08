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
#include "onboardingwelcome.h"
#include "ui_onboardingwelcome.h"

#include <statemanager.h>
#include <onboardingmanager.h>
#include <localemanager.h>
#include <tsettings.h>
#include <Wm/desktopwm.h>
#include <QSvgRenderer>
#include <QPainter>
#include <QDBusInterface>

struct OnboardingWelcomePrivate {
    tSettings settings;
    QSvgRenderer backgroundRenderer;
    QDBusInterface* hostnamed;
};

OnboardingWelcome::OnboardingWelcome(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingWelcome) {
    ui->setupUi(this);

    d = new OnboardingWelcomePrivate();
    d->backgroundRenderer.load(d->settings.value("Onboarding/welcomeGraphic").toString());

    ui->emergencyButton->setVisible(false);

    connect(DesktopWm::instance(), &DesktopWm::currentKeyboardLayoutChanged, this, [ = ] {
        ui->keyboardButton->setText(DesktopWm::currentKeyboardLayout());
    });
    ui->keyboardButton->setText(DesktopWm::currentKeyboardLayout());

    d->hostnamed = new QDBusInterface("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.hostname1", QDBusConnection::systemBus(), this);

    if (StateManager::onboardingManager()->isSystemOnboarding()) {
        ui->titleLabel->setText(tr("Welcome to %1!").arg(d->hostnamed->property("OperatingSystemPrettyName").toString()));
        ui->descriptionLabel->setText(tr("You've made it! We'll quickly go through some important setup like setting up accounts and connecting to the Internet, and then you'll be up and running."));
    } else {
        ui->titleLabel->setText(tr("Welcome to %1!").arg(QApplication::applicationName()));
        ui->descriptionLabel->setText(tr("Just a bit of important setup to do before you get started. This won't take long!"));
    }

    QPalette pal = this->palette();
    pal.setColor(QPalette::WindowText, Qt::white);
    this->setPalette(pal);
}

OnboardingWelcome::~OnboardingWelcome() {
    delete d;
    delete ui;
}

void OnboardingWelcome::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}


QString OnboardingWelcome::name() {
    return "OnboardingWelcome";
}

QString OnboardingWelcome::displayName() {
    return tr("Welcome");
}

void OnboardingWelcome::on_nextButton_clicked() {
    StateManager::onboardingManager()->nextStep();
}

void OnboardingWelcome::on_languageButton_clicked() {
    LocaleManager* manager = StateManager::localeManager();
    bool ok;
    QLocale locale = manager->showLocaleSelector(this->window(), &ok);
    if (ok) {
        if (manager->locales().contains(locale)) manager->removeLocale(locale);
        manager->prependLocale(locale);
    }
}

void OnboardingWelcome::on_keyboardButton_clicked() {
    bool ok;
    QString newLayout = StateManager::localeManager()->showKeyboardLayoutSelector(this->window(), &ok);
    if (!ok) return;

    QStringList layouts = d->settings.delimitedList("Input/keyboard.layouts");
    if (!layouts.contains(newLayout)) {
        layouts.append(newLayout);
        layouts.removeAll("");
        d->settings.setDelimitedList("Input/keyboard.layouts", layouts);
    }

    DesktopWm::setCurrentKeyboardLayout(newLayout);
}


void OnboardingWelcome::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    d->backgroundRenderer.render(&painter);
}
