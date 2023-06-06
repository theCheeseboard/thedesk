#include "screenshotinterface.h"

#include "portalcommon.h"
#include "screenshotmanager.h"
#include <QCoroCore>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QPainter>
#include <QScreen>
#include <QTemporaryFile>
#include <QUrl>
#include <tapplication.h>

QDBusArgument& operator<<(QDBusArgument& argument, const ScreenshotColorReply& myStruct) {
    argument.beginStructure();
    argument << myStruct.red << myStruct.green << myStruct.blue;
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, ScreenshotColorReply& myStruct) {
    argument.beginStructure();
    argument >> myStruct.red >> myStruct.green >> myStruct.blue;
    argument.endStructure();
    return argument;
}

ScreenshotInterface::ScreenshotInterface(QObject* parent) :
    QDBusAbstractAdaptor{parent} {
    qDBusRegisterMetaType<ScreenshotColorReply>();
}

uint ScreenshotInterface::version() {
    return 2;
}

uint ScreenshotInterface::Screenshot(QDBusObjectPath handle, QString app_id, QString parent_window, QVariantMap options, const QDBusMessage& message, QVariantMap& results) {
    PortalCommon::setupCoro([options, parent_window, handle, this, app_id](QDBusMessage reply) -> QCoro::Task<> {
        auto interactive = options.value("interactive", false).toBool();
        auto permissionStoreChecked = options.value("permission_store_checked", false).toBool();
        auto isTheDesk = app_id == "com.vicr123.thedesk" && options.value("x-thedesk-screenshot", false).toBool();

        QPixmap finalPixmap;
        if (interactive) {
            ScreenshotManager mgr;
            if (isTheDesk) {
                mgr.setType(ScreenshotWindow::Type::TheDeskScreenshot);
            }
            mgr.showScreenshotWindows();

            co_await qCoro(&mgr, &ScreenshotManager::finished);
            if (!mgr.accepted()) {
                reply.setArguments({uint(1), QVariantMap()});
                QDBusConnection::sessionBus().send(reply);
                co_return;
            }

            finalPixmap = mgr.finalPixmap();
        } else {
            if (!permissionStoreChecked) {
                // We haven't checked permissions of the app, so deny the request
                reply.setArguments({uint(2), QVariantMap()});
                QDBusConnection::sessionBus().send(reply);
                co_return;
            }

            // Compile a picture
            QRect rect;
            for (auto screen : qApp->screens()) {
                rect = rect.united(screen->geometry());
            }

            QPixmap pixmap(rect.size());
            pixmap.fill(Qt::black);
            QPainter painter(&pixmap);
            for (auto screen : qApp->screens()) {
                painter.drawPixmap(screen->geometry(), screen->grabWindow());
            }

            finalPixmap = pixmap;
        }

        QTemporaryFile temp;
        temp.setAutoRemove(false);
        temp.open();

        finalPixmap.save(&temp, "PNG");
        temp.close();

        reply.setArguments({
            uint(0), QVariantMap({{"uri", QUrl::fromLocalFile(temp.fileName()).toString()}}
             )
        });
        QDBusConnection::sessionBus().send(reply);
    },
        message);
    return 0;
}

uint ScreenshotInterface::PickColor(QDBusObjectPath handle, QString app_id, QString parent_window, QVariantMap options, const QDBusMessage& message, QVariantMap& results) {
    PortalCommon::setupCoro([options, parent_window, handle, this, app_id](QDBusMessage reply) -> QCoro::Task<> {
        ScreenshotManager mgr;
        mgr.setType(ScreenshotWindow::Type::ColourPicker);
        mgr.showScreenshotWindows();

        co_await qCoro(&mgr, &ScreenshotManager::finished);
        if (!mgr.accepted()) {
            reply.setArguments({uint(1), QVariantMap()});
            QDBusConnection::sessionBus().send(reply);
            co_return;
        }

        auto color = mgr.clickedColor();

        ScreenshotColorReply replyArg;
        replyArg.red = color.redF();
        replyArg.green = color.greenF();
        replyArg.blue = color.blueF();

        QDBusArgument arg;
        arg << replyArg;

        reply.setArguments({uint(0), QVariantMap{{"color", QVariant::fromValue(arg)}}});
        QDBusConnection::sessionBus().send(reply);
    },
        message);
    return 0;
}
