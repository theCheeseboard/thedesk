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
#include "wirelessnetworklistdelegate.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include <QPainter>
#include <libcontemporary_global.h>

#include "networkplugincommon.h"

struct WirelessNetworkListDelegatePrivate {
        NetworkManager::WirelessDevice::Ptr device;

        struct Rects {
                QRect iconRect;
                QRect textRect;
                QRect descRect;

                Rects(const QStyleOptionViewItem& option) {
                    iconRect.setLeft(option.rect.left() + SC_DPI(6));
                    iconRect.setTop(option.rect.top() + SC_DPI(6));
                    iconRect.setBottom(iconRect.top() + SC_DPI(32));
                    iconRect.setRight(iconRect.left() + SC_DPI(32));

                    textRect.setLeft(iconRect.right() + SC_DPI(6));
                    textRect.setTop(option.rect.top() + SC_DPI(6));
                    textRect.setBottom(option.rect.top() + option.fontMetrics.height() + SC_DPI(6));
                    textRect.setRight(option.rect.right());

                    descRect.setLeft(iconRect.right() + SC_DPI(6));
                    descRect.setTop(option.rect.top() + option.fontMetrics.height() + SC_DPI(8));
                    descRect.setBottom(option.rect.top() + option.fontMetrics.height() * 2 + SC_DPI(6));
                    descRect.setRight(option.rect.right());
                }
        };
};

WirelessNetworkListDelegate::WirelessNetworkListDelegate(QString deviceUni, QObject* parent) :
    QAbstractItemDelegate(parent) {
    d = new WirelessNetworkListDelegatePrivate();
    d->device = NetworkManager::findNetworkInterface(deviceUni).staticCast<NetworkManager::WirelessDevice>();
}

WirelessNetworkListDelegate::~WirelessNetworkListDelegate() {
    delete d;
}

void WirelessNetworkListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->setFont(option.font);
    painter->setLayoutDirection(option.direction);

    WirelessNetworkListDelegatePrivate::Rects rects(option);
    QString text = index.data().toString();
    QString desc;
    QIcon icon;

    QVariant networkInformation = index.data(Qt::UserRole);
    if (networkInformation.canConvert<NetworkManager::Connection::Ptr>()) {
        NetworkManager::Connection::Ptr connection = networkInformation.value<NetworkManager::Connection::Ptr>();
        QString targetSsid = connection->settings()->setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>()->ssid();
        for (QString apPath : d->device->accessPoints()) {
            NetworkManager::AccessPoint::Ptr ap(new NetworkManager::AccessPoint(apPath));
            if (ap->ssid() == targetSsid) {
                if (d->device->activeAccessPoint() && ap->uni() == d->device->activeAccessPoint()->uni()) {
                    desc = tr("Connected");
                    icon = QIcon::fromTheme(NetworkPluginCommon::iconForSignalStrength(ap->signalStrength(), NetworkPluginCommon::WiFi));
                    break;
                } else {
                    desc = tr("In Range");
                    icon = QIcon::fromTheme(NetworkPluginCommon::iconForSignalStrength(ap->signalStrength(), NetworkPluginCommon::WiFi));
                    break;
                }
            }
        }

        if (desc.isEmpty()) {
            desc = tr("Out of range");
        }
    } else if (networkInformation.canConvert<NetworkManager::AccessPoint::Ptr>()) {
        NetworkManager::AccessPoint::Ptr ap = networkInformation.value<NetworkManager::AccessPoint::Ptr>();
        icon = QIcon::fromTheme(NetworkPluginCommon::iconForSignalStrength(ap->signalStrength(), NetworkPluginCommon::WiFi));

        NetworkManager::WirelessSecurityType security = NetworkManager::findBestWirelessSecurity(d->device->wirelessCapabilities(), true, false, ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());
        switch (security) {
            case NetworkManager::UnknownSecurity:
            case NetworkManager::NoneSecurity:
                desc = tr("Not Secured");
                break;
            case NetworkManager::StaticWep:
                desc = tr("Secured with Static WEP");
                break;
            case NetworkManager::DynamicWep:
                desc = tr("Secured with Dynamic WEP");
                break;
            case NetworkManager::Leap:
                desc = tr("Secured with LEAP");
                break;
            case NetworkManager::WpaPsk:
                desc = tr("Secured with WPA-PSK");
                break;
            case NetworkManager::WpaEap:
                desc = tr("Secured with WPA Enterprise");
                break;
            case NetworkManager::Wpa2Psk:
                desc = tr("Secured with WPA2-PSK");
                break;
            case NetworkManager::Wpa2Eap:
                desc = tr("Secured with WPA2 Enterprise");
                break;
            case NetworkManager::SAE:
                desc = tr("Secured with WPA3");
                break;
        }
    }

    if (option.direction == Qt::RightToLeft) {
        rects.iconRect.moveRight(option.rect.right() - SC_DPI(6));
        rects.textRect.moveRight(rects.iconRect.left() - SC_DPI(6));
        rects.descRect.moveRight(rects.iconRect.left() - SC_DPI(6));
    }

    if (option.state & QStyle::State_Selected) {
        painter->setPen(Qt::transparent);
        painter->setBrush(option.palette.color(QPalette::Highlight));
        painter->drawRect(option.rect);
        painter->setBrush(Qt::transparent);
        painter->setPen(option.palette.color(QPalette::HighlightedText));
        painter->drawText(rects.textRect, Qt::AlignLeading, text);
        painter->drawText(rects.descRect, Qt::AlignLeading, desc);
    } else if (option.state & QStyle::State_MouseOver) {
        QColor col = option.palette.color(QPalette::Highlight);
        col.setAlpha(127);
        painter->setBrush(col);
        painter->setPen(Qt::transparent);
        painter->drawRect(option.rect);
        painter->setBrush(Qt::transparent);
        painter->setPen(option.palette.color(QPalette::WindowText));
        painter->drawText(rects.textRect, Qt::AlignLeading, text);
        painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
        painter->drawText(rects.descRect, Qt::AlignLeading, desc);
    } else {
        painter->setPen(option.palette.color(QPalette::WindowText));
        painter->drawText(rects.textRect, Qt::AlignLeading, text);
        painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
        painter->drawText(rects.descRect, Qt::AlignLeading, desc);
    }

    if (!icon.isNull()) {
        painter->drawPixmap(rects.iconRect, icon.pixmap(rects.iconRect.size()));
    }
}

QSize WirelessNetworkListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    WirelessNetworkListDelegatePrivate::Rects rects(option);

    QRect u = rects.iconRect;
    u = u.united(rects.textRect);
    u = u.united(rects.descRect);
    u.adjust(SC_DPI(-6), SC_DPI(-6), SC_DPI(6), SC_DPI(6));

    return u.size();
}
