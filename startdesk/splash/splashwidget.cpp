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
#include "splashwidget.h"
#include "ui_splashwidget.h"

#include <QFontDatabase>
#include <QProcess>

SplashWidget::SplashWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SplashWidget) {
    ui->setupUi(this);

//    QProcess proc;
//    proc.start("fortune", QStringList());
//    proc.waitForFinished();
//    QProcess cs;
//    cs.start("cowsay", QStringList());
//    cs.write(proc.readAll());
//    cs.closeWriteChannel();
//    cs.waitForFinished();

//    ui->fortuneLabel->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
//    ui->fortuneLabel->setText(cs.readAll().trimmed());
    ui->fortuneLabel->setVisible(false);
}

SplashWidget::~SplashWidget() {
    delete ui;
}
