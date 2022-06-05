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
#ifndef PLATFORMTHEME_H
#define PLATFORMTHEME_H

#include <qpa/qplatformtheme.h>

struct PlatformThemePrivate;
class PlatformTheme : public QPlatformTheme {
    public:
        explicit PlatformTheme();
        ~PlatformTheme();

        // QPlatformTheme interface
    public:
        QPlatformMenuItem* createPlatformMenuItem() const;
        QPlatformMenu* createPlatformMenu() const;
        QPlatformMenuBar* createPlatformMenuBar() const;
        void showPlatformMenuBar();
        bool usePlatformNativeDialog(DialogType type) const;
        QPlatformDialogHelper* createPlatformDialogHelper(DialogType type) const;
        QPlatformSystemTrayIcon* createPlatformSystemTrayIcon() const;
        const QPalette* palette(Palette type) const;
        const QFont* font(Font type) const;
        QVariant themeHint(ThemeHint hint) const;
        QPixmap standardPixmap(StandardPixmap sp, const QSizeF& size) const;
        QIcon fileIcon(const QFileInfo& fileInfo, QPlatformTheme::IconOptions iconOptions) const;
        QIconEngine* createIconEngine(const QString& iconName) const;
        QList<QKeySequence> keyBindings(QKeySequence::StandardKey key) const;
        QString standardButtonText(int button) const;
        QKeySequence standardButtonShortcut(int button) const;
        Appearance appearance() const;

    private:
        PlatformThemePrivate* d;

        void updatePalette();
        void updateFont();
};

#endif // PLATFORMTHEME_H
