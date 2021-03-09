/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "appsearchprovider.h"

#include <QPainter>
#include <Applications/application.h>

struct AppSearchProviderPrivate {

};

AppSearchProvider::AppSearchProvider(QObject* parent) : GatewaySearchProvider(parent) {
    d = new AppSearchProviderPrivate();
}

AppSearchProvider::~AppSearchProvider() {
    delete d;
}

tPromise<QList<QVariantMap>>* AppSearchProvider::searchResults(QString query) {
    return TPROMISE_CREATE_SAME_THREAD(QList<QVariantMap>, {
        Q_UNUSED(rej)

        QList<QVariantMap> results;

        for (const QString& desktopEntry : Application::allApplications()) {
            ApplicationPointer a(new Application(desktopEntry));

            //Make sure this app is good to be shown
            if (a->getProperty("Type", "").toString() != "Application") continue;
            if (a->getProperty("NoDisplay", false).toBool()) continue;
            if (!a->getStringList("OnlyShowIn", {"thedesk"}).contains("thedesk")) continue;
            if (a->getStringList("NotShowIn").contains("thedesk")) continue;

            QStringList possibleWords;
            possibleWords.append(a->getProperty("Name").toString());
            possibleWords.append(a->getProperty("GenericName").toString());
            possibleWords.append(a->getStringList("Keywords"));

            for (QString s : possibleWords) {
                if (s.contains(query, Qt::CaseInsensitive)) {
                    results.append({
                        {"application", a->desktopEntry()},
                        {"mainText", a->getProperty("Name").toString()},
                        {"priority", 0}
                    });
                    break;
                }
            }
        }

//                                          if (theLibsGlobal::searchInPath(query.split(" ")[0]).count() > 0) {
//                                              d->appsShown.append(ApplicationPointer(new Application({
//                                                  {"Name", query},
//                                                  {"Exec", query},
//                                                  {"GenericName", tr("Run Command")},
//                                                  {"Icon", "system-run"}
//                                              })));
//                                          }

        res(results);
    });
}

void AppSearchProvider::launch(QVariantMap data) {
    Application(data.value("application").toString()).launch();
}

void AppSearchProvider::paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariantMap data) const {
    Application app(data.value("application").toString());
    QString name = app.getProperty("Name").toString();
    QString genericName = app.getProperty("GenericName", tr("Application")).toString();
    QPixmap icon = app.icon(SC_DPI_T(QSize(32, 32), QSize));

    painter->setFont(option.font);
    painter->setLayoutDirection(option.direction);

    QRect iconRect;
    iconRect.setLeft(option.rect.left() + SC_DPI(6));
    iconRect.setTop(option.rect.top() + SC_DPI(6));
    iconRect.setBottom(iconRect.top() + SC_DPI(32));
    iconRect.setRight(iconRect.left() + SC_DPI(32));

    QRect textRect;
    textRect.setLeft(iconRect.right() + SC_DPI(6));
    textRect.setTop(option.rect.top() + SC_DPI(6));
    textRect.setBottom(option.rect.top() + option.fontMetrics.height() + SC_DPI(6));
    textRect.setRight(option.rect.right());

    QRect descRect;
    descRect.setLeft(iconRect.right() + SC_DPI(6));
    descRect.setTop(option.rect.top() + option.fontMetrics.height() + SC_DPI(8));
    descRect.setBottom(option.rect.top() + option.fontMetrics.height() * 2 + SC_DPI(6));
    descRect.setRight(option.rect.right());

    if (option.direction == Qt::RightToLeft) {
        iconRect.moveRight(option.rect.right() - SC_DPI(6));
        textRect.moveRight(iconRect.left() - SC_DPI(6));
        descRect.moveRight(iconRect.left() - SC_DPI(6));
    }

    if (option.state & QStyle::State_Selected) {
        painter->setPen(Qt::transparent);
        painter->setBrush(option.palette.color(QPalette::Highlight));
        painter->drawRect(option.rect);
        painter->setBrush(Qt::transparent);
        painter->setPen(option.palette.color(QPalette::HighlightedText));
        painter->drawText(textRect, Qt::AlignLeading, name);
        painter->drawText(descRect, Qt::AlignLeading, genericName);
    } else if (option.state & QStyle::State_MouseOver) {
        QColor col = option.palette.color(QPalette::Highlight);
        col.setAlpha(127);
        painter->setBrush(col);
        painter->setPen(Qt::transparent);
        painter->drawRect(option.rect);
        painter->setBrush(Qt::transparent);
        painter->setPen(option.palette.color(QPalette::WindowText));
        painter->drawText(textRect, Qt::AlignLeading, name);
        painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
        painter->drawText(descRect, Qt::AlignLeading, genericName);
    } else {
        painter->setPen(option.palette.color(QPalette::WindowText));
        painter->drawText(textRect, Qt::AlignLeading, name);
        painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
        painter->drawText(descRect, Qt::AlignLeading, genericName);
    }
    painter->drawPixmap(iconRect, icon);

    if (data.value("drawArrows", true).toBool()) {
        if (app.getStringList("Actions").count() > 0) { //Actions included
            QRect actionsRect;
            actionsRect.setWidth(SC_DPI(16));
            actionsRect.setHeight(SC_DPI(16));
            actionsRect.moveRight(option.rect.right() - SC_DPI(9));
            actionsRect.moveTop(option.rect.top() + option.rect.height() / 2 - SC_DPI(8));

            if (option.direction == Qt::RightToLeft) {
                actionsRect.moveLeft(option.rect.left() + SC_DPI(9));
            }

            painter->drawPixmap(actionsRect, QIcon::fromTheme("arrow-right").pixmap(SC_DPI(16), SC_DPI(16)));
        }
    }
}

QSize AppSearchProvider::sizeHint(const QStyleOptionViewItem& option, const QVariantMap data) const {
    Application app(data.value("application").toString());
    int fontHeight = option.fontMetrics.height() * 2 + SC_DPI(14);
    int iconHeight = SC_DPI(46);

    return QSize(option.fontMetrics.horizontalAdvance(app.getProperty("Name").toString()), qMax(fontHeight, iconHeight));
}
