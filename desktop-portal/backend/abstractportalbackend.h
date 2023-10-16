#ifndef ABSTRACTPORTALBACKEND_H
#define ABSTRACTPORTALBACKEND_H

#include <QCoroTask>
#include <QObject>
#include <QScreen>

class AbstractPortalBackend : public QObject {
        Q_OBJECT
    public:
        explicit AbstractPortalBackend(QObject* parent = nullptr);

        static AbstractPortalBackend* instance();

        virtual QCoro::Task<QPixmap> takeScreenshot(QScreen* screen) = 0;

    signals:
};

#endif // ABSTRACTPORTALBACKEND_H
