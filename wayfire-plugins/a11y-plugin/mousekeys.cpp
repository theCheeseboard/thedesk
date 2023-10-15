#include "mousekeys.h"

#include "wayland-tdesktopenvironment-accessibility-v1-server-protocol.h"
#include <QDateTime>
#include <QTextStream>
#include <linux/input-event-codes.h>
#include <wlr/types/wlr_cursor.h>

struct MouseKeysPrivate {
        static constexpr int mouseKeysDelay = 100;
        static constexpr int mouseKeysPixels = 1;

        wlr_keyboard* keyboard;
        wlr_pointer* pointer;
        wl_resource* tdeA11yMouseKeys;
        MouseKeys* parent;

        wl_event_loop* eventLoop;
        wl_event_source* timer;

        Qt::Edges moveDirection;
        QDateTime lastPress;

        wf::signal::connection_t<wf::input_event_signal<wlr_keyboard_key_event>> keyboardButtonEvent = [this](wf::input_event_signal<wlr_keyboard_key_event>* ev) {
            parent->keyboardButtonPressed(ev);
        };

        static int mouseKeysTimer(void* data) {
            return reinterpret_cast<MouseKeys*>(data)->mouseKeysTimer();
        }
};

MouseKeys::MouseKeys(wlr_keyboard* keyboard, wlr_pointer* pointer, QObject* parent) :
    AbstractA11yController{parent} {
    d = new MouseKeysPrivate();
    d->parent = this;
    d->keyboard = keyboard;
    d->pointer = pointer;

    wl_global_create(wf::get_core().display, &tdesktopenvironment_accessibility_mouse_keys_v1_interface, 1, this, [](wl_client* client, void* data, uint32_t version, uint32_t id) {
        auto* plugin = reinterpret_cast<MouseKeys*>(data);

        plugin->d->tdeA11yMouseKeys = wl_resource_create(client, &tdesktopenvironment_accessibility_mouse_keys_v1_interface, 1, id);

        auto interface = new struct tdesktopenvironment_accessibility_sticky_keys_v1_interface();
        interface->set_enabled = [](struct wl_client* client, struct wl_resource* resource, uint enabled) {
            reinterpret_cast<MouseKeys*>(resource->data)->setEnabled(enabled);
        };
        wl_resource_set_implementation(plugin->d->tdeA11yMouseKeys, interface, plugin, nullptr);
    });

    d->eventLoop = wl_display_get_event_loop(wf::get_core().display);
    d->timer = wl_event_loop_add_timer(d->eventLoop, MouseKeysPrivate::mouseKeysTimer, this);
}

MouseKeys::~MouseKeys() {
    wl_event_source_remove(d->timer);
    delete d;
}

void MouseKeys::keyboardButtonPressed(wf::input_event_signal<wlr_keyboard_key_event>* event) {
    Qt::Edges edges;
    switch (event->event->keycode) {
        case KEY_KP7:
            edges = Qt::LeftEdge | Qt::TopEdge;
            break;
        case KEY_KP8:
            edges = Qt::TopEdge;
            break;
        case KEY_KP9:
            edges = Qt::RightEdge | Qt::TopEdge;
            break;
        case KEY_KP4:
            edges = Qt::LeftEdge;
            break;
        case KEY_KP6:
            edges = Qt::RightEdge;
            break;
        case KEY_KP1:
            edges = Qt::LeftEdge | Qt::BottomEdge;
            break;
        case KEY_KP2:
            edges = Qt::BottomEdge;
            break;
        case KEY_KP3:
            edges = Qt::RightEdge | Qt::BottomEdge;
            break;
        case KEY_KP5:
            {
                if (event->event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                    this->pressMouse();
                } else if (event->event->state == WL_KEYBOARD_KEY_STATE_RELEASED) {
                    this->releaseMouse();
                }
                return;
            }
    }

    if (edges == 0) return;

    event->mode = wf::input_event_processing_mode_t::IGNORE;

    if (event->event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        // Always override the existing movement
        d->moveDirection = edges;
        d->lastPress = QDateTime::currentDateTimeUtc();

        // Always move the mouse at least once
        processMouseMovement();
    } else if (event->event->state == WL_KEYBOARD_KEY_STATE_RELEASED) {
        // Only stop movement if the existing movement is the same
        if (d->moveDirection == edges) {
            d->moveDirection = {};
        }
    }
}

int MouseKeys::mouseKeysTimer() {
    if (!this->enabled()) return 0;

    if (d->moveDirection) processMouseMovement();

    wl_event_source_timer_update(d->timer, d->mouseKeysDelay);
    return 1;
}

void MouseKeys::enable() {
    wf::get_core().connect(&d->keyboardButtonEvent);
    wl_event_source_timer_update(d->timer, d->mouseKeysDelay);
    this->updateMouseKeysState();
}

void MouseKeys::disable() {
    wf::get_core().disconnect(&d->keyboardButtonEvent);
    this->updateMouseKeysState();
}

void MouseKeys::pressMouse() {
    wlr_pointer_button_event ev;
    ev.pointer = d->pointer;
    ev.button = BTN_LEFT;
    ev.state = WLR_BUTTON_PRESSED;
    ev.time_msec = wf::get_current_time();
    wl_signal_emit(&d->pointer->events.button, &ev);
    wl_signal_emit(&d->pointer->events.frame, nullptr);
}

void MouseKeys::releaseMouse() {
    wlr_pointer_button_event ev;
    ev.pointer = d->pointer;
    ev.button = BTN_LEFT;
    ev.state = WLR_BUTTON_RELEASED;
    ev.time_msec = wf::get_current_time();
    wl_signal_emit(&d->pointer->events.button, &ev);
    wl_signal_emit(&d->pointer->events.frame, nullptr);
}

void MouseKeys::processMouseMovement() {
    double x = 0;
    double y = 0;

    auto movement = d->mouseKeysPixels;
    auto stride = qMin(5000, d->lastPress.msecsTo(QDateTime::currentDateTimeUtc()));
    do {
        movement += d->mouseKeysPixels;
        stride -= 500;
    } while (stride >= 0);

    if (d->moveDirection & Qt::LeftEdge) x -= movement;
    if (d->moveDirection & Qt::RightEdge) x += movement;
    if (d->moveDirection & Qt::TopEdge) y -= movement;
    if (d->moveDirection & Qt::BottomEdge) y += movement;

    wlr_pointer_motion_event ev;
    ev.pointer = d->pointer;
    ev.time_msec = wf::get_current_time();
    ev.delta_x = ev.unaccel_dx = x;
    ev.delta_y = ev.unaccel_dy = y;
    wl_signal_emit(&d->pointer->events.motion, &ev);
    wl_signal_emit(&d->pointer->events.frame, nullptr);
}

void MouseKeys::updateMouseKeysState() {
    tdesktopenvironment_accessibility_mouse_keys_v1_send_mouse_keys_enabled(d->tdeA11yMouseKeys, this->enabled());
}
