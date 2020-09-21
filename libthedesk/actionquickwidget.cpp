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
#include "actionquickwidget.h"
#include "ui_actionquickwidget.h"

#include <QPushButton>
#include <QActionEvent>
#include <QAction>
#include "chunk.h"

struct ActionQuickWidgetPrivate {
    Chunk* parentChunk;
    QMap<QAction*, QWidget*> buttons;
};

ActionQuickWidget::ActionQuickWidget(Chunk* parent) :
    QWidget(parent),
    ui(new Ui::ActionQuickWidget) {
    ui->setupUi(this);

    d = new ActionQuickWidgetPrivate();
    d->parentChunk = parent;
}

ActionQuickWidget::~ActionQuickWidget() {
    delete ui;
    delete d;
}

QAction* ActionQuickWidget::addAction(QString text, std::function<void ()> triggered) {
    QAction* action = new QAction(text, this);
    connect(action, &QAction::triggered, this, triggered);
    QWidget::addAction(action);
    return action;
}

QAction* ActionQuickWidget::addAction(QIcon icon, QString text, std::function<void ()> triggered) {
    QAction* action = new QAction(icon, text, this);
    connect(action, &QAction::triggered, this, triggered);
    QWidget::addAction(action);
    return action;
}

bool ActionQuickWidget::event(QEvent* event) {
    QActionEvent* e = static_cast<QActionEvent*>(event);
    switch (event->type()) {
        case QEvent::ActionAdded: {
            QAction* action = e->action();
            QPushButton* button = new QPushButton();
            button->setText(action->text());
            button->setIcon(action->icon());
            button->setEnabled(action->isEnabled());
            button->setVisible(action->isVisible());

            connect(button, &QPushButton::clicked, this, [ = ] {
                d->parentChunk->hideQuickWidget();
                action->trigger();
            });
            connect(action, &QAction::changed, this, [ = ] {
                button->setText(action->text());
                button->setIcon(action->icon());
                button->setEnabled(action->isEnabled());
                button->setVisible(action->isVisible());
            });

            d->buttons.insert(action, button);
            ui->actionsLayout->addWidget(button);
            break;
        }
        case QEvent::ActionChanged:
            break;
        case QEvent::ActionRemoved: {
            QWidget* button = d->buttons.value(e->action());
            ui->actionsLayout->removeWidget(button);
            button->deleteLater();
            d->buttons.remove(e->action());
            break;
        }
        default:
            break;
    }

    return QWidget::event(event);
}
