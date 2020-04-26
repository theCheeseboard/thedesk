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
#include "fontformat.h"

#include <QMap>

const QMap<QString, QPlatformTheme::Font> fontTypes = {
    {"system", QPlatformTheme::SystemFont},
    {"menu", QPlatformTheme::MenuFont},
    {"menubar", QPlatformTheme::MenuBarFont},
    {"menuItem", QPlatformTheme::MenuItemFont},
    {"messageBox", QPlatformTheme::MessageBoxFont},
    {"label", QPlatformTheme::LabelFont},
    {"tipLabel", QPlatformTheme::TipLabelFont},
    {"statusBar", QPlatformTheme::StatusBarFont},
    {"titleBar", QPlatformTheme::TitleBarFont},
    {"titleBar.mdi", QPlatformTheme::MdiSubWindowTitleFont},
    {"titleBar.dock", QPlatformTheme::DockWidgetTitleFont},
    {"button", QPlatformTheme::PushButtonFont},
    {"button.tool", QPlatformTheme::ToolButtonFont},
    {"button.tab", QPlatformTheme::TabButtonFont},
    {"checkbox", QPlatformTheme::CheckBoxFont},
    {"radio", QPlatformTheme::RadioButtonFont},
    {"view.item", QPlatformTheme::ItemViewFont},
    {"view.list", QPlatformTheme::ListViewFont},
    {"view.header", QPlatformTheme::HeaderViewFont},
    {"combo", QPlatformTheme::ListBoxFont},
    {"combo.item", QPlatformTheme::ComboMenuItemFont},
    {"combo.edit", QPlatformTheme::ComboLineEditFont},
    {"small", QPlatformTheme::SmallFont},
    {"mini", QPlatformTheme::MiniFont},
    {"groupboxTitle", QPlatformTheme::GroupBoxTitleFont},
    {"editor", QPlatformTheme::EditorFont},
    {"fixed", QPlatformTheme::FixedFont}
};

QPlatformTheme::Font FontFormat::fontForString(QString string) {
    return fontTypes.value(string);
}
