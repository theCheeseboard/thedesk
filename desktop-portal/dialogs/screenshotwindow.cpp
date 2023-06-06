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
#include "screenshotwindow.h"
#include "ui_screenshotwindow.h"

#include "penbutton.h"
#include <QClipboard>
#include <QMouseEvent>
#include <QPainter>
#include <QPicture>
#include <QScreen>
#include <QShortcut>
#include <tvariantanimation.h>

struct ScreenshotWindowPrivate {
        enum Operation {
            Crop,
            Redact,
            Pen
        };
        Operation currentOperation = Crop;
        QColor currentColor;
        QPainter::CompositionMode compMode = QPainter::CompositionMode_SourceOver;
        qreal penWidth;

        QPixmap originalShot;

        tVariantAnimation* darkenAnim;
        tVariantAnimation* viewportAnim;

        QRect cropRect;

        QPoint topLeftDrag;
        QRect editingRect;

        QPixmap overlay;
};

ScreenshotWindow::ScreenshotWindow(QScreen* screen, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ScreenshotWindow) {
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    d = new ScreenshotWindowPrivate();
    d->originalShot = screen->grabWindow(0);

    d->darkenAnim = new tVariantAnimation(this);
    d->darkenAnim->setStartValue(0.0);
    d->darkenAnim->setEndValue(0.5);
    d->darkenAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->darkenAnim->setDuration(250);
    connect(d->darkenAnim, &tVariantAnimation::valueChanged, this, [=] {
        this->update();
    });
    connect(d->darkenAnim, &tVariantAnimation::finished, this, [=] {
        this->update();
    });
    d->darkenAnim->start();

    d->viewportAnim = new tVariantAnimation(this);
    d->viewportAnim->setStartValue(QRect(0, 0, screen->geometry().width(), screen->geometry().height()));
    d->viewportAnim->setEndValue(QRect(0, 0, screen->geometry().width(), screen->geometry().height()));
    d->viewportAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->viewportAnim->setDuration(250);
    connect(d->viewportAnim, &tVariantAnimation::valueChanged, this, [=] {
        this->update();
    });
    connect(d->viewportAnim, &tVariantAnimation::finished, this, [=] {
        this->update();
    });

    this->setGeometry(screen->geometry());

    d->overlay = QPixmap(screen->geometry().size());
    d->overlay.fill(Qt::transparent);

    for (QColor col : {
             QColor(255, 0, 0),
             QColor(0, 255, 0),
             QColor(0, 0, 255),
             QColor(255, 255, 0),
             QColor(0, 255, 255),
             QColor(255, 0, 255)}) {
        PenButton* b = new PenButton(PenButton::Pen, col, this);
        connect(b, &PenButton::toggled, this, [=](bool checked) {
            if (checked) {
                d->currentOperation = ScreenshotWindowPrivate::Pen;
                d->currentColor = b->color();
                d->compMode = QPainter::CompositionMode_SourceOver;
                d->penWidth = SC_DPI(3);

                this->update();
            }
        });
        ui->pensLayout->addWidget(b, 0, Qt::AlignBottom);
    }

    PenButton* eraser = new PenButton(PenButton::Eraser, QColor(0, 0, 0, 0), this);
    connect(eraser, &PenButton::toggled, this, [=](bool checked) {
        if (checked) {
            d->currentOperation = ScreenshotWindowPrivate::Pen;
            d->currentColor = eraser->color();
            d->compMode = QPainter::CompositionMode_Source;
            d->penWidth = SC_DPI(10);

            this->update();
        }
    });
    ui->pensLayout->addWidget(eraser, 0, Qt::AlignBottom);

    QShortcut* discardShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(discardShortcut, &QShortcut::activated, this, [=] {
        ui->discardButton->click();
    });

    ui->discardButton->setText(tr("Cancel"));
    ui->copyButton->setText(tr("Share Screenshot"));
    ui->copyButton->setIcon(QIcon::fromTheme("dialog-ok"));
}

