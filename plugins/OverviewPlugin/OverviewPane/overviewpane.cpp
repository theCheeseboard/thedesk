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
#include "overviewpane.h"
#include "ui_overviewpane.h"

#include "worldclock.h"
#include <QDBusInterface>
#include <QIcon>
#include <QPainter>
#include <QProcess>
#include <QTime>
#include <QTimer>
#include <QToolButton>
#include <QVariantAnimation>
#include <TimeDate/desktoptimedate.h>
#include <Wm/desktopwm.h>
#include <libcontemporary_global.h>
#include <math.h>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <tsettings.h>

struct OverviewPanePrivate {
        OverviewPanePrivate() :
            worldClockSettings("theSuite", "the24"){};

        QToolButton* hamburgerButton;
        QList<WorldClock*> worldClocks;

        tSettings worldClockSettings;
};

OverviewPane::OverviewPane() :
    StatusCenterPane(),
    ui(new Ui::OverviewPane) {
    ui->setupUi(this);

    d = new OverviewPanePrivate();

    d->hamburgerButton = new QToolButton(this);
    d->hamburgerButton->setIcon(QIcon::fromTheme("application-menu"));
    d->hamburgerButton->move(0, 0);
    d->hamburgerButton->setFixedSize(d->hamburgerButton->sizeHint());
    d->hamburgerButton->setVisible(StateManager::statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, d->hamburgerButton, &QToolButton::setVisible);
    connect(d->hamburgerButton, &QToolButton::clicked, StateManager::statusCenterManager(), &StatusCenterManager::showStatusCenterHamburgerMenu);

    ui->dstIcon->setPixmap(QIcon::fromTheme("chronometer").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
    ui->weatherIcon->setPixmap(QIcon::fromTheme("weather-clear").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
    ui->dstPanel->setVisible(false);
    ui->weatherPanel->setVisible(false);

    QTimer* timer = new QTimer();
    timer->setInterval(60000);
    connect(timer, &QTimer::timeout, this, &OverviewPane::updateGreeting);
    timer->start();
    updateGreeting();

    DesktopTimeDate::makeTimeLabel(ui->date, DesktopTimeDate::StandardDate);
    DesktopTimeDate::makeTimeLabel(ui->time, DesktopTimeDate::Time);
    DesktopTimeDate::makeTimeLabel(ui->timeAmPm, DesktopTimeDate::AmPm);

    connect(DesktopTimeDate::timeUpdateTimer(), &QTimer::timeout, this, [=] {
        for (WorldClock* wc : d->worldClocks) {
            wc->updateClock();
        }
    });

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->mainWidget->setFixedWidth(contentWidth);
    ui->scrollAreaWidgetContents->installEventFilter(this);

    connect(&d->worldClockSettings, &tSettings::settingChanged, this, [=](QString key, QVariant value) {
        if (key == "WorldClock/timezones") {
            updateWorldClocks();
        }
    });
    updateWorldClocks();
    updateDSTNotification();
}

OverviewPane::~OverviewPane() {
    delete d;
    delete ui;
}

void OverviewPane::updateGreeting() {
    QString userDisplayName = DesktopWm::userDisplayName();

    QTime now = QTime::currentTime();
    if (now.hour() < 6) {
        ui->greetingLabel->setText(tr("Hi %1!").arg(userDisplayName));
    } else if (now.hour() < 12) {
        ui->greetingLabel->setText(tr("Good morning, %1!").arg(userDisplayName));
    } else if (now.hour() < 17) {
        ui->greetingLabel->setText(tr("Good afternoon, %1!").arg(userDisplayName));
    } else {
        ui->greetingLabel->setText(tr("Good evening, %1!").arg(userDisplayName));
    }
}

void OverviewPane::updateWorldClocks() {
    for (WorldClock* wc : d->worldClocks) {
        ui->worldClocksLayout->removeWidget(wc);
        wc->deleteLater();
    }

    d->worldClocks.clear();

    for (QString tz : d->worldClockSettings.delimitedList("WorldClock/timezones").mid(0, 4)) {
        WorldClock* wc = new WorldClock(QTimeZone(tz.toUtf8()));
        ui->worldClocksLayout->addWidget(wc);
        d->worldClocks.append(wc);
    }
}

void OverviewPane::updateDSTNotification() {
    QDBusInterface dateTimeInterface("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", QDBusConnection::systemBus());
    QString currentTimezone = dateTimeInterface.property("Timezone").toString();

    QString timezoneInfoPath = "/usr/share/zoneinfo/" + currentTimezone;
    QProcess* timezoneProcess = new QProcess();
    connect(timezoneProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus) {
        timezoneProcess->deleteLater();

        struct Changeover {
                QDateTime changeoverDate;
                bool isDST;
                int gmtOffset;
        };

        QList<Changeover> changeovers;

        while (!timezoneProcess->atEnd()) {
            QStringList parts = QString(timezoneProcess->readLine()).split(" ", Qt::SkipEmptyParts);
            if (parts.length() == 16) {
                QStringList dateText;

                const QStringList months = {
                    "Jan",
                    "Feb",
                    "Mar",
                    "Apr",
                    "May",
                    "Jun",
                    "Jul",
                    "Aug",
                    "Sep",
                    "Oct",
                    "Nov",
                    "Dec"};
                dateText.append(parts.at(3));
                dateText.append(QString::number(months.indexOf(parts.at(2)) + 1).rightJustified(2, '0'));
                dateText.append(parts.at(5));
                dateText.append(parts.at(4));

                Changeover c;
                QString dateConnectedText = dateText.join(".");

                QDate date = QDate(parts.at(5).toInt(), months.indexOf(parts.at(2)) + 1, parts.at(3).toInt());
                QTime time = QTime::fromString(parts.at(4), "hh:mm:ss");
                c.changeoverDate = QDateTime(date, time, Qt::LocalTime);
                c.changeoverDate.setTimeSpec(Qt::UTC);
                c.isDST = parts.at(14).endsWith("1");
                c.gmtOffset = parts.at(15).mid(7).toInt();
                changeovers.append(c);
            }
        }

        bool showDaylightSavingsPanel = false;
        Changeover changeover;
        QDateTime current = QDateTime::currentDateTimeUtc();
        QDateTime currentLocal = QDateTime::currentDateTime();

        for (int i = 0; i < changeovers.count(); i++) {
            Changeover c = changeovers.at(i);

            int days = current.daysTo(c.changeoverDate);
            if (days > 0 && days < 14) {
                if ((currentLocal.isDaylightTime() && !c.isDST) || (!currentLocal.isDaylightTime() && c.isDST)) {
                    showDaylightSavingsPanel = true;
                    changeover = c;
                }
            }
        }

        if (showDaylightSavingsPanel) {
            ui->dstPanel->setVisible(true);

            if (currentLocal.isDaylightTime()) {
                ui->dstLabel->setText(tr("Daylight Savings Time is ending on %2. The clock will automatically shift backwards by %n hour(s).", nullptr, 1));
            } else {
                ui->dstLabel->setText(tr("Daylight Savings Time is starting on %2. The clock will automatically shift forwards by %n hour(s).", nullptr, 1));
            }
        } else {
            ui->dstPanel->setVisible(false);
        }
    });
    timezoneProcess->start("zdump", {"-v", currentTimezone});
}

void OverviewPane::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}

