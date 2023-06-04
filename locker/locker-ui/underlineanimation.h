#ifndef UNDERLINEANIMATION_H
#define UNDERLINEANIMATION_H

#include <QWidget>
#include <QPaintEvent>
#include <tvariantanimation.h>
#include <QPainter>
#include <QTimer>

class UnderlineAnimation : public QWidget
{
        Q_OBJECT
    public:
        explicit UnderlineAnimation(QWidget *parent = nullptr);


        QSize sizeHint() const;
    signals:

    public slots:
        void startAnimation();

    private:
        void paintEvent(QPaintEvent* event);

        tVariantAnimation *anim1, *anim2;
};

#endif // UNDERLINEANIMATION_H
