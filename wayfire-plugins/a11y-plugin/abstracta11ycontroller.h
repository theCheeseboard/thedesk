#ifndef ABSTRACTA11YCONTROLLER_H
#define ABSTRACTA11YCONTROLLER_H

#include <QObject>

struct AbstractA11yControllerPrivate;
class AbstractA11yController : public QObject {
        Q_OBJECT
    public:
        explicit AbstractA11yController(QObject* parent = nullptr);
        ~AbstractA11yController();

        void setEnabled(bool enabled);
        bool enabled();

    Q_SIGNALS:

    private:
        AbstractA11yControllerPrivate* d;

        virtual void enable() = 0;
        virtual void disable() = 0;
};

#endif // ABSTRACTA11YCONTROLLER_H
