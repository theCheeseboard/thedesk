#ifndef SCREENSHOTMANAGER_H
#define SCREENSHOTMANAGER_H

#include "dialogs/screenshotwindow.h"
#include <QObject>

struct ScreenshotManagerPrivate;
class ScreenshotManager : public QObject {
        Q_OBJECT
    public:
        explicit ScreenshotManager(QObject* parent = nullptr);
        ~ScreenshotManager();

        void setType(ScreenshotWindow::Type type);
        void showScreenshotWindows();

        QPixmap finalPixmap();
        QColor clickedColor();
        bool accepted();

    signals:
        void finished();

    private:
        ScreenshotManagerPrivate* d;

        void acceptPixmap(QPixmap pixmap);
        void acceptColor(QColor color);
        void reject();
};

#endif // SCREENSHOTMANAGER_H
