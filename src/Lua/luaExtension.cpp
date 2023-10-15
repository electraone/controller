/*
* Electra One MIDI Controller Firmware
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Electra One Project (http://electra.one/).
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.
*/

#include "luaExtension.h"

void loadLuaLibs(void)
{
    static const luaL_Reg ctrlv2libs[] = { { "parameterMap",
                                             luaopen_parameterMap },
                                           { "pages", luaopen_pages },
                                           { "groups", luaopen_groups },
                                           { "devices", luaopen_devices },
                                           { "controls", luaopen_controls },
                                           { "helpers", luaopen_helpers },
                                           { "patch", luaopen_patch },
                                           { "preset", luaopen_preset },
                                           { "info", luaopen_info },
                                           { "events", luaopen_events },
                                           { "overlays", luaopen_overlays },
                                           { NULL, NULL } };

    luaLE_openEoslibs(L, ctrlv2libs);
    value_register(L);
    page_register(L);
    group_register(L);
    device_register(L);
    control_register(L);
    message_register(L);
    overlay_register(L);

    // Clear the stack
    lua_settop(L, 0);
}

/** @todo Get rid of this global variables */
Preset *luaPreset = nullptr;
Presets *luaPresets = nullptr;
MainDelegate *luaDelegate = nullptr;