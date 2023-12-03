#ifndef SCREENCASTSESSION_H
#define SCREENCASTSESSION_H

#include <QDBusObjectPath>
#include <QObject>

struct ScreencastSessionPrivate;
class ScreencastSession : public QObject {
        Q_OBJECT
    public:
        explicit ScreencastSession(QDBusObjectPath sessionHandle, QObject* parent = nullptr);
        ~ScreencastSession();

        void configureOptions(QVariantMap options);

    signals:
        void done();

    private:
        ScreencastSessionPrivate* d;
};

typedef QSharedPointer<ScreencastSession> ScreencastSessionPtr;

#endif // SCREENCASTSESSION_H
