#include "stickykeys.h"

#include <QTextStream>
#include <linux/input-event-codes.h>
#include <wayfire/bindings-repository.hpp>
#include <wayfire/core.hpp>
#include <wayfire/seat.hpp>

struct StickyKeysPrivate {
        wlr_keyboard* keyboard;
        StickyKeys* parent;

        wf::signal::connection_t<wf::input_event_signal<wlr_keyboard_key_event>> keyboardButtonEvent = [this](wf::input_event_signal<wlr_keyboard_key_event>* ev) {
            parent->keyboardButtonPressed(ev);
        };

        Qt::KeyboardModifiers heldModifiers = Qt::NoModifier;
        Qt::KeyboardModifiers latchedModifiers = Qt::NoModifier;

        bool processing = false;

        class KeySession {
            public:
                KeySession(wlr_keyboard* keyboard) {
                    this->keyboard = keyboard;
                }

                ~KeySession() {
                    for (auto keycode = keycodes.crbegin(); keycode != keycodes.crend(); keycode++) {
                        wlr_keyboard_key_event newEvent;
                        newEvent.keycode = *keycode;
                        newEvent.update_state = true;
                        newEvent.time_msec = wf::get_current_time();
                        newEvent.state = WL_KEYBOARD_KEY_STATE_RELEASED;
                        wlr_keyboard_notify_key(keyboard, &newEvent);
                    }
                }

                void push(quint32 keycode) {
                    keycodes.append(keycode);

                    wlr_keyboard_key_event newEvent;
                    newEvent.keycode = keycode;
                    newEvent.update_state = true;
                    newEvent.time_msec = wf::get_current_time();
                    newEvent.state = WL_KEYBOARD_KEY_STATE_PRESSED;
                    wlr_keyboard_notify_key(keyboard, &newEvent);
                }

            private:
                QList<quint32> keycodes;
                wlr_keyboard* keyboard;
        };
};

StickyKeys::StickyKeys(wlr_keyboard* keyboard, QObject* parent) :
    AbstractA11yController{parent} {
    d = new StickyKeysPrivate();
    d->parent = this;
    d->keyboard = keyboard;
}

StickyKeys::~StickyKeys() {
    delete d;
}

void StickyKeys::enable() {
    wf::get_core().connect(&d->keyboardButtonEvent);
}

void StickyKeys::disable() {
    wf::get_core().disconnect(&d->keyboardButtonEvent);
}

void StickyKeys::cycleModifier(Qt::KeyboardModifier modifier) {
    if (d->latchedModifiers.testFlag(modifier)) {
        // Turn off the latch
        d->latchedModifiers = d->latchedModifiers.setFlag(modifier, false);
    } else if (d->heldModifiers.testFlag(modifier)) {
        // Latch this key
        d->heldModifiers = d->heldModifiers.setFlag(modifier, false);
        d->latchedModifiers = d->latchedModifiers.setFlag(modifier, true);
    } else {
        // Hold this key
        d->heldModifiers = d->heldModifiers.setFlag(modifier);
    }
}

void StickyKeys::keyboardButtonPressed(wf::input_event_signal<wlr_keyboard_key_event>* event) {
    if (d->processing) return;
    QTextStream(stderr) << "Processing keycode: " << event->event->keycode << "  state: " << event->event->state << "\n";

    switch (event->event->keycode) {
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
            event->mode = wf::input_event_processing_mode_t::IGNORE;
            if (event->event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                // Cycle the CTRL key
                this->cycleModifier(Qt::ControlModifier);
            }
            break;
        case KEY_LEFTALT:
        case KEY_RIGHTALT:
            event->mode = wf::input_event_processing_mode_t::IGNORE;
            if (event->event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                // Cycle the ALT key
                this->cycleModifier(Qt::AltModifier);
            }
            break;
        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
            event->mode = wf::input_event_processing_mode_t::IGNORE;
            if (event->event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                // Cycle the SHIFT key
                this->cycleModifier(Qt::ShiftModifier);
            }
            break;
        case KEY_LEFTMETA:
        case KEY_RIGHTMETA:
            event->mode = wf::input_event_processing_mode_t::IGNORE;
            if (event->event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                // Cycle the META key
                this->cycleModifier(Qt::MetaModifier);
            }
            break;
        default:
            {
                // If we have any keys held or latched, press them now
                auto thisModifiers = d->heldModifiers | d->latchedModifiers;

                if (thisModifiers != Qt::NoModifier) {
                    event->mode = wf::input_event_processing_mode_t::IGNORE;
                    if (event->event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                        QTextStream(stderr) << "Pressing keycode: " << event->event->keycode << "  modifiers: " << thisModifiers << "\n";
                        d->processing = true;

                        {
                            StickyKeysPrivate::KeySession session(d->keyboard);
                            if (thisModifiers.testFlag(Qt::ControlModifier)) session.push(KEY_LEFTCTRL);
                            if (thisModifiers.testFlag(Qt::AltModifier)) session.push(KEY_LEFTALT);
                            if (thisModifiers.testFlag(Qt::ShiftModifier)) session.push(KEY_LEFTSHIFT);
                            if (thisModifiers.testFlag(Qt::MetaModifier)) session.push(KEY_LEFTMETA);
                            session.push(event->event->keycode);
                        }

                        d->processing = false;
                    }
                }

                // Clear out all the held modifiers
                d->heldModifiers = Qt::NoModifier;
            }
    }
}
