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
#include "maingatewaywidget.h"
#include "ui_maingatewaywidget.h"

#include <Wm/desktopwm.h>
#include <QScroller>
#include <tvariantanimation.h>
#include <Applications/application.h>
#include "appselectionmodel.h"
#include "appselectionmodellistdelegate.h"
#include "session/endsession.h"
#include <statemanager.h>
#include <powermanager.h>
#include <tscrim.h>
#include "searchresultswidget.h"
#include <statuscentermanager.h>
#include <QShortcut>
#include <QMenu>

struct MainGatewayWidgetPrivate {
    AppSelectionModel* model;

    SearchResultsWidget* searchWidget;
};

MainGatewayWidget::MainGatewayWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainGatewayWidget) {
    ui->setupUi(this);

    d = new MainGatewayWidgetPrivate();
    d->model = new AppSelectionModel(this);
    connect(d->model, &AppSelectionModel::loading, this, [ = ] {
        ui->gatewayTypeStack->setCurrentWidget(ui->gatewayLoading);
    });
    connect(d->model, &AppSelectionModel::ready, this, [ = ] {
        QTimer::singleShot(500, [ = ] {
            ui->gatewayTypeStack->setCurrentWidget(ui->gatewayList);
        });
    });
    ui->gatewayTypeStack->setCurrentWidget(ui->gatewayLoading);
    ui->gatewayTypeStack->setCurrentAnimation(tStackedWidget::Fade);

    ui->titleLabel->setBackButtonShown(true);
    ui->leftPane->setFixedWidth(SC_DPI(200));

//    ui->usernameLabel->setText(tr("Hey, %1!").arg(DesktopWm::userDisplayName()));
    ui->usernameLabel->setText(DesktopWm::userDisplayName());

    ui->appsList->setModel(d->model);
    ui->appsList->setItemDelegate(new AppSelectionModelListDelegate(this, true));
    ui->appsList->setFocusProxy(ui->searchBox);
    QScroller::grabGesture(ui->appsList->viewport(), QScroller::LeftMouseButtonGesture);

    this->setFocusProxy(ui->searchBox);
    ui->endSessionButton->setFocusProxy(ui->searchBox);

    d->searchWidget = new SearchResultsWidget(ui->searchResultsContainer);
    d->searchWidget->move(0, 0);
    d->searchWidget->hide();
    connect(d->searchWidget, &SearchResultsWidget::closeGateway, this, &MainGatewayWidget::closeGateway);

    connect(tScrim::scrimForWidget(ui->appContentsWidget), &tScrim::scrimClicked, this, [ = ] {
        ui->searchBox->setText("");
        tScrim::scrimForWidget(ui->appContentsWidget)->hide();
        d->searchWidget->hide();
    });

    QList<QPair<QString, QString>> categories = {
        {"AudioVideo", tr("A/V")},
        {"Development", tr("Development")},
        {"Education", tr("Education")},
        {"Game", tr("Games")},
        {"Graphics", tr("Graphics")},
        {"Network", tr("Networking")},
        {"Office", tr("Office")},
        {"Science", tr("Science")},
        {"Utility", tr("Utilities")}
    };

    std::sort(categories.begin(), categories.end(), [ = ](const QPair<QString, QString>& first, const QPair<QString, QString>& second) {
        return first.second.localeAwareCompare(second.second) < 0;
    });

    for (const QPair<QString, QString>& category : categories) {
        QPushButton* button = new QPushButton(this);
        button->setAutoExclusive(true);
        button->setCheckable(true);
        button->setText(category.second);
        connect(button, &QPushButton::toggled, this, [ = ](bool checked) {
            if (checked) d->model->filterCategory(category.first);
        });
        ui->categoriesLayout->addWidget(button);
    }

    ui->searchBox->installEventFilter(this);
}

MainGatewayWidget::~MainGatewayWidget() {
    delete d;
    delete ui;
}

QSize MainGatewayWidget::sizeHint() const {
    QSize sizeHint = QWidget::sizeHint();
    sizeHint.setWidth(SC_DPI(400));
    sizeHint.setWidth(SC_DPI(700));
    return sizeHint;
}

void MainGatewayWidget::clearState() {
    ui->searchBox->setText("");
    tScrim::scrimForWidget(ui->appContentsWidget)->hide();
    d->searchWidget->hide();
    ui->allAppsButton->setChecked(true);
}

void MainGatewayWidget::on_searchBox_textEdited(const QString& arg1) {
    if (arg1.isEmpty()) {
        tScrim::scrimForWidget(ui->appContentsWidget)->hide();
        d->searchWidget->hide();
    } else {
        tScrim::scrimForWidget(ui->appContentsWidget)->show();
        d->searchWidget->show();
    }
    d->searchWidget->search(arg1);
}

