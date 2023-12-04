#ifndef SCREENCASTSOURCESELECTIONITEM_H
#define SCREENCASTSOURCESELECTIONITEM_H

#include "backend/abstractscreencopysession.h"
#include <QWidget>

namespace Ui {
    class ScreencastSourceSelectionItem;
}

struct ScreencastSourceSelectionItemPrivate;
class ScreencastSourceSelectionItem : public QWidget {
        Q_OBJECT

    public:
        explicit ScreencastSourceSelectionItem(ScreenCopySource source, QWidget* parent = nullptr);
        ~ScreencastSourceSelectionItem();

    private:
        Ui::ScreencastSourceSelectionItem* ui;
        ScreencastSourceSelectionItemPrivate* d;

        // QObject interface
    public:
        bool eventFilter(QObject *watched, QEvent *event);
};

#endif // SCREENCASTSOURCESELECTIONITEM_H
