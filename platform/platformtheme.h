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
        QPlatformMenuItem* createPlatformMenuItem() const override;
        QPlatformMenu* createPlatformMenu() const override;
        QPlatformMenuBar* createPlatformMenuBar() const override;
        void showPlatformMenuBar() override;
        bool usePlatformNativeDialog(DialogType type) const override;
        QPlatformDialogHelper* createPlatformDialogHelper(DialogType type) const override;
        QPlatformSystemTrayIcon* createPlatformSystemTrayIcon() const override;
        const QPalette* palette(Palette type) const override;
        const QFont* font(Font type) const override;
        QVariant themeHint(ThemeHint hint) const override;
        QPixmap standardPixmap(StandardPixmap sp, const QSizeF& size) const override;
        QIcon fileIcon(const QFileInfo& fileInfo, QPlatformTheme::IconOptions iconOptions) const override;
        QIconEngine* createIconEngine(const QString& iconName) const override;
        QList<QKeySequence> keyBindings(QKeySequence::StandardKey key) const override;
        QString standardButtonText(int button) const override;
        QKeySequence standardButtonShortcut(int button) const override;
        Appearance appearance() const override;

    private:
        PlatformThemePrivate* d;

        void updatePalette();
        void updateFont();
};

#endif // PLATFORMTHEME_H
