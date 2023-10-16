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
#ifndef KEYGRABPLUGIN_H
#define KEYGRABPLUGIN_H

#include <wayfire/plugin.hpp>

struct wl_client;
struct A11yPluginPrivate;
class A11yPlugin : public wf::plugin_interface_t {
    public:
        A11yPlugin();
        ~A11yPlugin();

        // plugin_interface_t interface
    public:
        void init();
        void fini();
        bool is_unloadable();

    private:
        A11yPluginPrivate* d;
};

DECLARE_WAYFIRE_PLUGIN(A11yPlugin)

#endif // KEYGRABPLUGIN_H
