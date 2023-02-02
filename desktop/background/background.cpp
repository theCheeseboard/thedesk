/****************************************
 *
 *   theShell - Desktop Environment
 *   Copyright (C) 2019 Victor Tran
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

#include "background.h"
#include "ui_background.h"

#include <Background/backgroundcontroller.h>
#include <Background/backgroundselectionmodel.h>
#include <QFileDialog>
#include <QMenu>
#include <Screens/screendaemon.h>
#include <Screens/systemscreen.h>
#include <Wm/desktopwm.h>
#include <X11/Xlib.h>

struct BackgroundPrivate {
        static BackgroundController* bg;
        static QList<Background*> backgrounds;

        SystemScreen* oldScreen = nullptr;
        QMetaObject::Connection screenGeometryChangedConnection;
        QSettings settings;

        bool retrieving = false;
        bool retrieveAgain = false;
        BackgroundController::BackgroundData background;

        bool isChangeBackgroundVisible = false;
        bool communityBackgroundSettingsShown = true;
};

BackgroundController* BackgroundPrivate::bg = nullptr;
QList<Background*> BackgroundPrivate::backgrounds = QList<Background*>();

Background::Background() :
    QDialog(nullptr),
    ui(new Ui::Background) {
    ui->setupUi(this);
    d = new BackgroundPrivate();

    if (!d->bg) {
        d->bg = new BackgroundController(BackgroundController::Desktop);

        connect(qApp, &QApplication::screenAdded, &Background::reconfigureBackgrounds);
        connect(qApp, &QApplication::screenRemoved, &Background::reconfigureBackgrounds);
        connect(ScreenDaemon::instance(), &ScreenDaemon::screensUpdated, &Background::reconfigureBackgrounds);
    }

    connect(d->bg, &BackgroundController::currentBackgroundChanged, this, [this](BackgroundController::BackgroundType type) {
        if (type == BackgroundController::Desktop) this->changeBackground();
        this->showCommunityBackgroundSettings(d->bg->currentBackgroundName(BackgroundController::Desktop) == "community" || d->bg->currentBackgroundName(BackgroundController::LockScreen) == "community");
    });
    connect(d->bg, &BackgroundController::shouldShowCommunityLabelsChanged, this, [this] {
        if (d->bg->currentBackgroundName(BackgroundController::Desktop) == "community") this->changeBackground();
    });
    connect(d->bg, &BackgroundController::stretchTypeChanged, this, [this](BackgroundController::StretchType stretchType) {
        switch (stretchType) {
            case BackgroundController::StretchFit:
                ui->stretchFitButton->setChecked(true);
                break;
            case BackgroundController::ZoomCrop:
                ui->zoomCropButton->setChecked(true);
                break;
            case BackgroundController::Center:
                ui->centerButton->setChecked(true);
                break;
            case BackgroundController::Tile:
                ui->tileButton->setChecked(true);
                break;
            case BackgroundController::ZoomFit:
                ui->zoomFitButton->setChecked(true);
                break;
        }

        this->changeBackground();
    });

    this->showCommunityBackgroundSettings(d->bg->currentBackgroundName(BackgroundController::Desktop) == "community" || d->bg->currentBackgroundName(BackgroundController::LockScreen) == "community");
    switch (d->bg->stretchType()) {
        case BackgroundController::StretchFit:
            ui->stretchFitButton->setChecked(true);
            break;
        case BackgroundController::ZoomCrop:
            ui->zoomCropButton->setChecked(true);
            break;
        case BackgroundController::Center:
            ui->centerButton->setChecked(true);
            break;
        case BackgroundController::Tile:
            ui->tileButton->setChecked(true);
            break;
        case BackgroundController::ZoomFit:
            ui->zoomFitButton->setChecked(true);
            break;
    }

    ui->showImageInformationBox->setChecked(d->bg->shouldShowCommunityLabels());

    ui->stackedWidget->setCurrentWidget(ui->backgroundPage);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->backgroundPage->installEventFilter(this);

    ui->backgroundSelectionWidget->setFixedHeight(0);
    ui->backgroundList->setModel(new BackgroundSelectionModel());
    ui->backgroundList->setItemDelegate(new BackgroundSelectionDelegate());
    ui->backgroundList->setIconSize(SC_DPI_T(QSize(213, 120), QSize));
    ui->backgroundList->setFixedHeight(SC_DPI(120));

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    this->setAttribute(Qt::WA_ShowWithoutActivating, true);

    changeBackground();
}

Background::~Background() {
    delete d;
    delete ui;
}

QCoro::Task<> Background::changeBackground() {
    if (d->retrieving) {
        d->retrieveAgain = true;
        co_return;
    }

    d->retrieving = true;
    ui->stackedWidget->setCurrentWidget(ui->loadingBackgroundPage);

    try {
        auto data = co_await d->bg->getCurrentBackground(this->size());
        // TODO: what if the backgrounds change?
        d->background = data;

        if (d->background.extendedInfoAvailable) {
            QPainter painter(&data.px);

            if (d->settings.value("desktop/showLabels", true).toBool()) {
                QLinearGradient darkener;
                darkener.setColorAt(0, QColor::fromRgb(0, 0, 0, 0));
                darkener.setColorAt(1, QColor::fromRgb(0, 0, 0, 200));

                if (d->settings.value("bar/onTop", true).toBool()) {
                    darkener.setStart(0, 0);
                    darkener.setFinalStop(0, data.px.height());
                } else {
                    darkener.setStart(0, data.px.height());
                    darkener.setFinalStop(0, 0);
                }
                painter.setBrush(darkener);
                painter.drawRect(0, 0, data.px.width(), data.px.height());

                painter.setPen(Qt::white);
                int currentX = SC_DPI(30);
                int baselineY;

                if (d->settings.value("bar/onTop", true).toBool()) {
                    baselineY = data.px.height() - SC_DPI(30);
                } else {
                    baselineY = SC_DPI(30) + QFontMetrics(QFont(this->font().family(), 20)).ascent();
                }

                if (!data.name.isEmpty()) {
                    painter.setFont(QFont(this->font().family(), 20));
                    int width = painter.fontMetrics().horizontalAdvance(data.name);
                    painter.drawText(currentX, baselineY, data.name);

                    currentX += width + SC_DPI(9);
                }

                if (!data.location.isEmpty()) {
                    painter.setFont(QFont(this->font().family(), 10));
                    QIcon locationIcon = QIcon::fromTheme("gps");
                    int height = painter.fontMetrics().height();
                    int width = painter.fontMetrics().horizontalAdvance(data.location) + height;

                    painter.drawPixmap(currentX, baselineY - height, locationIcon.pixmap(SC_DPI_T(QSize(16, 16), QSize)));
                    painter.drawText(currentX + height + SC_DPI(6), baselineY - painter.fontMetrics().descent(), data.location);

                    currentX += width + SC_DPI(20);
                }

                if (!data.author.isEmpty()) {
                    painter.setFont(QFont(this->font().family(), 10));
                    QString author = tr("by %1").arg(data.author);
                    int width = painter.fontMetrics().horizontalAdvance(author);
                    painter.drawText(data.px.width() - width - SC_DPI(30), baselineY, author);
                }
            }
        }

        QTimer::singleShot(1000, this, [this, data] {
            d->background = data;
            this->update();

            d->retrieving = false;
            if (d->retrieveAgain) {
                d->retrieveAgain = false;
                this->changeBackground();
            } else {
                ui->stackedWidget->setCurrentWidget(ui->backgroundPage);
            }
        });
    } catch (BackgroundException ex) {
        d->retrieving = false;
        if (d->retrieveAgain) {
            d->retrieveAgain = false;
            this->changeBackground();
        } else {
            ui->stackedWidget->setCurrentWidget(ui->backgroundErrorPage);
        }
    }
}

void Background::toggleChangeBackground() {
    d->isChangeBackgroundVisible = !d->isChangeBackgroundVisible;
    tVariantAnimation* anim = new tVariantAnimation();
    anim->setStartValue(ui->backgroundSelectionWidget->height());
    if (d->isChangeBackgroundVisible) {
        anim->setEndValue(ui->backgroundSelectionWidget->sizeHint().height());
        // TODO: Hide the bar
        //        MainWindow::instance()->forceHide();
    } else {
        // TODO: Show the bar
        anim->setEndValue(0);
        //        MainWindow::instance()->unforceHide();
    }
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [this](QVariant value) {
        ui->backgroundSelectionWidget->setFixedHeight(value.toInt());
    });
    connect(anim, &tVariantAnimation::finished, this, [this] {
        if (d->isChangeBackgroundVisible) {
            ui->backgroundSelectionWidget->setFixedHeight(QWIDGETSIZE_MAX);
        }
    });
    anim->start();

    DesktopWm::setShowDesktop(d->isChangeBackgroundVisible);
}

void Background::showCommunityBackgroundSettings(bool shown) {
    if (shown == d->communityBackgroundSettingsShown) return;
    d->communityBackgroundSettingsShown = shown;
    ui->communityBackgroundSettings->setVisible(shown);
}

void Background::show() {
    DesktopWm::setSystemWindow(this, DesktopWm::SystemWindowTypeDesktop);
    QDialog::show();
}

void Background::on_actionChange_Background_triggered() {
    this->toggleChangeBackground();
}

void Background::reject() {
}

bool Background::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->backgroundPage) {
        if (event->type() == QEvent::Paint) {
            QPainter p(ui->backgroundPage);
            if (d->retrieving) {
                p.setPen(Qt::transparent);
                p.setBrush(Qt::black);
                p.drawRect(0, 0, this->width(), this->height());
            } else {
                p.drawPixmap(0, -ui->backgroundSelectionWidget->height(), d->background.px);
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            if (d->isChangeBackgroundVisible) toggleChangeBackground();
        }
    }
    return false;
}

void Background::on_Background_customContextMenuRequested(const QPoint& pos) {
    QMenu* menu = new QMenu(this);
    menu->addSection(tr("For desktop"));
    menu->addAction(ui->actionChange_Background);

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(this->mapToGlobal(pos));
}

void Background::paintEvent(QPaintEvent* event) {
}

void Background::resizeEvent(QResizeEvent* event) {
    this->changeBackground();
}

void Background::resizeToScreen(int screen) {
    auto s = ScreenDaemon::instance()->screens().at(screen);
    if (s != d->oldScreen) {
        if (s != nullptr) disconnect(d->screenGeometryChangedConnection);

        connect(s, &SystemScreen::geometryChanged, this, [this, s] {
            this->setGeometry(s->geometry());
        });
        this->setGeometry(s->geometry());
        this->show();

        d->oldScreen = s;
    }
}

void Background::reconfigureBackgrounds() {
    if (BackgroundPrivate::backgrounds.count() > ScreenDaemon::instance()->screens().count()) {
        // Remove backgrounds until the correct number of backgrounds have been created
        int difference = BackgroundPrivate::backgrounds.count() - ScreenDaemon::instance()->screens().count();
        for (int i = 0; i < difference; i++) {
            BackgroundPrivate::backgrounds.takeLast()->deleteLater();
        }
    }

    if (BackgroundPrivate::backgrounds.count() < ScreenDaemon::instance()->screens().count()) {
        // Add new backgrounds until the correct number of backgrounds have been created
        int difference = ScreenDaemon::instance()->screens().count() - BackgroundPrivate::backgrounds.count();
        for (int i = 0; i < difference; i++) {
            Background* w = new Background();
            BackgroundPrivate::backgrounds.append(w);
        }
    }

    for (int i = 0; i < ScreenDaemon::instance()->screens().count(); i++) {
        BackgroundPrivate::backgrounds.at(i)->resizeToScreen(i);
    }
}

void Background::on_tryReloadBackgroundButton_clicked() {
    this->changeBackground();
}

void Background::on_backgroundList_clicked(const QModelIndex& index) {
    QString background = index.data(Qt::UserRole).toString();
    if (background == "custom") {
        // Ask the user to select a background
        background = QFileDialog::getOpenFileName(this, tr("Select Background"), "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
        if (background.isEmpty()) return;
    }
    d->bg->setBackground(background, BackgroundController::Desktop);
}

void Background::on_backButton_clicked() {
    this->toggleChangeBackground();
}

void Background::on_showImageInformationBox_toggled(bool checked) {
    d->bg->setShouldShowCommunityLabels(checked);
}

void Background::on_stretchFitButton_toggled(bool checked) {
    if (checked) {
        d->bg->setStretchType(BackgroundController::StretchFit);
    }
}

void Background::on_zoomCropButton_toggled(bool checked) {
    if (checked) {
        d->bg->setStretchType(BackgroundController::ZoomCrop);
    }
}

void Background::on_centerButton_toggled(bool checked) {
    if (checked) {
        d->bg->setStretchType(BackgroundController::Center);
    }
}

void Background::on_tileButton_toggled(bool checked) {
    if (checked) {
        d->bg->setStretchType(BackgroundController::Tile);
    }
}

void Background::on_zoomFitButton_toggled(bool checked) {
    if (checked) {
        d->bg->setStretchType(BackgroundController::ZoomFit);
    }
}

void Background::on_actionEndSession_triggered() {
}
