/*
 * ____ HYPSEUS COPYRIGHT NOTICE ____
 *
 * Copyright (C) 2021 DirtBagXon
 *
 * This file is part of HYPSEUS SINGE, a laserdisc arcade game emulator
 *
 * HYPSEUS SINGE is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * HYPSEUS SINGE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "input.h"
#include "conout.h"
#include "keycodes.h"
#include <plog/Log.h>

// Win32 doesn't use strcmp
#ifdef WIN32
#define strcmp stricmp
#endif

#define INC(N) (N + 1)
#define TRIGGER(N) (N + AXIS_TRIGGER)

// sAn3 subset - this can be extended with decimal keycodes
int sdl2_keycode(const char *str)
{
	if (strcmp(str, "SDLK_BACKSPACE") == 0) return SDLK_BACKSPACE;
	if (strcmp(str, "SDLK_TAB") == 0) return SDLK_TAB;
	if (strcmp(str, "SDLK_RETURN") == 0) return SDLK_RETURN;
	if (strcmp(str, "SDLK_ESCAPE") == 0) return SDLK_ESCAPE;
	if (strcmp(str, "SDLK_SPACE") == 0) return SDLK_SPACE;
	if (strcmp(str, "SDLK_HASH") == 0) return SDLK_HASH;
	if (strcmp(str, "SDLK_QUOTE") == 0) return SDLK_QUOTE;
	if (strcmp(str, "SDLK_COMMA") == 0) return SDLK_COMMA;
	if (strcmp(str, "SDLK_MINUS") == 0) return SDLK_MINUS;
	if (strcmp(str, "SDLK_PERIOD") == 0) return SDLK_PERIOD;
	if (strcmp(str, "SDLK_APPLICATION") == 0) return SDLK_APPLICATION;
	if (strcmp(str, "SDLK_SLASH") == 0) return SDLK_SLASH;
	if (strcmp(str, "SDLK_0") == 0) return SDLK_0;
	if (strcmp(str, "SDLK_1") == 0) return SDLK_1;
	if (strcmp(str, "SDLK_2") == 0) return SDLK_2;
	if (strcmp(str, "SDLK_3") == 0) return SDLK_3;
	if (strcmp(str, "SDLK_4") == 0) return SDLK_4;
	if (strcmp(str, "SDLK_5") == 0) return SDLK_5;
	if (strcmp(str, "SDLK_6") == 0) return SDLK_6;
	if (strcmp(str, "SDLK_7") == 0) return SDLK_7;
	if (strcmp(str, "SDLK_8") == 0) return SDLK_8;
	if (strcmp(str, "SDLK_9") == 0) return SDLK_9;
	if (strcmp(str, "SDLK_SEMICOLON") == 0) return SDLK_SEMICOLON;
	if (strcmp(str, "SDLK_EQUALS") == 0) return SDLK_EQUALS;
	if (strcmp(str, "SDLK_LEFTBRACKET") == 0) return SDLK_LEFTBRACKET;
	if (strcmp(str, "SDLK_RIGHTBRACKET") == 0) return SDLK_RIGHTBRACKET;
	if (strcmp(str, "SDLK_BACKSLASH") == 0) return SDLK_BACKSLASH;
	if (strcmp(str, "SDLK_BACKQUOTE") == 0) return SDLK_BACKQUOTE;
	if (strcmp(str, "SDLK_a") == 0) return SDLK_a;
	if (strcmp(str, "SDLK_b") == 0) return SDLK_b;
	if (strcmp(str, "SDLK_c") == 0) return SDLK_c;
	if (strcmp(str, "SDLK_d") == 0) return SDLK_d;
	if (strcmp(str, "SDLK_e") == 0) return SDLK_e;
	if (strcmp(str, "SDLK_f") == 0) return SDLK_f;
	if (strcmp(str, "SDLK_g") == 0) return SDLK_g;
	if (strcmp(str, "SDLK_h") == 0) return SDLK_h;
	if (strcmp(str, "SDLK_i") == 0) return SDLK_i;
	if (strcmp(str, "SDLK_j") == 0) return SDLK_j;
	if (strcmp(str, "SDLK_k") == 0) return SDLK_k;
	if (strcmp(str, "SDLK_l") == 0) return SDLK_l;
	if (strcmp(str, "SDLK_m") == 0) return SDLK_m;
	if (strcmp(str, "SDLK_n") == 0) return SDLK_n;
	if (strcmp(str, "SDLK_o") == 0) return SDLK_o;
	if (strcmp(str, "SDLK_p") == 0) return SDLK_p;
	if (strcmp(str, "SDLK_q") == 0) return SDLK_q;
	if (strcmp(str, "SDLK_r") == 0) return SDLK_r;
	if (strcmp(str, "SDLK_s") == 0) return SDLK_s;
	if (strcmp(str, "SDLK_t") == 0) return SDLK_t;
	if (strcmp(str, "SDLK_u") == 0) return SDLK_u;
	if (strcmp(str, "SDLK_v") == 0) return SDLK_v;
	if (strcmp(str, "SDLK_w") == 0) return SDLK_w;
	if (strcmp(str, "SDLK_x") == 0) return SDLK_x;
	if (strcmp(str, "SDLK_y") == 0) return SDLK_y;
	if (strcmp(str, "SDLK_z") == 0) return SDLK_z;
	if (strcmp(str, "SDLK_CAPSLOCK") == 0) return SDLK_CAPSLOCK;
	if (strcmp(str, "SDLK_DELETE") == 0) return SDLK_DELETE;
	if (strcmp(str, "SDLK_F1") == 0) return SDLK_F1;
	if (strcmp(str, "SDLK_F2") == 0) return SDLK_F2;
	if (strcmp(str, "SDLK_F3") == 0) return SDLK_F3;
	if (strcmp(str, "SDLK_F4") == 0) return SDLK_F4;
	if (strcmp(str, "SDLK_F5") == 0) return SDLK_F5;
	if (strcmp(str, "SDLK_F6") == 0) return SDLK_F6;
	if (strcmp(str, "SDLK_F7") == 0) return SDLK_F7;
	if (strcmp(str, "SDLK_F8") == 0) return SDLK_F8;
	if (strcmp(str, "SDLK_F9") == 0) return SDLK_F9;
	if (strcmp(str, "SDLK_F10") == 0) return SDLK_F10;
	if (strcmp(str, "SDLK_F11") == 0) return SDLK_F11;
	if (strcmp(str, "SDLK_F12") == 0) return SDLK_F12;
	if (strcmp(str, "SDLK_SCROLLLOCK") == 0) return SDLK_SCROLLLOCK;
	if (strcmp(str, "SDLK_PAUSE") == 0) return SDLK_PAUSE;
	if (strcmp(str, "SDLK_INSERT") == 0) return SDLK_INSERT;
	if (strcmp(str, "SDLK_HOME") == 0) return SDLK_HOME;
	if (strcmp(str, "SDLK_PAGEUP") == 0) return SDLK_PAGEUP;
	if (strcmp(str, "SDLK_END") == 0) return SDLK_END;
	if (strcmp(str, "SDLK_PAGEDOWN") == 0) return SDLK_PAGEDOWN;
	if (strcmp(str, "SDLK_RIGHT") == 0) return SDLK_RIGHT;
	if (strcmp(str, "SDLK_LEFT") == 0) return SDLK_LEFT;
	if (strcmp(str, "SDLK_DOWN") == 0) return SDLK_DOWN;
	if (strcmp(str, "SDLK_UP") == 0) return SDLK_UP;
	if (strcmp(str, "SDLK_NUMLOCKCLEAR") == 0) return SDLK_NUMLOCKCLEAR;
	if (strcmp(str, "SDLK_KP_DIVIDE") == 0) return SDLK_KP_DIVIDE;
	if (strcmp(str, "SDLK_KP_MULTIPLY") == 0) return SDLK_KP_MULTIPLY;
	if (strcmp(str, "SDLK_KP_MINUS") == 0) return SDLK_KP_MINUS;
	if (strcmp(str, "SDLK_KP_PLUS") == 0) return SDLK_KP_PLUS;
	if (strcmp(str, "SDLK_KP_ENTER") == 0) return SDLK_KP_ENTER;
	if (strcmp(str, "SDLK_KP_1") == 0) return SDLK_KP_1;
	if (strcmp(str, "SDLK_KP_2") == 0) return SDLK_KP_2;
	if (strcmp(str, "SDLK_KP_3") == 0) return SDLK_KP_3;
	if (strcmp(str, "SDLK_KP_4") == 0) return SDLK_KP_4;
	if (strcmp(str, "SDLK_KP_5") == 0) return SDLK_KP_5;
	if (strcmp(str, "SDLK_KP_6") == 0) return SDLK_KP_6;
	if (strcmp(str, "SDLK_KP_7") == 0) return SDLK_KP_7;
	if (strcmp(str, "SDLK_KP_8") == 0) return SDLK_KP_8;
	if (strcmp(str, "SDLK_KP_9") == 0) return SDLK_KP_9;
	if (strcmp(str, "SDLK_KP_0") == 0) return SDLK_KP_0;
	if (strcmp(str, "SDLK_KP_PERIOD") == 0) return SDLK_KP_PERIOD;
	if (strcmp(str, "SDLK_VOLUMEUP") == 0) return SDLK_VOLUMEUP;
	if (strcmp(str, "SDLK_VOLUMEDOWN") == 0) return SDLK_VOLUMEDOWN;
	if (strcmp(str, "SDLK_LCTRL") == 0) return SDLK_LCTRL;
	if (strcmp(str, "SDLK_LSHIFT") == 0) return SDLK_LSHIFT;
	if (strcmp(str, "SDLK_LALT") == 0) return SDLK_LALT;
	if (strcmp(str, "SDLK_LGUI") == 0) return SDLK_LGUI;
	if (strcmp(str, "SDLK_RCTRL") == 0) return SDLK_RCTRL;
	if (strcmp(str, "SDLK_RSHIFT") == 0) return SDLK_RSHIFT;
	if (strcmp(str, "SDLK_RALT") == 0) return SDLK_RALT;
	if (strcmp(str, "SDLK_RGUI") == 0) return SDLK_RGUI;
	if (strcmp(str, "SDLK_MODE") == 0) return SDLK_MODE;

    LOGW << fmt("Unrecognized key macro in config: %s", str);
    LOGW << "Use decimal values for extended keycodes.";
    return SDLK_UNKNOWN;
}

// Only enum from early SDL2 for compatability
int sdl2_controller_button(const char *str)
{
	if (strcmp(str, "BUTTON_A") == 0) return INC(SDL_CONTROLLER_BUTTON_A);
	if (strcmp(str, "BUTTON_B") == 0) return INC(SDL_CONTROLLER_BUTTON_B);
	if (strcmp(str, "BUTTON_X") == 0) return INC(SDL_CONTROLLER_BUTTON_X);
	if (strcmp(str, "BUTTON_Y") == 0) return INC(SDL_CONTROLLER_BUTTON_Y);
	if (strcmp(str, "BUTTON_BACK") == 0) return INC(SDL_CONTROLLER_BUTTON_BACK);
	if (strcmp(str, "BUTTON_GUIDE") == 0) return INC(SDL_CONTROLLER_BUTTON_GUIDE);
	if (strcmp(str, "BUTTON_START") == 0) return INC(SDL_CONTROLLER_BUTTON_START);
	if (strcmp(str, "BUTTON_LEFTSTICK") == 0) return INC(SDL_CONTROLLER_BUTTON_LEFTSTICK);
	if (strcmp(str, "BUTTON_RIGHTSTICK") == 0) return INC(SDL_CONTROLLER_BUTTON_RIGHTSTICK);
	if (strcmp(str, "BUTTON_LEFTSHOULDER") == 0) return INC(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	if (strcmp(str, "BUTTON_RIGHTSHOULDER") == 0) return INC(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	if (strcmp(str, "BUTTON_DPAD_UP") == 0) return INC(SDL_CONTROLLER_BUTTON_DPAD_UP);
	if (strcmp(str, "BUTTON_DPAD_DOWN") == 0) return INC(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	if (strcmp(str, "BUTTON_DPAD_LEFT") == 0) return INC(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	if (strcmp(str, "BUTTON_DPAD_RIGHT") == 0) return INC(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	if (strcmp(str, "AXIS_TRIGGER_LEFT") == 0) return TRIGGER(SDL_CONTROLLER_AXIS_TRIGGERLEFT);
	if (strcmp(str, "AXIS_TRIGGER_RIGHT") == 0) return TRIGGER(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

    LOGW << fmt("Unrecognized button macro in config: %s", str);
    return SDLK_UNKNOWN;
}

int sdl2_controller_axis(const char *str)
{
	if (strcmp(str, "AXIS_LEFT_UP") == 0) return -2;
	if (strcmp(str, "AXIS_LEFT_DOWN") == 0) return 2;
	if (strcmp(str, "AXIS_LEFT_LEFT") == 0) return -1;
	if (strcmp(str, "AXIS_LEFT_RIGHT") == 0) return 1;
	if (strcmp(str, "AXIS_RIGHT_UP") == 0) return -4;
	if (strcmp(str, "AXIS_RIGHT_DOWN") == 0) return 4;
	if (strcmp(str, "AXIS_RIGHT_LEFT") == 0) return -3;
	if (strcmp(str, "AXIS_RIGHT_RIGHT") == 0) return 3;

    if (strncmp(str, "0", 1) != 0) {
        LOGW << fmt("Unrecognized axis macro in config: %s", str);
    }

    return SDLK_UNKNOWN;
}
