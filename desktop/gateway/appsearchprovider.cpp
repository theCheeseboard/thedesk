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
#include <tpaintcalculator.h>
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

            for (const QString& s : possibleWords) {
                if (s.contains(query, Qt::CaseInsensitive)) {
                    results.append({
                        {"application", a->desktopEntry()},
                        {"mainText", a->getProperty("Name").toString()},
                        {"priority", 0}
                    });

                    for (QString action : a->getStringList("Actions")) {
                        results.append({
                            {"application", a->desktopEntry()},
                            {"action", action},
                            {"mainText", a->getProperty("Name").toString()},
                            {"priority", 0}
                        });
                    }
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
    Application app(data.value("application").toString());
    if (data.contains("action")) {
        app.launchAction(data.value("action").toString());
    } else {
        app.launch();
    }
}

void AppSearchProvider::paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariantMap data) const {
    tPaintCalculator calculator = this->calculator(painter, option, data);
    calculator.setPainter(painter);

    painter->setFont(option.font);

    calculator.performPaint();
}

QSize AppSearchProvider::sizeHint(const QStyleOptionViewItem& option, const QVariantMap data) const {
//    Application app(data.value("application").toString());
//    int fontHeight = option.fontMetrics.height() * 2 + SC_DPI(14);
//    int iconHeight = SC_DPI(46);

//    return QSize(option.fontMetrics.horizontalAdvance(app.getProperty("Name").toString()), qMax(fontHeight, iconHeight));
    return calculator(nullptr, option, data).sizeWithMargins().toSize();
}

tPaintCalculator AppSearchProvider::calculator(QPainter* painter, const QStyleOptionViewItem& option, const QVariantMap data) const {
    Application app(data.value("application").toString());

    tPaintCalculator calculator;
    calculator.setDrawBounds(option.rect);
    calculator.setLayoutDirection(option.direction);

    QString name;
    QString genericName;
    QPixmap icon;

    QRect iconRect;
    QRect textRect;
    QRect descRect;

    if (data.contains("action")) {
        QString action = data.value("action").toString();
        name = app.getActionProperty(action, "Name").toString();
        icon = app.icon(SC_DPI_T(QSize(16, 16), QSize));

        iconRect.setLeft(option.rect.left() + SC_DPI(12));
        iconRect.setTop(option.rect.top() + SC_DPI(6));
        iconRect.setSize(icon.size());

        textRect.setHeight(option.fontMetrics.height());
        textRect.setWidth(option.fontMetrics.horizontalAdvance(name) + 1);
        textRect.moveCenter(iconRect.center());
        textRect.moveLeft(iconRect.right() + SC_DPI(6));
    } else {
        name = app.getProperty("Name").toString();
        genericName = app.getProperty("GenericName", tr("Application")).toString();
        icon = app.icon(SC_DPI_T(QSize(32, 32), QSize));

        iconRect.setLeft(option.rect.left() + SC_DPI(6));
        iconRect.setTop(option.rect.top() + SC_DPI(6));
        iconRect.setSize(icon.size());

        textRect.setLeft(iconRect.right() + SC_DPI(6));
        textRect.setTop(option.rect.top() + SC_DPI(6));
        textRect.setBottom(option.rect.top() + option.fontMetrics.height() + SC_DPI(6));
        textRect.setRight(option.rect.right());

        descRect.setLeft(iconRect.right() + SC_DPI(6));
        descRect.setTop(option.rect.top() + option.fontMetrics.height() + SC_DPI(8));
        descRect.setBottom(option.rect.top() + option.fontMetrics.height() * 2 + SC_DPI(6));
        descRect.setRight(option.rect.right());
    }


    if (option.state & QStyle::State_Selected) {
        calculator.addRect(option.rect, [ = ](QRectF drawBounds) {
            painter->setPen(Qt::transparent);
            painter->setBrush(option.palette.color(QPalette::Highlight));
            painter->drawRect(drawBounds);
        });
        calculator.addRect(textRect, [ = ](QRectF drawBounds) {
            painter->setBrush(Qt::transparent);
            painter->setPen(option.palette.color(QPalette::HighlightedText));
            painter->drawText(drawBounds, Qt::AlignLeading, name);
        });
        calculator.addRect(descRect, [ = ](QRectF drawBounds) {
            painter->drawText(drawBounds, Qt::AlignLeading, genericName);
        });
    } else if (option.state & QStyle::State_MouseOver) {
        calculator.addRect(option.rect, [ = ](QRectF drawBounds) {
            QColor col = option.palette.color(QPalette::Highlight);
            col.setAlpha(127);
            painter->setBrush(col);
            painter->setPen(Qt::transparent);
            painter->drawRect(drawBounds);
        });
        calculator.addRect(textRect, [ = ](QRectF drawBounds) {
            painter->setBrush(Qt::transparent);
            painter->setPen(option.palette.color(QPalette::WindowText));
            painter->drawText(drawBounds, Qt::AlignLeading, name);
        });
        calculator.addRect(descRect, [ = ](QRectF drawBounds) {
            painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
            painter->drawText(drawBounds, Qt::AlignLeading, genericName);
        });
    } else {
        calculator.addRect(textRect, [ = ](QRectF drawBounds) {
            painter->setPen(option.palette.color(QPalette::WindowText));
            painter->drawText(drawBounds, Qt::AlignLeading, name);
        });
        calculator.addRect(descRect, [ = ](QRectF drawBounds) {
            painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
            painter->drawText(drawBounds, Qt::AlignLeading, genericName);
        });
    }

    calculator.addRect(iconRect, [ = ](QRectF drawBounds) {
        painter->drawPixmap(drawBounds.toRect(), icon);
    });


    if (data.value("drawArrows", true).toBool()) {
        if (app.getStringList("Actions").count() > 0) { //Actions included
            QRect actionsRect;
            actionsRect.setWidth(SC_DPI(16));
            actionsRect.setHeight(SC_DPI(16));
            actionsRect.moveCenter(iconRect.center());
            actionsRect.moveRight(option.rect.right() - SC_DPI(9));

            calculator.addRect(actionsRect, [ = ](QRectF drawBounds) {
                painter->drawPixmap(drawBounds.toRect(), QIcon::fromTheme("arrow-right").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            });
        }
    }

    return calculator;
}
