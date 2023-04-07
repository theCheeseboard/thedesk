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
#include "platformtheme.h"

#include <private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>
#include <qpa/qplatformtheme_p.h>
#include <qpa/qplatformthemefactory_p.h>

#include "cursorhandler.h"
#include "iconloaderengine.h"
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPixmap>
#include <QScrollerProperties>
#include <QTextCharFormat>
#include <QTimer>
#include <QVariant>
#include <limits>
#include <tlogger.h>
#include <tsettings.h>

#include "messagedialog/messagedialoghelper.h"

#include "fontformat.h"
#include "paletteformat.h"

struct PlatformThemePrivate {
        QObject* parent;
        tSettings* settings;
        QPalette pal;
        CursorHandler* cursorHandler;

        QMimeDatabase mimeDb;

        QMap<QPlatformTheme::Font, QFont*> fonts;

        QPlatformTheme* flatpakPlatformTheme = nullptr;
};

PlatformTheme::PlatformTheme() :
    QPlatformTheme() {
    // Register settings locations
    tSettings::registerDefaults("theSuite", "theDesk.platform", "/etc/theSuite/theDesk/platformdefaults.conf");

    d = new PlatformThemePrivate();
    d->parent = new QObject();
    d->settings = new tSettings("theSuite", "theDesk.platform", d->parent);
    d->cursorHandler = new CursorHandler(d->parent);
    d->pal.setResolveMask(std::numeric_limits<QPalette::ResolveMask>().max());

    QObject::connect(d->settings, &tSettings::settingChanged, d->parent, [=](QString key, QVariant value) {
        // Qt Creator might crash heh...
        if (QApplication::applicationName() == "QtCreator") return;

        if (key.startsWith("Fonts/")) {
            updateFont();

            // Update the font in the application
            if (qobject_cast<QApplication*>(QCoreApplication::instance())) {
                qApp->setFont(*d->fonts.value(SystemFont));
                emit qApp->setFont(*d->fonts.value(SystemFont));
            } else if (qobject_cast<QGuiApplication*>(QCoreApplication::instance())) {
                qGuiApp->setFont(*d->fonts.value(SystemFont));
            }
        } else if (key.startsWith("Palette/")) {
            updatePalette();

            // Update the palette in the application
            if (qobject_cast<QApplication*>(QCoreApplication::instance())) {
                qApp->setPalette(d->pal);
                emit qApp->paletteChanged(d->pal);
            } else if (qobject_cast<QGuiApplication*>(QCoreApplication::instance())) {
                qGuiApp->setPalette(d->pal);
            }
        } else if (key == "Platform/style") {
            if (qobject_cast<QApplication*>(QCoreApplication::instance())) {
                qApp->setStyle(d->settings->delimitedList(key).first());
            }
        }
    });

    QScrollerProperties scrollerProperties;
    scrollerProperties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.1);
    scrollerProperties.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0.1);
    QScrollerProperties::setDefaultScrollerProperties(scrollerProperties);

    this->updateFont();
    this->updatePalette();

    tDebug("PlatformTheme") << "Using theDesk platform theme";

    // Initialise the Flatpak platform theme so that we can use the portal to open files
    d->flatpakPlatformTheme = QPlatformThemeFactory::create("flatpak", nullptr);
    if (d->flatpakPlatformTheme) {
        tDebug("PlatformTheme") << "Created Flatpak platform theme";
    }
}

PlatformTheme::~PlatformTheme() {
    d->parent->deleteLater();
    delete d;
}

QPlatformMenuItem* PlatformTheme::createPlatformMenuItem() const {
    return QPlatformTheme::createPlatformMenuItem();
}

QPlatformMenu* PlatformTheme::createPlatformMenu() const {
    return QPlatformTheme::createPlatformMenu();
}

QPlatformMenuBar* PlatformTheme::createPlatformMenuBar() const {
    return QPlatformTheme::createPlatformMenuBar();
}

void PlatformTheme::showPlatformMenuBar() {
    QPlatformTheme::showPlatformMenuBar();
}

