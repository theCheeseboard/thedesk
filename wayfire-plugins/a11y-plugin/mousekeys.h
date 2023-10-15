#ifndef MOUSEKEYS_H
#define MOUSEKEYS_H

#include "abstracta11ycontroller.h"
#include <wayfire/signal-definitions.hpp>

struct MouseKeysPrivate;
class MouseKeys : public AbstractA11yController {
        Q_OBJECT
    public:
        explicit MouseKeys(wlr_keyboard* keyboard, wlr_pointer* pointer, QObject* parent = nullptr);
        ~MouseKeys();

    protected:
        friend MouseKeysPrivate;
        void keyboardButtonPressed(wf::input_event_signal<wlr_keyboard_key_event>* event);
        int mouseKeysTimer();

    private:
        MouseKeysPrivate* d;

        void updateMouseKeysState();

        // AbstractA11yController interface
    private:
        void enable();
        void disable();

        void clickMouse();
};

#endif // MOUSEKEYS_H
