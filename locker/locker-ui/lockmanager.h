#ifndef LOCKMANAGER_H
#define LOCKMANAGER_H

#include <QObject>

struct LockManagerPrivate;
class LockManager : public QObject {
        Q_OBJECT
    public:
        static LockManager* instance();

        void setDebug(bool debugMode);

        void openLockWindows();
        void establishGrab();
        void raiseAll();

    signals:

    private:
        explicit LockManager(QObject* parent = nullptr);

        LockManagerPrivate* d;
};

#endif // LOCKMANAGER_H
