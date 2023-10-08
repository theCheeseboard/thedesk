#ifndef STICKYKEYS_H
#define STICKYKEYS_H

#include "abstracta11ycontroller.h"
#include <wayfire/signal-definitions.hpp>

struct StickyKeysPrivate;
class StickyKeys : public AbstractA11yController {
        Q_OBJECT
    public:
        explicit StickyKeys(wlr_keyboard* keyboard, QObject* parent = nullptr);
        ~StickyKeys();

    Q_SIGNALS:

    protected:
        friend StickyKeysPrivate;
        void keyboardButtonPressed(wf::input_event_signal<wlr_keyboard_key_event>* event);

    private:
        StickyKeysPrivate* d;

        // AbstractA11yController interface
    private:
        void enable();
        void disable();

        void cycleModifier(Qt::KeyboardModifier modifier);
};

#endif // STICKYKEYS_H
