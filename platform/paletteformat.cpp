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

#include <paletteformat.h>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

const QMap<QString, QPalette::ColorRole> roles = {
    {"window",          QPalette::Window         },
    {"windowText",      QPalette::WindowText     },
    {"base",            QPalette::Base           },
    {"altBase",         QPalette::AlternateBase  },
    {"alternateBase",   QPalette::AlternateBase  },
    {"text",            QPalette::Text           },
    {"brightText",      QPalette::BrightText     },
    {"button",          QPalette::Button         },
    {"buttonText",      QPalette::ButtonText     },
    {"highlight",       QPalette::Highlight      },
    {"highlightText",   QPalette::HighlightedText},
    {"highlightedText", QPalette::HighlightedText},
    {"link",            QPalette::Link           },
    {"linkVisited",     QPalette::LinkVisited    },
    {"tooltip",         QPalette::ToolTipBase    },
    {"tooltipBase",     QPalette::ToolTipBase    },
    {"tooltipText",     QPalette::ToolTipText    },
    {"placeholderText", QPalette::PlaceholderText},
    {"light",           QPalette::Light          },
    {"midLight",        QPalette::Midlight       },
    {"mid",             QPalette::Mid            },
    {"dark",            QPalette::Dark           },
    {"shadow",          QPalette::Shadow         }
};

const QMap<QString, QPalette::ColorGroup> groups = {
    {"normal",   QPalette::Normal  },
    {"inactive", QPalette::Inactive},
    {"disabled", QPalette::Disabled}
};

QPalette::ColorRole PaletteFormat::roleForString(QString string) {
    return roles.value(string);
}

void PaletteFormat::applyColor(QPalette* pal, QString key, QJsonValue value) {
    QPalette::ColorRole role = PaletteFormat::roleForString(key);

    if (value.type() == QJsonValue::Object) {
        QJsonObject obj = value.toObject();
        for (auto i = obj.constBegin(); i != obj.constEnd(); i++) {
            QPalette::ColorGroup group = PaletteFormat::groupForString(i.key());
            pal->setColor(group, role, PaletteFormat::colorFromValue(i.value()));
        }
    } else {
        pal->setColor(role, PaletteFormat::colorFromValue(value));
    }
}

QColor PaletteFormat::colorFromValue(QJsonValue value) {
    switch (value.type()) {
        case QJsonValue::Double:
            {
                // Grayscale value
                int v = value.toInt();
                return QColor(v, v, v);
            }
        case QJsonValue::String:
            // Name color
            return QColor(value.toString());
        case QJsonValue::Array:
            {
                // RGB Triplet
                QJsonArray array = value.toArray();
                return QColor(array.at(0).toInt(), array.at(1).toInt(), array.at(2).toInt());
            }
        default:
            //??? invalid format
            return QColor();
    }
}

QPalette::ColorGroup PaletteFormat::groupForString(QString string) {
    return groups.value(string);
}
