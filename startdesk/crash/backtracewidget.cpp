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
#include "backtracewidget.h"
#include "ui_backtracewidget.h"

#include "splash/splashcontroller.h"
#include <QFileDialog>
#include <QFontDatabase>
#include <libcontemporary_global.h>

BacktraceWidget::BacktraceWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::BacktraceWidget) {
    ui->setupUi(this);

    ui->textBrowser->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->textBrowser->setText(SplashController::instance()->lastBacktrace().join("\n"));

    ui->topWidget->setFixedWidth(SC_DPI(600));
}

BacktraceWidget::~BacktraceWidget() {
    delete ui;
}

void BacktraceWidget::on_saveButton_clicked() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setFileMode(QFileDialog::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this, [=](QString file) {
        QFile fileOutput(file);
        fileOutput.open(QFile::WriteOnly);
        fileOutput.write(ui->textBrowser->toPlainText().toUtf8());
        fileOutput.close();
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}