bool PlatformTheme::usePlatformNativeDialog(QPlatformTheme::DialogType type) const {
    switch (type) {
        case QPlatformTheme::ColorDialog:
        case QPlatformTheme::FontDialog:
            return false;
        case QPlatformTheme::MessageDialog:
            return true;
        case QPlatformTheme::FileDialog:
            if (d->flatpakPlatformTheme) return d->flatpakPlatformTheme->usePlatformNativeDialog(type);
            return false;
    }
}

QPlatformDialogHelper* PlatformTheme::createPlatformDialogHelper(QPlatformTheme::DialogType type) const {
    switch (type) {
        case QPlatformTheme::MessageDialog:
            return new MessageDialogHelper();
        case QPlatformTheme::FileDialog:
            if (d->flatpakPlatformTheme && d->flatpakPlatformTheme->usePlatformNativeDialog(type)) return d->flatpakPlatformTheme->createPlatformDialogHelper(type);
            return QPlatformTheme::createPlatformDialogHelper(type);
        case QPlatformTheme::ColorDialog:
        case QPlatformTheme::FontDialog:
            return QPlatformTheme::createPlatformDialogHelper(type);
    }
}

QPlatformSystemTrayIcon* PlatformTheme::createPlatformSystemTrayIcon() const {
    return QPlatformTheme::createPlatformSystemTrayIcon();
}

const QPalette* PlatformTheme::palette(QPlatformTheme::Palette type) const {
    //    if (type == QPlatformTheme::SystemPalette) return d->pal;

    //    return QPlatformTheme::palette(type);
    return &d->pal;
}

const QFont* PlatformTheme::font(QPlatformTheme::Font type) const {
    if (d->fonts.contains(type)) {
        return d->fonts.value(type);
    } else {
        return d->fonts.value(QPlatformTheme::SystemFont);
    }

    // return QPlatformTheme::font(type);
}

QVariant PlatformTheme::themeHint(QPlatformTheme::ThemeHint hint) const {
    switch (hint) {
        case QPlatformTheme::SystemIconThemeName:
            return d->settings->value("Platform/icons");
        case QPlatformTheme::SystemIconFallbackThemeName:
            return d->settings->value("Platform/icons.fallback");
        case QPlatformTheme::StyleNames:
            return d->settings->delimitedList("Platform/style");
        case QPlatformTheme::IconThemeSearchPaths:
        case QPlatformTheme::IconFallbackSearchPaths:
            return QStringList({QDir::homePath() + "/.local/share/icons/", "/usr/share/icons", "/var/lib/flatpak/exports/share/icons", QDir::homePath() + "/.local/share/flatpak/exports/share/icons"});
        case QPlatformTheme::KeyboardScheme:
            return KdeKeyboardScheme;
        case QPlatformTheme::SpellCheckUnderlineStyle:
            return QTextCharFormat::SingleUnderline;
        case QPlatformTheme::ItemViewActivateItemOnSingleClick:
        case QPlatformTheme::DialogButtonBoxButtonsHaveIcons:
        case QPlatformTheme::ShowShortcutsInContextMenus:
        case QPlatformTheme::UiEffects:
            return true;
        case QPlatformTheme::PasswordMaskCharacter:
            return QChar(0x2022);
        case QPlatformTheme::CursorFlashTime:
        case QPlatformTheme::KeyboardInputInterval:
        case QPlatformTheme::MouseDoubleClickInterval:
        case QPlatformTheme::StartDragDistance:
        case QPlatformTheme::StartDragTime:
        case QPlatformTheme::KeyboardAutoRepeatRate:
        case QPlatformTheme::PasswordMaskDelay:
        case QPlatformTheme::StartDragVelocity:
        case QPlatformTheme::TextCursorWidth:
        case QPlatformTheme::DropShadow:
        case QPlatformTheme::MaximumScrollBarDragDistance:
        case QPlatformTheme::ToolButtonStyle:
        case QPlatformTheme::ToolBarIconSize:
        case QPlatformTheme::WindowAutoPlacement:
        case QPlatformTheme::DialogButtonBoxLayout:
        case QPlatformTheme::UseFullScreenForPopupMenu:
        case QPlatformTheme::TabFocusBehavior:
        case QPlatformTheme::IconPixmapSizes:
        case QPlatformTheme::DialogSnapToDefaultButton:
        case QPlatformTheme::ContextMenuOnMouseRelease:
        case QPlatformTheme::MousePressAndHoldInterval:
        case QPlatformTheme::MouseDoubleClickDistance:
        case QPlatformTheme::WheelScrollLines:
        case QPlatformTheme::TouchDoubleTapDistance:
        case QPlatformTheme::MouseQuickSelectionThreshold:
        default:
            return QPlatformTheme::themeHint(hint);
    }
}

