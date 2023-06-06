#ifndef SCREENSHOTMANAGER_H
#define SCREENSHOTMANAGER_H

#include <QObject>

struct ScreenshotManagerPrivate;
class ScreenshotManager : public QObject {
        Q_OBJECT
    public:
        explicit ScreenshotManager(QObject* parent = nullptr);
        ~ScreenshotManager();

        void setupForTheDesk();
        void showScreenshotWindows();

        QPixmap finalPixmap();
        bool accepted();

    signals:
        void finished();

    private:
        ScreenshotManagerPrivate* d;

        void accept(QPixmap pixmap);
        void reject();
};

#endif // SCREENSHOTMANAGER_H
