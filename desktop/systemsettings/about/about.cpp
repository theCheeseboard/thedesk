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
#include "about.h"
#include "ui_about.h"

#include <QIcon>
#include <QFile>
#include <QBuffer>
#include <QDBusInterface>
#include <QHostInfo>
#include <statemanager.h>
#include <statuscentermanager.h>
#include <sys/sysinfo.h>
#include <tpopover.h>
#include <tlogger.h>
#include "changehostnamepopover.h"
#include "acknowledgements.h"

struct AboutPrivate {
    QDBusInterface* hostnamed;
};

About::About() :
    StatusCenterPane(),
    ui(new Ui::About) {
    ui->setupUi(this);

    d = new AboutPrivate();
    d->hostnamed = new QDBusInterface("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.hostname1", QDBusConnection::systemBus());

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    //Get distribution information
    QString osreleaseFile = "";
    if (QFile("/etc/os-release").exists()) {
        osreleaseFile = "/etc/os-release";
    } else if (QFile("/usr/lib/os-release").exists()) {
        osreleaseFile = "/usr/lib/os-release";
    }

    if (osreleaseFile != "") {
        QMap<QString, QString> values;

        QFile information(osreleaseFile);
        information.open(QFile::ReadOnly);

        while (!information.atEnd()) {
            QString line = information.readLine().trimmed();
            int equalsIndex = line.indexOf("=");

            QString key = line.left(equalsIndex);
            QString value = line.mid(equalsIndex + 1);
            if (value.startsWith("\"") && value.endsWith("\"")) value = value.mid(1, value.count() - 2);
            values.insert(key, value);
        }
        information.close();

        ui->distroName->setText(values.value("PRETTY_NAME", tr("Unknown")));
        ui->distroWebpage->setText(values.value("HOME_URL", tr("Unknown")));
        ui->distroSupport->setText(values.value("SUPPORT_URL", tr("Unknown")));
    }

    struct sysinfo* info = new struct sysinfo;
    if (sysinfo(info) == 0) {
        ui->availableMemory->setText(QLocale().formattedDataSize(static_cast<qint64>(info->totalram)));
        ui->availableSwap->setText(QLocale().formattedDataSize(static_cast<qint64>(info->totalswap)));
    } else {
        ui->availableMemory->setText(tr("Unknown"));
        ui->availableSwap->setText(tr("Unknown"));
    }
    delete info;

    ui->kernelVersion->setText(QSysInfo::kernelVersion());
    ui->qtVersion->setText(qVersion());

    QFile cpuInfoFile("/proc/cpuinfo");
    cpuInfoFile.open(QFile::ReadOnly);
    QByteArray cpuInfoBuf = cpuInfoFile.readAll();
    QBuffer cpuInfo(&cpuInfoBuf);
    cpuInfo.open(QBuffer::ReadOnly);
    QStringList knownCpus;
    while (!cpuInfo.atEnd()) {
        QString line = cpuInfo.readLine();
        if (line.startsWith("model name")) {
            QString cpu = line.mid(line.indexOf(":") + 1).trimmed();
            knownCpus.append(cpu);
        }
    }

    QStringList shownCpus;
    while (knownCpus.length() > 0) {
        QString cpu = knownCpus.value(0);
        int numberOfThisCpu = knownCpus.count(cpu);

        knownCpus.removeAll(cpu);

        if (numberOfThisCpu == 1) {
            shownCpus.append(cpu);
        } else {
            shownCpus.append(QString::number(numberOfThisCpu) + " × " + cpu);
        }
    }

    if (shownCpus.length() == 0) {
        ui->processorType->setText(tr("Unknown"));
    } else {
        ui->processorType->setText(shownCpus.join(" · "));
    }

    QDBusConnection::systemBus().connect("org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(updateHostname()));
    updateHostname();

#ifdef BLUEPRINT
    ui->versionLabel->setText(tr("theDesk %1 - Blueprint").arg("1.0"));
    ui->compileDate->setText(tr("You compiled theDesk on %1").arg(__DATE__));
#else
    ui->versionLabel->setText(tr("theDesk %1").arg("Beta 3"));
    ui->compileDate->setVisible(false);
#endif

    ui->copyrightNotice->setText(tr("Copyright © Victor Tran %1. Licensed under the terms of the GNU General Public License, version 3 or later.").arg("2020"));

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->sysinfoFrame->layout()->setAlignment(ui->distributionPane, Qt::AlignHCenter);
    ui->sysinfoFrame->layout()->setAlignment(ui->hardwareSoftwarePane, Qt::AlignHCenter);
    ui->centralWidget->layout()->setAlignment(ui->aboutPane, Qt::AlignHCenter);
    ui->centralWidget->layout()->setAlignment(ui->copyrightNotice, Qt::AlignHCenter);
    ui->distributionPane->setFixedWidth(contentWidth);
    ui->hardwareSoftwarePane->setFixedWidth(contentWidth);
    ui->aboutPane->setFixedWidth(contentWidth);
    ui->copyrightNotice->setFixedWidth(contentWidth);
}

About::~About() {
    delete d;
    delete ui;
}

void About::on_titleLabel_backButtonClicked() {
    StateManager::instance()->statusCenterManager()->showStatusCenterHamburgerMenu();
}

void About::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit displayNameChanged();
    }
}

QString About::name() {
    return "SystemAbout";
}

QString About::displayName() {
    return tr("About");
}

QIcon About::icon() {
    return QIcon::fromTheme("preferences-desktop-about");
}

QWidget* About::leftPane() {
    return nullptr;
}

void About::on_acknowledgementsButton_clicked() {
    Acknowledgements* ack = new Acknowledgements();
    tPopover* popover = new tPopover(ack);
    popover->setPopoverWidth(SC_DPI(600));
    connect(ack, &Acknowledgements::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, ack, &Acknowledgements::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this->window());
}

void About::on_debugLogButton_clicked() {
    StateManager::statusCenterManager()->hide();
    tLogger::openDebugLogWindow();
}

void About::on_changeHostnameButton_clicked() {
    ChangeHostnamePopover* changeHostname = new ChangeHostnamePopover();
    tPopover* popover = new tPopover(changeHostname);
    popover->setPopoverWidth(SC_DPI(600));
    connect(changeHostname, &ChangeHostnamePopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, changeHostname, &Acknowledgements::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this->window());
}

void About::updateHostname() {
    QString hostname = d->hostnamed->property("PrettyHostname").toString();
    if (hostname.isEmpty()) hostname = d->hostnamed->property("Hostname").toString();
    if (hostname.isEmpty()) hostname = QHostInfo::localHostName();

    ui->hostnameLabel->setText(hostname);
}