void ScreenshotWindow::paintEvent(QPaintEvent* event) {
    bool editing = d->editingRect.isValid();
    QRect cropRect;

    if (ui->cropButton->isChecked() && editing) {
        cropRect = d->editingRect;
    } else {
        cropRect = d->cropRect;
    }

    QPainter painter(this);
    painter.setViewport(d->viewportAnim->currentValue().toRect());
    painter.drawPixmap(0, 0, this->width(), this->height(), d->originalShot);

    if (cropRect.isValid() || d->currentOperation == ScreenshotWindowPrivate::Crop) {
        painter.setOpacity(d->darkenAnim->currentValue().toDouble());
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::black);
        painter.drawRect(0, 0, this->width(), this->height());
        painter.setOpacity(1);

        if (cropRect.isValid()) {
            painter.drawPixmap(cropRect, d->originalShot, cropRect);
        }
    }

    painter.drawPixmap(0, 0, this->width(), this->height(), d->overlay);

    if (d->currentOperation == ScreenshotWindowPrivate::Redact && editing) {
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::black);
        painter.drawRect(d->editingRect);
    }

    if (cropRect.isValid()) {
        painter.setPen(Qt::white);
        painter.setBrush(Qt::transparent);
        painter.drawRect(cropRect.adjusted(-1, -1, 0, 0));
    }
}

ScreenshotWindow::~ScreenshotWindow() {
    delete d;
    delete ui;
}

void ScreenshotWindow::take(QScreen* screen, int delay) {
    ScreenshotWindow* w = new ScreenshotWindow(screen);
    w->showFullScreen();
}

void ScreenshotWindow::animateDiscard() {
    d->viewportAnim->setEndValue(QRect(0, this->height(), this->width(), this->height()));
    d->viewportAnim->setEasingCurve(QEasingCurve::InCubic);
    d->viewportAnim->start();

    tVariantAnimation* paneAnim = new tVariantAnimation(this);
    paneAnim->setStartValue(ui->bottomPane->height());
    paneAnim->setEndValue(0);
    paneAnim->setDuration(250);
    paneAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(paneAnim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        ui->bottomPane->setFixedHeight(value.toInt());
    });
    paneAnim->start();

    connect(d->viewportAnim, &tVariantAnimation::finished, this, &ScreenshotWindow::animationComplete);
}

void ScreenshotWindow::animateTake() {
    d->viewportAnim->setEndValue(QRect(0, -this->height(), this->width(), this->height()));
    d->viewportAnim->setEasingCurve(QEasingCurve::InCubic);
    d->viewportAnim->start();

    tVariantAnimation* paneAnim = new tVariantAnimation(this);
    paneAnim->setStartValue(ui->bottomPane->height());
    paneAnim->setEndValue(0);
    paneAnim->setDuration(250);
    paneAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(paneAnim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        ui->bottomPane->setFixedHeight(value.toInt());
    });
    paneAnim->start();

    connect(d->viewportAnim, &tVariantAnimation::finished, this, &ScreenshotWindow::animationComplete);
}

void ScreenshotWindow::on_discardButton_clicked() {
    emit cancelled();
}

void ScreenshotWindow::mousePressEvent(QMouseEvent* event) {
    d->topLeftDrag = event->pos();
}

void ScreenshotWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (d->currentOperation == ScreenshotWindowPrivate::Crop) {
        d->cropRect = d->editingRect;
        this->update();
    } else if (d->currentOperation == ScreenshotWindowPrivate::Redact) {
        QPainter painter(&d->overlay);
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::black);
        painter.drawRect(d->editingRect);
    }

    d->editingRect = QRect();
    this->update();
}

void ScreenshotWindow::mouseMoveEvent(QMouseEvent* event) {
    if (d->currentOperation == ScreenshotWindowPrivate::Pen) {
        QPainter painter(&d->overlay);
        painter.setCompositionMode(d->compMode);
        painter.setPen(QPen(d->currentColor, d->penWidth));
        painter.drawLine(d->topLeftDrag, event->pos());

        d->topLeftDrag = event->pos();
    } else {
        d->editingRect = QRect(d->topLeftDrag, event->pos());
        d->editingRect = d->editingRect.normalized();
    }
    this->update();
}

QPixmap ScreenshotWindow::finalResult() {
    QPixmap result = d->originalShot;

    // Draw in any edits
    QPainter painter(&result);
    painter.drawPixmap(0, 0, this->width(), this->height(), d->overlay);
    painter.end();

    // Crop the image
    if (d->cropRect.isValid()) {
        result = result.copy(d->cropRect);
    }

    return result;
}

void ScreenshotWindow::on_copyButton_clicked() {
    emit screenshotAvailable(finalResult());
}

void ScreenshotWindow::on_cropButton_toggled(bool checked) {
    if (checked) {
        d->currentOperation = ScreenshotWindowPrivate::Crop;

        this->update();
    }
}

void ScreenshotWindow::on_redactButton_toggled(bool checked) {
    if (checked) {
        d->currentOperation = ScreenshotWindowPrivate::Redact;

        this->update();
    }
}

void ScreenshotWindow::on_resetButton_clicked() {
    d->overlay.fill(Qt::transparent);
    this->update();
}
