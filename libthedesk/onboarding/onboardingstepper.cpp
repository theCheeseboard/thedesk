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
#include "onboardingstepper.h"

#include <QPainter>
#include <libcontemporary_global.h>
#include <tpaintcalculator.h>

struct OnboardingStepperPrivate {
        QString text;
        int step = 1;
        int currentStep = 1;
        bool isFinal = false;
};

OnboardingStepper::OnboardingStepper(QWidget* parent) :
    QWidget(parent) {
    d = new OnboardingStepperPrivate();

    QFont fnt = this->font();
    fnt.setPointSize(13);
    this->setFont(fnt);
}

OnboardingStepper::~OnboardingStepper() {
    delete d;
}

void OnboardingStepper::setText(QString text) {
    d->text = text;
    this->update();
}

void OnboardingStepper::setStep(int step) {
    d->step = step;
    this->update();
}

void OnboardingStepper::setCurrentStep(int step) {
    d->currentStep = step;
    this->update();
}

void OnboardingStepper::setIsFinal(bool isFinal) {
    d->isFinal = isFinal;
    this->update();
}

QSize OnboardingStepper::sizeHint() const {
    return QSize(300, this->fontMetrics().height() + 18);
}

void OnboardingStepper::paintEvent(QPaintEvent* event) {
    QPainter* painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);

    tPaintCalculator calculator;
    calculator.setPainter(painter);
    calculator.setDrawBounds(this->size());
    calculator.setLayoutDirection(this->layoutDirection());

    QPalette::ColorGroup group = d->currentStep < d->step ? QPalette::Disabled : QPalette::Normal;

    int radius = this->height() - SC_DPI(5);
    QRect circleRect;
    circleRect.setSize(QSize(radius, radius));
    circleRect.moveLeft(SC_DPI(9));
    circleRect.moveTop(this->height() / 2 - radius / 2);

    calculator.addRect(circleRect, [=](QRectF drawBounds) {
        painter->setPen(Qt::transparent);
        painter->setBrush(this->palette().color(group, QPalette::WindowText));
        painter->drawEllipse(drawBounds);

        painter->setPen(this->palette().color(group, QPalette::Window));
        painter->drawText(drawBounds, Qt::AlignCenter, QLocale().toString(d->step));
    });

    QRect textRect;
    textRect.setWidth(this->fontMetrics().horizontalAdvance(d->text));
    textRect.setHeight(this->fontMetrics().height());
    textRect.moveLeft(circleRect.right() + SC_DPI(9));
    textRect.moveTop(this->height() / 2 - textRect.height() / 2);

    calculator.addRect(textRect, [=](QRectF drawBounds) {
        painter->setPen(this->palette().color(group, QPalette::WindowText));
        painter->drawText(drawBounds, Qt::AlignLeft, d->text);
    });

    if (d->step != 1) {
        calculator.addRect(circleRect, [=](QRectF drawBounds) {
            painter->drawLine(drawBounds.center().x(), 0, drawBounds.center().x(), drawBounds.top());
        });
    }
    if (!d->isFinal) {
        calculator.addRect(circleRect, [=](QRectF drawBounds) {
            painter->drawLine(drawBounds.center().x(), drawBounds.bottom(), drawBounds.center().x(), this->height());
        });
    }

    calculator.performPaint();

    delete painter;
}
