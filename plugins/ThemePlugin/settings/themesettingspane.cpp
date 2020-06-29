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
#include "themesettingspane.h"
#include "ui_themesettingspane.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <tsettings.h>
#include <QIcon>
#include <QDBusConnectionInterface>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QDir>

struct ThemeSettingsPanePrivate {
    QSettings* kwinSettings;
    tSettings* themeSettings;
    tSettings settings;
};

ThemeSettingsPane::ThemeSettingsPane() :
    StatusCenterPane(),
    ui(new Ui::ThemeSettingsPane) {
    ui->setupUi(this);

    d = new ThemeSettingsPanePrivate();
    d->themeSettings = new tSettings("theDesk.platform", this);
    d->kwinSettings = new QSettings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kwinrc", QSettings::IniFormat);

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->coloursWidget->setFixedWidth(contentWidth);
    ui->fontsWidget->setFixedWidth(contentWidth);
    ui->widgetsWidget->setFixedWidth(contentWidth);
    ui->windowBordersWidget->setFixedWidth(contentWidth);
    ui->effectsWidget->setFixedWidth(contentWidth);

    ui->accentBlue->setColorName("blue");
    ui->accentGreen->setColorName("green");
    ui->accentOrange->setColorName("orange");
    ui->accentPink->setColorName("pink");

    for (QString key : QStyleFactory::keys()) {
        QSignalBlocker blocker(ui->widgetThemeBox);
        ui->widgetThemeBox->addItem(key);
    }

    connect(d->themeSettings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
        if (key == "Palette/base") {
            updateBaseColour();
        } else if (key.startsWith("Fonts/")) {
            updateFonts();
        } else if (key == "Platform/style") {
            updateWidgets();
        }
    });
    updateBaseColour();
    updateFonts();
    updateWidgets();

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
        if (key == "Appearance/translucent") {
            ui->transparencySwitch->setChecked(value.toBool());
        }
    });
    ui->transparencySwitch->setChecked(d->settings.value("Appearance/translucent").toBool());
}

ThemeSettingsPane::~ThemeSettingsPane() {
    delete d;
    delete ui;
}

void ThemeSettingsPane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}

void ThemeSettingsPane::updateBaseColour() {
    d->kwinSettings->beginGroup("org.kde.kdecoration2");
    QString theme = d->kwinSettings->value("theme").toString();
    QString library = d->kwinSettings->value("library").toString();
    d->kwinSettings->endGroup();

    bool isHandlingWindowBorders = (library == "org.kde.kwin.aurorae" && theme.startsWith("__aurorae__svg__Contemporary"));

    QSignalBlocker blocker(ui->baseColourComboBox);
    QString baseColor = d->themeSettings->value("Palette/base").toString();
    if (baseColor == "dark") {
        ui->baseColourComboBox->setCurrentIndex(0);
    } else if (baseColor == "light") {
        ui->baseColourComboBox->setCurrentIndex(1);
    }

    if (isHandlingWindowBorders) {
        writeWindowBorders();
    } else {
        //Ensure that KWin is running and that the themes are (probably) installed
        if (QDir("/usr/share/aurorae/themes/Contemporary").exists() &&
            QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.KWin").value()) {
            ui->windowBordersConditonalWidget->expand();
        }
    }
}

void ThemeSettingsPane::updateFonts() {
    QSignalBlocker blocker1(ui->interfaceFont);
    QSignalBlocker blocker2(ui->interfaceFontSize);
    QSignalBlocker blocker3(ui->fixedFont);
    QSignalBlocker blocker4(ui->fixedFontSize);
    QStringList systemFont = d->themeSettings->value("Fonts/system").toStringList();
    if (systemFont.count() == 2) {
        QFont font(systemFont.at(0));
        ui->interfaceFont->setCurrentFont(font);
        ui->interfaceFontSize->setValue(systemFont.at(1).toDouble());
    }

    QStringList fixedFont = d->themeSettings->value("Fonts/fixed").toStringList();
    if (fixedFont.count() == 2) {
        QFont font(fixedFont.at(0));
        ui->fixedFont->setCurrentFont(font);
        ui->fixedFontSize->setValue(fixedFont.at(1).toDouble());
    }
}

void ThemeSettingsPane::updateWidgets() {
    QSignalBlocker blocker(ui->widgetThemeBox);
    QString name = d->themeSettings->value("Platform/style").toString();
    ui->widgetThemeBox->setCurrentIndex(QStyleFactory::keys().indexOf(name));
}

void ThemeSettingsPane::setFonts() {
    d->themeSettings->setValue("Fonts/system", QStringList({
        ui->interfaceFont->currentFont().family(),
        QString::number(ui->interfaceFontSize->value())
    }));
    d->themeSettings->setValue("Fonts/fixed", QStringList({
        ui->fixedFont->currentFont().family(),
        QString::number(ui->fixedFontSize->value())
    }));
}

void ThemeSettingsPane::writeWindowBorders() {
    QString theme;
    QString baseColor = d->themeSettings->value("Palette/base").toString();
    if (baseColor == "dark") {
        theme = "__aurorae__svg__Contemporary";
    } else {
        theme = "__aurorae__svg__Contemporary-light";
    }

    d->kwinSettings->beginGroup("org.kde.kdecoration2");
    if (d->kwinSettings->value("theme").toString() == theme && d->kwinSettings->value("library").toString() == "org.kde.kwin.aurorae") {
        //Nothing to do
        d->kwinSettings->endGroup();
        return;
    }
    d->kwinSettings->setValue("library", "org.kde.kwin.aurorae");
    d->kwinSettings->setValue("theme", theme);
    d->kwinSettings->endGroup();

    d->kwinSettings->sync();

    //Reload KWin
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.KWin", "/KWin", "org.kde.KWin", "reconfigure");
    QDBusConnection::sessionBus().asyncCall(message);

    ui->windowBordersConditonalWidget->collapse();
}

QString ThemeSettingsPane::name() {
    return "ThemeSettings";
}

QString ThemeSettingsPane::displayName() {
    return tr("Theme");
}

QIcon ThemeSettingsPane::icon() {
    return QIcon::fromTheme("preferences-desktop-theme");
}

QWidget* ThemeSettingsPane::leftPane() {
    return nullptr;
}

void ThemeSettingsPane::on_baseColourComboBox_currentIndexChanged(int index) {
    switch (index) {
        case 0:
            d->themeSettings->setValue("Palette/base", "dark");
            break;
        case 1:
            d->themeSettings->setValue("Palette/base", "light");
    }
}

void ThemeSettingsPane::on_titleLabel_backButtonClicked() {
    StateManager::statusCenterManager()->showStatusCenterHamburgerMenu();
}

void ThemeSettingsPane::on_fixedFont_currentFontChanged(const QFont& f) {
    setFonts();
}

void ThemeSettingsPane::on_fixedFontSize_valueChanged(double arg1) {
    setFonts();
}

void ThemeSettingsPane::on_interfaceFont_currentFontChanged(const QFont& f) {
    setFonts();
}

void ThemeSettingsPane::on_interfaceFontSize_valueChanged(double arg1) {
    setFonts();
}

void ThemeSettingsPane::on_widgetThemeBox_currentIndexChanged(int index) {
    QString name = QStyleFactory::keys().at(index);
    d->themeSettings->setValue("Platform/style", name);
}

void ThemeSettingsPane::on_setWindowBordersButton_clicked() {
    writeWindowBorders();
}

void ThemeSettingsPane::on_transparencySwitch_toggled(bool checked) {
    d->settings.setValue("Appearance/translucent", checked);
}
