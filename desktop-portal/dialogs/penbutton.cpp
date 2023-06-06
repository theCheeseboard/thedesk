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
#include "penbutton.h"

#include <QPainter>
#include <QSvgRenderer>
#include <QFile>
#include <tvariantanimation.h>

struct PenButtonPrivate {
    PenButton::PenType penType;
    QColor color;

    tVariantAnimation* heightAnim;
    QSvgRenderer* renderer;
};

PenButton::PenButton(PenType penType, QColor color, QWidget* parent) : QPushButton(parent) {
    d = new PenButtonPrivate();
    d->color = color;
    d->penType = penType;

    QString data;
    if (penType == Pen) {
        this->setFixedSize(SC_DPI_T(QSize(40, 60), QSize));

        QFile penFile(":/screenshot/icons/pen.svg");
        penFile.open(QFile::ReadOnly);
        data = penFile.readAll();
        data = data.arg(color.name(QColor::HexRgb));
    } else {
        this->setFixedSize(SC_DPI_T(QSize(60, 60), QSize));

        QFile penFile(":/screenshot/icons/eraser.svg");
        penFile.open(QFile::ReadOnly);
        data = penFile.readAll();
        data = data.arg(tr("Erase-o", "This and the next translation span two lines. Be spiffy with these!"))
            .arg(tr("matic 2000", "This and the previous translation span two lines. Be spiffy with these!"))
            .arg(this->font().family());
    }

    d->heightAnim = new tVariantAnimation(this);
    d->heightAnim->setStartValue(0);
    d->heightAnim->setEndValue(0);
    d->heightAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->heightAnim->setDuration(250);
    connect(d->heightAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setFixedHeight(value.toInt());
    });

    d->renderer = new QSvgRenderer();
    d->renderer->load(data.toUtf8());

    this->setCheckable(true);
    this->setAutoExclusive(true);
}

PenButton::~PenButton() {
    d->renderer->deleteLater();
    delete d;
}

QColor PenButton::color() const {
    return d->color;
}

void PenButton::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    d->renderer->render(&painter, QRectF(0, 0, this->width(), SC_DPI(100)));
}

void PenButton::updateChecked() {
    d->heightAnim->setStartValue(this->height());
    if (this->isChecked()) {
        d->heightAnim->setEndValue(SC_DPI(100));
    } else {
        d->heightAnim->setEndValue(SC_DPI(60));
    }
    d->heightAnim->start();
}


void PenButton::checkStateSet() {
    updateChecked();
}

void PenButton::nextCheckState() {
    this->setChecked(!this->isChecked());
    updateChecked();
}
