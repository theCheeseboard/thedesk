#include "screencastsourceselectionitem.h"
#include "ui_screencastsourceselectionitem.h"

#include "backend/abstractscreencopysession.h"
#include <QPainter>

struct ScreencastSourceSelectionItemPrivate {
        QImage image;

        AbstractScreenCopySession* session;
};

ScreencastSourceSelectionItem::ScreencastSourceSelectionItem(ScreenCopySource source, QWidget* parent) :
    QWidget(parent), ui(new Ui::ScreencastSourceSelectionItem) {
    ui->setupUi(this);
    d = new ScreencastSourceSelectionItemPrivate();

    d->session = AbstractScreenCopySession::sessionFor(source);
    connect(d->session, &AbstractScreenCopySession::imageReady, this, [this] {
        d->image = d->session->currentImage();
        ui->imageWidget->update();
    });

    ui->label->setText(source.name);
    ui->imageWidget->installEventFilter(this);
}

ScreencastSourceSelectionItem::~ScreencastSourceSelectionItem() {
    delete ui;
    d->session->deleteLater();
    delete d;
}

bool ScreencastSourceSelectionItem::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->imageWidget && event->type() == QEvent::Paint) {
        QRect imageRect;
        imageRect.setSize(d->image.size().scaled(ui->imageWidget->size(), Qt::KeepAspectRatio));
        imageRect.moveCenter(QRect(QPoint(0, 0), ui->imageWidget->size()).center());

        QPainter painter(ui->imageWidget);
        painter.drawImage(imageRect, d->image);
    }
    return false;
}
