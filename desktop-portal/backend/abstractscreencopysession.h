#ifndef ABSTRACTSCREENCOPYSESSION_H
#define ABSTRACTSCREENCOPYSESSION_H

#include <QObject>

struct ScreenCopySource {
        QString name;
        void* data;
};

class AbstractScreenCopySession : public QObject {
        Q_OBJECT
    public:
        explicit AbstractScreenCopySession(QObject* parent = nullptr);

        virtual QImage currentImage() = 0;
        static AbstractScreenCopySession* sessionFor(ScreenCopySource source);

    signals:
        void imageReady();
};

#endif // ABSTRACTSCREENCOPYSESSION_H