QPixmap PlatformTheme::standardPixmap(QPlatformTheme::StandardPixmap sp, const QSizeF& size) const {
    return QPlatformTheme::standardPixmap(sp, size);
}

QIcon PlatformTheme::fileIcon(const QFileInfo& fileInfo, QPlatformTheme::IconOptions iconOptions) const {
    QMimeType mt = d->mimeDb.mimeTypeForFile(fileInfo);
    return QIcon::fromTheme(mt.iconName(), QIcon::fromTheme("unknown"));
}

QIconEngine* PlatformTheme::createIconEngine(const QString& iconName) const {
    auto iconEngine = QPlatformTheme::createIconEngine(iconName);
    if (iconEngine) {
        return new IconLoaderEngine(iconName, iconEngine, nullptr);
    }
    return nullptr;
}

QList<QKeySequence> PlatformTheme::keyBindings(QKeySequence::StandardKey key) const {
    return QPlatformTheme::keyBindings(key);
}

QString PlatformTheme::standardButtonText(int button) const {
    return QPlatformTheme::standardButtonText(button);
}

QKeySequence PlatformTheme::standardButtonShortcut(int button) const {
    return QPlatformTheme::standardButtonShortcut(button);
}

void PlatformTheme::updatePalette() {
    QString base = d->settings->value("Palette/base").toString();
    QString accent = d->settings->value("Palette/accent").toString();

    QFile palettesFile(":/thedesk-platform/palettes.json");
    palettesFile.open(QFile::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(palettesFile.readAll());
    QJsonObject rootObj = doc.object();

    QJsonObject baseDefinitions;
    QJsonValue definitions;

    if (rootObj.contains(base)) {
        QJsonObject baseObject = rootObj.value(base).toObject();
        baseDefinitions = baseObject.value("bases").toObject();
        if (baseObject.value("accents").toObject().contains(accent)) {
            definitions = baseObject.value("accents").toObject().value(accent);
        }
    }

    if (definitions.type() != QJsonValue::Null) {
        QJsonObject accentDefinitions = definitions.toObject();

        // Apply the palette
        for (QJsonObject obj : {
                 baseDefinitions, accentDefinitions}) {
            for (auto i = obj.constBegin(); i != obj.constEnd(); i++) {
                PaletteFormat::applyColor(&d->pal, i.key(), i.value());
            }
        }
    }
}

void PlatformTheme::updateFont() {
    QStringList fonts = d->settings->keysInGroup("Fonts");
    for (QString fontTypeStr : fonts) {
        QPlatformTheme::Font fontType = FontFormat::fontForString(fontTypeStr);

        QStringList parts = d->settings->value("Fonts/" + fontTypeStr).toStringList();

        QFont* font;
        if (d->fonts.contains(fontType)) {
            font = d->fonts.value(fontType);
            font->setFamily(parts.at(0));
        } else {
            font = new QFont(parts.at(0));
            d->fonts.insert(fontType, font);
        }

        font->setPointSizeF(parts.at(1).toDouble());
    }
}

PlatformTheme::Appearance PlatformTheme::appearance() const {
    return PlatformTheme::Appearance::Dark;
}