bool OverviewPane::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->scrollAreaWidgetContents && event->type() == QEvent::Paint) {
        QPainter p(ui->scrollAreaWidgetContents);
        p.setRenderHint(QPainter::Antialiasing);

        QColor newTextColor;

        QColor top, bottom;
        QTime now = QTime::currentTime();

        /*if (currentCondition.isValid && currentCondition.properties.value(WeatherCondition::isCloudy, false).toBool()) {
            top = QColor(150, 150, 150);
            bottom = QColor(100, 100, 100);
            newTextColor = QColor(Qt::black);
        } else*/
        if (now.hour() < 4 || now.hour() > 20) { // Assume night
            top = QColor(0, 36, 85);
            bottom = QColor(0, 17, 40);
            newTextColor = QColor(Qt::white);
        } else if (now.hour() > 8 && now.hour() < 16) { // Assume day
            top = QColor(126, 195, 255);
            bottom = QColor(64, 149, 185);
            newTextColor = QColor(Qt::black);
        } else { // Calculate interpolation
            int interpolation;
            // From 4-8 interpolate sunrise
            if (now.hour() > 4 && now.hour() < 8) {
                interpolation = now.msecsSinceStartOfDay() - 14400000; // 4 hours in milliseconds
            } else {
                interpolation = 14400000 - (now.msecsSinceStartOfDay() - 57600000); // 16 hours in milliseconds
            }

            QVariantAnimation a;
            a.setDuration(14400000);
            a.setCurrentTime(interpolation);
            a.setStartValue(QColor(0, 36, 85));
            a.setKeyValueAt(0.5, QColor(255, 140, 0));
            a.setEndValue(QColor(126, 195, 255));
            top = a.currentValue().value<QColor>();
            a.setStartValue(QColor(0, 17, 40));
            a.setKeyValueAt(0.5, QColor(167, 70, 25));
            a.setEndValue(QColor(64, 149, 185));
            bottom = a.currentValue().value<QColor>();

            bool dark = ((top.red() + top.green() + top.blue()) / 3) < 127;
            if (dark) {
                newTextColor = QColor(Qt::white);
            } else {
                newTextColor = QColor(Qt::black);
            }
        }

        QLinearGradient mainBackground;
        mainBackground.setStart(0, 0);
        mainBackground.setFinalStop(0, 500);
        mainBackground.setColorAt(0, top);
        mainBackground.setColorAt(1, bottom);

        p.setBrush(mainBackground);
        p.setPen(Qt::transparent);
        p.drawRect(0, 0, ui->scrollAreaWidgetContents->width(), ui->scrollAreaWidgetContents->height());

        // Draw background objects if neccessary
        //     if (currentCondition.isValid) {
        //         if (currentCondition.properties.value(WeatherCondition::isRainy, false).toBool()) drawRaindrops(&p);
        //         if (currentCondition.properties.value(WeatherCondition::WindBeaufort, 0).toInt() >= 4) drawWind(&p);
        //     }
        //     drawObjects(&p);

        // Draw celestial object if neccessary
        int daySegmentPassed = -1;
        bool isMoon = true;

        if (now.hour() < 5 || (now.hour() == 5 && now.minute() <= 30)) {                                                                         // Draw moon
            daySegmentPassed = now.msecsSinceStartOfDay() + 19800000;                                                                            // 5.5 hours in milliseconds
        } else if (now.hour() > 18 || (now.hour() == 18 && now.minute() >= 30)) {                                                                // Draw moon
            daySegmentPassed = now.msecsSinceStartOfDay() - 66600000;                                                                            // 18.5 hours in milliseconds
        } else if ((now.hour() > 6 && now.hour() < 17) || (now.hour() == 6 && now.minute() >= 30) || (now.hour() == 17 && now.minute() <= 30)) { // Draw sun
            daySegmentPassed = now.msecsSinceStartOfDay() - 23400000;                                                                            // 6.5 hours in milliseconds
            isMoon = false;
        }

        if (isMoon) {
            p.setBrush(QColor(127, 127, 127));
        } else {
            p.setBrush(QColor(255, 224, 130));
        }
        if (daySegmentPassed >= 0) {
            double percentageThroughArc = daySegmentPassed / 39600000.0; // 11 hours in milliseconds

            double sinArg = percentageThroughArc * M_PI;
            double heightDisplacement = -sin(sinArg) + 1;

            int top = static_cast<int>((heightDisplacement * 100) + 70);
            int left = static_cast<int>((ui->scrollAreaWidgetContents->width() + 100) * percentageThroughArc - 50);

            QPoint center(left, top);
            if (isMoon) {
                p.drawEllipse(center, 30, 30);
            } else {
                p.drawEllipse(center, 50, 50);
            }
        }

        QPalette pal = ui->scrollAreaWidgetContents->palette();
        pal.setColor(QPalette::WindowText, newTextColor);
        ui->scrollAreaWidgetContents->setPalette(pal);
    }
    return false;
}

QString OverviewPane::name() {
    return "OverviewPane";
}

QString OverviewPane::displayName() {
    return tr("Overview");
}

QIcon OverviewPane::icon() {
    return QIcon::fromTheme("thedesk-overview", QIcon::fromTheme("weather-clear"));
}

QWidget* OverviewPane::leftPane() {
    return nullptr;
}
