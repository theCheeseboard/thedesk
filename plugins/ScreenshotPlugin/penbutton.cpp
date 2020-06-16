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
    QColor color;

    tVariantAnimation* heightAnim;
    QSvgRenderer* renderer;
};

PenButton::PenButton(QColor color, QWidget* parent) : QPushButton(parent) {
    d = new PenButtonPrivate();
    d->color = color;

    this->setFixedSize(SC_DPI_T(QSize(40, 60), QSize));

    d->heightAnim = new tVariantAnimation(this);
    d->heightAnim->setStartValue(0);
    d->heightAnim->setEndValue(0);
    d->heightAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->heightAnim->setDuration(250);
    connect(d->heightAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setFixedHeight(value.toInt());
    });

    QFile penFile(":/screenshot/icons/pen.svg");
    penFile.open(QFile::ReadOnly);
    QString data = penFile.readAll();
    data = data.arg(color.name(QColor::HexRgb));

    d->renderer = new QSvgRenderer();
    d->renderer->load(data.toUtf8());

    this->setCheckable(true);
    this->setAutoExclusive(true);
}

PenButton::~PenButton() {
    d->renderer->deleteLater();
    delete d;
}

QSize PenButton::sizeHint() const {
    return SC_DPI_T(QSize(20, 30), QSize);
}

QColor PenButton::color() const {
    return d->color;
}

void PenButton::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    d->renderer->render(&painter, QRectF(0, 0, SC_DPI(40), SC_DPI(100)));
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
