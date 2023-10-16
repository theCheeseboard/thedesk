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
#ifndef STARTDESKPLUGIN_H
#define STARTDESKPLUGIN_H

#include <wayfire/plugin.hpp>

class StartdeskPlugin : public wf::plugin_interface_t {
    public:
        StartdeskPlugin();

    private:
        wf::wl_timer<true> waiter;

        // plugin_interface_t interface
    public:
        void init();
};

DECLARE_WAYFIRE_PLUGIN(StartdeskPlugin)

#endif // STARTDESKPLUGIN_H
