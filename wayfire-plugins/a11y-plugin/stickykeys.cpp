#include "stickykeys.h"

#include "wayland-tdesktopenvironment-accessibility-v1-server-protocol.h"
#include <QTextStream>
#include <linux/input-event-codes.h>
#include <wayfire/bindings-repository.hpp>
#include <wayfire/core.hpp>
#include <wayfire/seat.hpp>

struct StickyKeysPrivate {
        wlr_keyboard* keyboard;
        wl_resource* tdeA11yStickyKeys;
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

    wl_global_create(wf::get_core().display, &tdesktopenvironment_accessibility_sticky_keys_v1_interface, 1, this, [](wl_client* client, void* data, uint32_t version, uint32_t id) {
        auto* plugin = reinterpret_cast<StickyKeys*>(data);

        plugin->d->tdeA11yStickyKeys = wl_resource_create(client, &tdesktopenvironment_accessibility_sticky_keys_v1_interface, 1, id);

        auto interface = new struct tdesktopenvironment_accessibility_sticky_keys_v1_interface();
        interface->set_enabled = [](struct wl_client* client, struct wl_resource* resource, uint enabled) {
            reinterpret_cast<StickyKeys*>(resource->data)->setEnabled(enabled);
        };
        wl_resource_set_implementation(plugin->d->tdeA11yStickyKeys, interface, plugin, nullptr);
    });
}

StickyKeys::~StickyKeys() {
    delete d;
}

void StickyKeys::enable() {
    wf::get_core().connect(&d->keyboardButtonEvent);
    this->updateStickyKeysState();
}

void StickyKeys::disable() {
    wf::get_core().disconnect(&d->keyboardButtonEvent);
    this->updateStickyKeysState();
}

void StickyKeys::updateStickyKeysState() {
    tdesktopenvironment_accessibility_sticky_keys_v1_send_sticky_keys_enabled(d->tdeA11yStickyKeys, this->enabled());
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

    int heldMods = 0;
    if (d->heldModifiers.testFlag(Qt::ControlModifier)) heldMods |= TDESKTOPENVIRONMENT_ACCESSIBILITY_STICKY_KEYS_V1_MODIFIER_CONTROL;
    if (d->heldModifiers.testFlag(Qt::AltModifier)) heldMods |= TDESKTOPENVIRONMENT_ACCESSIBILITY_STICKY_KEYS_V1_MODIFIER_ALT;
    if (d->heldModifiers.testFlag(Qt::ShiftModifier)) heldMods |= TDESKTOPENVIRONMENT_ACCESSIBILITY_STICKY_KEYS_V1_MODIFIER_SHIFT;
    if (d->heldModifiers.testFlag(Qt::MetaModifier)) heldMods |= TDESKTOPENVIRONMENT_ACCESSIBILITY_STICKY_KEYS_V1_MODIFIER_SUPER;

    int latchedMods = 0;
    if (d->latchedModifiers.testFlag(Qt::ControlModifier)) latchedMods |= TDESKTOPENVIRONMENT_ACCESSIBILITY_STICKY_KEYS_V1_MODIFIER_CONTROL;
    if (d->latchedModifiers.testFlag(Qt::AltModifier)) latchedMods |= TDESKTOPENVIRONMENT_ACCESSIBILITY_STICKY_KEYS_V1_MODIFIER_ALT;
    if (d->latchedModifiers.testFlag(Qt::ShiftModifier)) latchedMods |= TDESKTOPENVIRONMENT_ACCESSIBILITY_STICKY_KEYS_V1_MODIFIER_SHIFT;
    if (d->latchedModifiers.testFlag(Qt::MetaModifier)) latchedMods |= TDESKTOPENVIRONMENT_ACCESSIBILITY_STICKY_KEYS_V1_MODIFIER_SUPER;

    tdesktopenvironment_accessibility_sticky_keys_v1_send_sticky_keys_held(d->tdeA11yStickyKeys, heldMods);
    tdesktopenvironment_accessibility_sticky_keys_v1_send_sticky_keys_latched(d->tdeA11yStickyKeys, latchedMods);
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
                tdesktopenvironment_accessibility_sticky_keys_v1_send_sticky_keys_held(d->tdeA11yStickyKeys, heldMods);
            }
    }
}
