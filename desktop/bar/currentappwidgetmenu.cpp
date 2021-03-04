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

#include <QTimer>

struct CurrentAppWidgetMenuPrivate {
    DesktopWmWindowPtr window;
    QList<QPushButton*> actionButtons;
};

CurrentAppWidgetMenu::CurrentAppWidgetMenu(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CurrentAppWidgetMenu) {
    ui->setupUi(this);

    d = new CurrentAppWidgetMenuPrivate();
    ui->quitImmediateButton->setProperty("type", "destructive");
    ui->doQuitImmediateButton->setProperty("type", "destructive");
    updateModifiers();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

CurrentAppWidgetMenu::~CurrentAppWidgetMenu() {
    delete d;
    delete ui;
}

void CurrentAppWidgetMenu::setWindow(DesktopWmWindowPtr window) {
    for (QPushButton* button : d->actionButtons) {
        ui->actionsLayout->removeWidget(button);
        button->setVisible(false);
        button->deleteLater();
    }
    d->actionButtons.clear();

    d->window = window;
    QString applicationName = window->application()->getProperty("Name").toString();
    ui->quitImmediateButton->setText(tr("Force Stop"));
    ui->quitImmediateTitle->setText(tr("Force Stop %1").arg(applicationName).toUpper());
    ui->quitImmediateDescription->setText(tr("%1 will be forced to exit and won't have a chance to save any unsaved data.").arg(applicationName));
    ui->doQuitImmediateButton->setText(tr("Force Stop %1").arg(applicationName));
    ui->cancelImmediateQuitButton->setVisible(true);

    QStringList actions = window->application()->getStringList("Actions");
    for (const QString& action : qAsConst(actions)) {
        QPushButton* button = new QPushButton();
        button->setText(window->application()->getActionProperty(action, "Name").toString());
        connect(button, &QPushButton::clicked, this, [ = ] {
            window->application()->launchAction(action);
            emit done();
        });
        ui->actionsLayout->addWidget(button);
        d->actionButtons.append(button);
    }
    ui->actionsWidget->setVisible(!actions.isEmpty());

    ui->stackedWidget->setCurrentWidget(ui->initialPage, false);
    updateModifiers();
}

void CurrentAppWidgetMenu::showForceStopScreen() {
    ui->cancelImmediateQuitButton->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->quitImmediatelyPage, false);
    this->setFixedSize(ui->stackedWidget->currentWidget()->sizeHint());
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
    QTimer::singleShot(0, [ = ] {
        tVariantAnimation::singleShot(this, this->size(), ui->stackedWidget->widget(frame)->sizeHint(), 250, [ = ](QVariant value) {
            this->setFixedSize(value.toSize());
        });
    });
}
