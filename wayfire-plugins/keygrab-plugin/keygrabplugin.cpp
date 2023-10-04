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
#include "keygrabplugin.h"

#include <wayfire/bindings-repository.hpp>
#include <wayfire/core.hpp>
#include <wayfire/output.hpp>

#include "wayland-tdesktopenvironment-keygrab-v1-server-protocol.h"
#include <iostream>
#include <wlr/types/wlr_xdg_foreign_registry.h>

struct KeygrabPluginPrivate {
        wl_resource* tdeKeygrabManager;
};

KeygrabPlugin::KeygrabPlugin() {
    d = new KeygrabPluginPrivate();
}

KeygrabPlugin::~KeygrabPlugin() {
    delete d;
}

void KeygrabPlugin::grabKey(wl_client* client, uint32_t mod, uint32_t key) {
    std::cout << "Grabbing key " << key << " mod " << mod << "\n";
    wf::get_core().bindings->add_key(wf::create_option(wf::keybinding_t(mod, key)), new wf::key_callback([=](const wf::keybinding_t& keybind) {
        std::cout << "Pressed " << key << " mod " << mod << "\n";
        tdesktopenvironment_keygrab_manager_v1_send_activated(d->tdeKeygrabManager, mod, key, 0);
        return true;
    }));
}

void KeygrabPlugin::ungrabKey(wl_client* client, uint32_t mod, uint32_t key) {
}

void KeygrabPlugin::init() {
    wf::get_core().bindings->add_key(wf::create_option(wf::keybinding_t(0, 36)), new wf::key_callback([=](const wf::keybinding_t& key) {
        std::cout << "Pressed J\n";
        return true;
    }));

    //    wf::get_core().
    wl_global_create(wf::get_core().display, &tdesktopenvironment_keygrab_manager_v1_interface, 1, this, [](wl_client* client, void* data, uint32_t version, uint32_t id) {
        KeygrabPlugin* plugin = reinterpret_cast<KeygrabPlugin*>(data);

        plugin->d->tdeKeygrabManager = wl_resource_create(client, &tdesktopenvironment_keygrab_manager_v1_interface, 1, id);

        struct tdesktopenvironment_keygrab_manager_v1_interface* interface = new struct tdesktopenvironment_keygrab_manager_v1_interface();
        interface->grab_key = [](struct wl_client* client, struct wl_resource* resource, uint32_t mod, uint32_t key) {
            reinterpret_cast<KeygrabPlugin*>(resource->data)->grabKey(client, mod, key);
        };
        interface->ungrab_key = [](struct wl_client* client, struct wl_resource* resource, uint32_t mod, uint32_t key) {
            reinterpret_cast<KeygrabPlugin*>(resource->data)->ungrabKey(client, mod, key);
        };
        interface->destroy = [](struct wl_client* client, struct wl_resource* resource) {

        };
        wl_resource_set_implementation(plugin->d->tdeKeygrabManager, interface, plugin, nullptr);
    });
}

void KeygrabPlugin::fini() {
}

bool KeygrabPlugin::is_unloadable() {
    return false;
}
