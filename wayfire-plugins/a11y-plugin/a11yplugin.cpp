/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "a11yplugin.h"

#include "stickykeys.h"

#include <wayfire/core.hpp>
#include <wayfire/seat.hpp>

extern "C" {
#include <wlr/backend/multi.h>
}

struct A11yPluginPrivate {
        wlr_backend* backend;
        wlr_keyboard keyboard;
        wl_resource* tdeKeygrabManager;
        StickyKeys* stickyKeys;

        wlr_keyboard_impl keyboardImpl = {
            "thedesk-a11y-keyboard",
            nullptr};
};

A11yPlugin::A11yPlugin() {
    d = new A11yPluginPrivate();
}

A11yPlugin::~A11yPlugin() {
    delete d;
}

void A11yPlugin::init() {
    d->backend = wlr_headless_backend_create(wf::get_core().display);
    wlr_multi_backend_add(wf::get_core().backend, d->backend);
    wlr_keyboard_init(&d->keyboard, &d->keyboardImpl, "thedesk-a11y-keyboard");

    wl_signal_emit_mutable(&d->backend->events.new_input, &d->keyboard.base);

    if (wf::get_core().get_current_state() == wf::compositor_state_t::RUNNING) {
        wlr_backend_start(d->backend);
    }

    d->stickyKeys = new StickyKeys(&d->keyboard);
}

void A11yPlugin::fini() {
    d->stickyKeys->deleteLater();
    wlr_keyboard_finish(&d->keyboard);
    wlr_multi_backend_remove(wf::get_core().backend, d->backend);
    wlr_backend_destroy(d->backend);
}

bool A11yPlugin::is_unloadable() {
    return false;
}
