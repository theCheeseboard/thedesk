#include "underlineanimation.h"

UnderlineAnimation::UnderlineAnimation(QWidget *parent) : QWidget(parent)
{
    anim1 = new tVariantAnimation();
    anim1->setStartValue(0);
    anim1->setEndValue(this->width() / 2);
    anim1->setDuration(500);
    anim1->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim1, &tVariantAnimation::valueChanged, [=](QVariant value) {
        this->repaint();
    });

    QColor col = this->palette().color(QPalette::WindowText).toRgb();
    anim2 = new tVariantAnimation();
    anim2->setStartValue(col);

    col.setAlpha(0);
    anim2->setEndValue(col);
    anim2->setDuration(500);
    anim2->setEasingCurve(QEasingCurve::OutCubic);
}

QSize UnderlineAnimation::sizeHint() const {
    return QSize(20, 1);
}

void UnderlineAnimation::startAnimation() {
    anim1->setEndValue(this->width() / 2);
    anim1->start();

    anim2->setCurrentTime(0);
    //QTimer::singleShot(500, [=] {
        anim2->start();
    //});
}

void UnderlineAnimation::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setPen(Qt::transparent);
    p.setBrush(anim2->currentValue().value<QColor>());

    int mid = this->width() / 2;
    p.drawRect(mid - anim1->currentValue().toInt(), 0, anim1->currentValue().toInt() * 2, this->height());
}
