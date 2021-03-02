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
#include "currentappwidgetmenu.h"
#include "ui_currentappwidgetmenu.h"

struct CurrentAppWidgetMenuPrivate {
    DesktopWmWindowPtr window;
};

CurrentAppWidgetMenu::CurrentAppWidgetMenu(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CurrentAppWidgetMenu) {
    ui->setupUi(this);

    d = new CurrentAppWidgetMenuPrivate();
    ui->quitImmediateButton->setProperty("type", "destructive");
    ui->doQuitImmediateButton->setProperty("type", "destructive");
    updateModifiers();
}

CurrentAppWidgetMenu::~CurrentAppWidgetMenu() {
    delete d;
    delete ui;
}

void CurrentAppWidgetMenu::setWindow(DesktopWmWindowPtr window) {
    d->window = window;
    QString applicationName = window->application()->getProperty("Name").toString();
    ui->quitImmediateButton->setText(tr("Force Stop"));
    ui->quitImmediateTitle->setText(tr("Force Stop %1").arg(applicationName).toUpper());
    ui->quitImmediateDescription->setText(tr("%1 will be forced to exit and won't have a chance to save any unsaved data.").arg(applicationName));
    ui->doQuitImmediateButton->setText(tr("Force Stop %1").arg(applicationName));

    ui->stackedWidget->setCurrentWidget(ui->initialPage, false);
    updateModifiers();
}

void CurrentAppWidgetMenu::updateModifiers() {
    bool showExtraOptions = qApp->queryKeyboardModifiers() & Qt::ShiftModifier;
    ui->quitImmediateButton->setVisible(showExtraOptions);

    on_stackedWidget_switchingFrame(ui->stackedWidget->currentIndex());
}

void CurrentAppWidgetMenu::on_closeWindowButton_clicked() {
    d->window->close();
    emit done();
}

void CurrentAppWidgetMenu::keyPressEvent(QKeyEvent* event) {
    updateModifiers();
}

void CurrentAppWidgetMenu::keyReleaseEvent(QKeyEvent* event) {
    updateModifiers();
}

void CurrentAppWidgetMenu::on_doQuitImmediateButton_clicked() {
    d->window->kill();
    emit done();
}

void CurrentAppWidgetMenu::on_cancelImmediateQuitButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->initialPage);
}

void CurrentAppWidgetMenu::on_quitImmediateButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->quitImmediatelyPage);
}

void CurrentAppWidgetMenu::on_stackedWidget_switchingFrame(int frame) {
    this->setFixedSize(ui->stackedWidget->widget(frame)->sizeHint());
}