void MainGatewayWidget::on_appsList_clicked(const QModelIndex& index) {
    int x = ui->appsList->mapFromGlobal(QCursor::pos()).x();
    if ((this->layoutDirection() == Qt::RightToLeft && x < SC_DPI(32)) ||
        (this->layoutDirection() == Qt::LeftToRight && x > ui->appsList->width() - SC_DPI(32))) {
        if (showActionMenu(index)) return;
    }
    launch(index);
}

void MainGatewayWidget::on_searchBox_returnPressed() {
    if (!ui->searchBox->text().isEmpty()) d->searchWidget->launchSelected();
}

void MainGatewayWidget::launch(QModelIndex applicationIndex) {
    applicationIndex.data(Qt::UserRole + 3).value<ApplicationPointer>()->launch();
    emit closeGateway();
}

bool MainGatewayWidget::showActionMenu(QModelIndex applicationIndex) {
    ApplicationPointer application = applicationIndex.data(Qt::UserRole + 3).value<ApplicationPointer>();
    if (application->getStringList("Actions").isEmpty()) return false;

    QMenu* menu = new QMenu();
    menu->addSection(tr("Actions for %1").arg(QLocale().quoteString(application->getProperty("Name").toString())));
    for (const QString& action : application->getStringList("Actions")) {
        menu->addAction(application->actionIcon(action), application->getActionProperty(action, "Name").toString(), this, [ = ] {
            application->launchAction(action);
            emit closeGateway();
        });
    }

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);

    QRect rect = ui->appsList->visualRect(applicationIndex);
    if (layoutDirection() == Qt::RightToLeft) {
        menu->popup(ui->appsList->mapToGlobal(rect.topLeft()));
    } else {
        menu->popup(ui->appsList->mapToGlobal(rect.topRight()));
    }
    return true;
}

void MainGatewayWidget::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void MainGatewayWidget::resizeEvent(QResizeEvent* event) {
    d->searchWidget->setFixedWidth(this->width());
}

void MainGatewayWidget::on_endSessionButton_clicked() {
    StateManager::instance()->powerManager()->showPowerOffConfirmation();
}

void MainGatewayWidget::on_allAppsButton_toggled(bool checked) {
    if (checked) d->model->filterCategory("");
}

void MainGatewayWidget::on_titleLabel_backButtonClicked() {
    emit closeGateway();
}

void MainGatewayWidget::on_statusCenterButton_clicked() {
    StateManager::statusCenterManager()->show();
    emit closeGateway();
}

void MainGatewayWidget::on_systemSettingsButton_clicked() {
    StateManager::statusCenterManager()->show();
    StateManager::statusCenterManager()->setPane("SystemSettings");
    emit closeGateway();
}


bool MainGatewayWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->searchBox) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Down) {
                if (ui->searchBox->text().isEmpty()) {
                    QItemSelectionModel* selectionModel = ui->appsList->selectionModel();
                    int selectionRow = 0;
                    if (selectionModel->hasSelection()) {
                        int row = selectionModel->selectedIndexes().first().row();
                        if (row != d->model->rowCount() - 1) selectionRow = row + 1;
                    }
                    selectionModel->select(d->model->index(selectionRow), QItemSelectionModel::ClearAndSelect);
                    ui->appsList->scrollTo(d->model->index(selectionRow), QListView::PositionAtCenter);
                } else {
                    d->searchWidget->moveSelectionDown();
                }
                return true;
            } else if (keyEvent->key() == Qt::Key_Up) {
                if (ui->searchBox->text().isEmpty()) {
                    QItemSelectionModel* selectionModel = ui->appsList->selectionModel();
                    int selectionRow = d->model->rowCount() - 1;
                    if (selectionModel->hasSelection()) {
                        int row = selectionModel->selectedIndexes().first().row();
                        if (row != 0) selectionRow = row - 1;
                    }
                    selectionModel->select(d->model->index(selectionRow), QItemSelectionModel::ClearAndSelect);
                    ui->appsList->scrollTo(d->model->index(selectionRow), QListView::PositionAtCenter);
                } else {
                    d->searchWidget->moveSelectionUp();
                }
                return true;
            } else if ((layoutDirection() == Qt::LeftToRight && keyEvent->key() == Qt::Key_Right) ||
                (layoutDirection() == Qt::RightToLeft && keyEvent->key() == Qt::Key_Left)) {
                if (ui->searchBox->text().isEmpty()) {
                    QItemSelectionModel* selectionModel = ui->appsList->selectionModel();
                    if (selectionModel->hasSelection()) {
                        showActionMenu(selectionModel->selectedIndexes().first());
                    }
                    return true;
                }
            }
        }
    }
    return false;
}
