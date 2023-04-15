/*
* ____ DAPHNE COPYRIGHT NOTICE ____
*
* Copyright (C) 2006 Scott C. Duensing
*
* This file is part of DAPHNE, a laserdisc arcade game emulator
*
* DAPHNE is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* DAPHNE is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
* This is SINGE - the Somewhat Interactive Nostalgic Game Engine!
*/

#include "config.h"

#include "singe.h"
#include "singe/singe_interface.h"

// Win32 doesn't use strcasecmp, it uses stricmp (lame)
#ifdef WIN32
#define strcasecmp stricmp
#endif

////////////////////////////////////////////////////////////////////////////////

// For intercepting the VLDP MPEG data

extern struct vldp_in_info g_local_info;
extern const struct vldp_out_info *g_vldp_info;

////////////////////////////////////////////////////////////////////////////////

typedef const struct singe_out_info *(*singeinitproc)(const struct singe_in_info *in_info);

// pointer to all info SINGE PROXY gives to us
const struct singe_out_info *g_pSingeOut = NULL;

// info that we provide to the SINGE PROXY DLL
struct singe_in_info g_SingeIn;

// joystick
static Sint16 g_singe_xpos, g_singe_ypos, g_singe_jrelx,
              g_singe_jrely, g_singe_xmov, g_singe_ymov;
static Sint16 g_singe_js_sen = 5;
static bool   g_singe_bjx = false, g_singe_bjy = false;

////////////////////////////////////////////////////////////////////////////////

// by RDG2010
const int singe::i_full_keybd_defs[] = {SDLK_BACKSPACE,    SDLK_TAB,
                                        SDLK_RETURN,       SDLK_PAUSE,
                                        SDLK_SPACE,        SDLK_QUOTE,
                                        SDLK_COMMA,        SDLK_SEMICOLON,
                                        SDLK_EQUALS,       SDLK_LEFTBRACKET,
                                        SDLK_RIGHTBRACKET, SDLK_BACKSLASH,
                                        SDLK_SLASH,        SDLK_DELETE,
                                        SDLK_PERIOD};

#define KEYBD_ARRAY_SIZE 15

singe::singe()
{
    m_strGameScript           = "";
    m_shortgamename           = "singe";
    m_strName                 = "[Undefined scripted game]";
    m_video_overlay_width     = 320; // sensible default
    m_video_overlay_height    = 240; // " " "
    m_palette_color_count     = 256;
    m_overlay_size_is_dynamic = true; // this 'game' does reallocate the size of
                                      // its overlay
    m_bMouseEnabled = true;
    m_dll_instance  = NULL;
    // by RDG2010
    m_game_type     = GAME_SINGE;
    i_keyboard_mode = KEYBD_NORMAL;

    m_use_virtual_overlay = true;
}

bool singe::init()
{
    bool bSuccess = false;
    singeinitproc pSingeInit; // pointer to the init proc ...

    pSingeInit = singeproxy_init;
    bSuccess   = true;

    // if pSingeInit is valid ...
    if (bSuccess) {
        g_SingeIn.uVersion            = SINGE_INTERFACE_API_VERSION;
        g_SingeIn.printline           = printline;
        g_SingeIn.set_quitflag        = set_quitflag;
        g_SingeIn.set_singe_errors    = set_singe_errors;
        g_SingeIn.disable_audio1      = disable_audio1;
        g_SingeIn.disable_audio2      = disable_audio2;
        g_SingeIn.enable_audio1       = enable_audio1;
        g_SingeIn.enable_audio2       = enable_audio2;
        g_SingeIn.framenum_to_frame   = framenum_to_frame;
        g_SingeIn.get_current_frame   = get_current_frame;
        g_SingeIn.pre_change_speed    = pre_change_speed;
        g_SingeIn.pre_pause           = pre_pause;
        g_SingeIn.pre_play            = pre_play;
        g_SingeIn.pre_search          = pre_search;
        g_SingeIn.pre_skip_backward   = pre_skip_backward;
        g_SingeIn.pre_skip_forward    = pre_skip_forward;
        g_SingeIn.pre_step_backward   = pre_step_backward;
        g_SingeIn.pre_step_forward    = pre_step_forward;
        g_SingeIn.pre_stop            = pre_stop;
        g_SingeIn.set_search_blanking = set_search_blanking;
        g_SingeIn.set_skip_blanking   = set_skip_blanking;
        g_SingeIn.g_local_info        = &g_local_info;
        g_SingeIn.g_vldp_info         = g_vldp_info;
        g_SingeIn.get_video_height    = video::get_video_height;
        g_SingeIn.get_video_width     = video::get_video_width;
        g_SingeIn.draw_string         = video::draw_string;
        g_SingeIn.samples_play_sample = samples::play;
        g_SingeIn.set_last_error      = set_last_error;
        g_SingeIn.get_retro_path      = get_retro_path;

        // by RDG2010
        g_SingeIn.get_status        = get_status;
        g_SingeIn.get_singe_version = get_singe_version;
        g_SingeIn.set_ldp_verbose   = set_ldp_verbose;

        g_SingeIn.samples_set_state = samples::set_state;
        g_SingeIn.samples_is_playing = samples::is_playing;
        g_SingeIn.samples_end_early = samples::end_early;
        g_SingeIn.samples_flush_queue = samples::flush_queue;

        // These functions allow the DLL side of SINGE
        // call the functions set_keyboard_mode and get_keyboard_mode inside
        // this very class.

        // Writing this down to hopefully remind myself in the future.
        // The function on the left side is called from the DLL side.
        // The DLL refers to this function from within. In this case
        // singeproxy.cpp
        // Have a look at the "singe_in_info" struct in singe_interface.h for
        // declaration examples.
        //
        // The function on the right side is the wrapper function.
        // It's the function that does something on the Hypseus side.
        //
        // Have a look at the class declaration in singe.h for details.
        // The two lines below basically link these functions together
        // So when the DLL needs something from Hypseus
        // then the DLL knows which function to call.
        g_SingeIn.cfm_set_keyboard_mode = gfm_set_keyboard_mode;
        g_SingeIn.cfm_get_keyboard_mode = gfm_get_keyboard_mode;

	// Number of attached mice
        g_SingeIn.cfm_get_number_of_mice = gfm_number_of_mice;

        // Extended args
        g_SingeIn.cfm_get_xratio         = gfm_get_xratio;
        g_SingeIn.cfm_get_yratio         = gfm_get_yratio;

        /*
        Why a wrapper?

        Special case. We can't hook up the function on the Hypseus side (CFMs)
        directly to the functions inside the singe class
        because their pointer types don't match.
        To do function callbacks you need to provide a static function.
        The wrapper function (GFMs) solve this problem.

        */

        // Pointer to this very same class
        // used by CFMs as a parameter on the DLL side
        // to gain access to members of the class.
        // Have a look at "sep_set_keyboard_mode(...."
        // in singeproxy.cpp for details
        g_SingeIn.pSingeInstance = this;

        // establish link betwixt singe proxy and us
        g_pSingeOut = pSingeInit(&g_SingeIn);

        // version check
        if (g_pSingeOut->uVersion != SINGE_INTERFACE_API_VERSION) {
            printline("Singe API version mismatch!  Something needs to be "
                      "recompiled...");
            bSuccess = false;
        }
    }

    // if we're not using VLDP, then singe will segfault, so abort ...
    if (g_vldp_info == NULL) {
        printerror("You must use VLDP when using Singe.");
        bSuccess = false;
    }

    return bSuccess;
}

void singe::start()
{
    char s1[100];
    int intTimer = 0;
    int intReturn = 0;
    snprintf(s1, sizeof(s1), "Starting Singe version %.2f", get_singe_version());
    printline(s1);
    g_pSingeOut->sep_set_surface(m_video_overlay_width, m_video_overlay_height);
    g_pSingeOut->sep_set_static_pointers(&m_disc_fps, &m_uDiscFPKS);
    g_pSingeOut->sep_startup(m_strGameScript.c_str());
    bool blanking = g_local_info.blank_during_searches | g_local_info.blank_during_skips;
    int delay = g_ldp->get_min_seek_delay() >> 6;
    g_ldp->set_seek_frames_per_ms(0);
    g_ldp->set_min_seek_delay(0);

    if (upgrade_overlay) g_pSingeOut->sep_upgrade_overlay();
    if (fullsize_overlay) g_pSingeOut->sep_overlay_resize();
    if (muteinit) g_pSingeOut->sep_mute_vldp_init();
    if (notarget) g_pSingeOut->sep_no_crosshair();
    if (singe_oc) g_pSingeOut->sep_alter_lua_clock(singe_ocv);

    // if singe didn't get an error during startup...
    if (!get_quitflag()) {

        while (!get_quitflag()) {
            g_pSingeOut->sep_call_lua("onOverlayUpdate", ">i", &intReturn);
            if (intReturn == 1) {
                m_video_overlay_needs_update = true;
            }

            if (g_singe_bjx||g_singe_bjy) {
                JoystickMotion();
            }

            if (blanking) {
                if (video::get_video_timer_blank()) {
                    if (intTimer > delay)
                        video::set_video_timer_blank(false);
                    intTimer++;
                } else intTimer = 0;
            }

            blit();
            SDL_check_input();
            samples::do_queued_callbacks(); // hack to ensure sound callbacks are
                                            // called at a time when lua can
                                            // accept them without crashing
            g_ldp->think_delay(10);         // don't hog cpu, and advance timer
        }

        g_pSingeOut->sep_call_lua("onShutdown", "");
    } // end if there was no startup error

    // always call sep_shutdown just to make sure everything gets cleaned up
    g_pSingeOut->sep_shutdown();
}

void singe::shutdown() {}

void singe::input_enable(Uint8 input, Sint8 mouseID)
{
    switch (input) {
    case SWITCH_UP:
       g_singe_ypos = -abs(g_singe_js_sen);
       g_singe_jrely--;
       g_singe_bjy = true;
       break;
    case SWITCH_DOWN:
       g_singe_ypos = g_singe_js_sen;
       g_singe_jrely++;
       g_singe_bjy = true;
       break;
    case SWITCH_LEFT:
       g_singe_xpos = -abs(g_singe_js_sen);
       g_singe_jrelx--;
       g_singe_bjx = true;
       break;
    case SWITCH_RIGHT:
       g_singe_xpos = g_singe_js_sen;
       g_singe_jrelx++;
       g_singe_bjx = true;
       break;
    }

    if (g_pSingeOut) // by RDG2010
        g_pSingeOut->sep_call_lua("onInputPressed", "ii", input, mouseID);
}

void singe::input_disable(Uint8 input, Sint8 mouseID)
{
    switch (input) {
    case SWITCH_UP:
    case SWITCH_DOWN:
       g_singe_ypos = 0;
       g_singe_jrely = 0;
       g_singe_bjy = false;
       break;
    case SWITCH_LEFT:
    case SWITCH_RIGHT:
       g_singe_xpos = 0;
       g_singe_jrelx = 0;
       g_singe_bjx = false;
       break;
    }

    if (g_pSingeOut) // by RDG2010
        g_pSingeOut->sep_call_lua("onInputReleased", "ii", input, mouseID);
}

void singe::OnMouseMotion(Uint16 x, Uint16 y, Sint16 xrel, Sint16 yrel, Sint8 mouseID)
{
    if (g_pSingeOut) {
        g_pSingeOut->sep_do_mouse_move(x, y, xrel, yrel, mouseID);
    }
}

void singe::JoystickMotion()
{
    Uint16 cur_w = g_SingeIn.get_video_width();
    Uint16 cur_h = g_SingeIn.get_video_height();
    static bool s = false;

    if (!s) { g_singe_xmov = cur_w/4; g_singe_ymov = cur_h/4; s = true; }

    g_singe_xmov = g_singe_xpos + g_singe_xmov;
    g_singe_ymov = g_singe_ypos + g_singe_ymov;

    if (g_singe_xmov > cur_w) { g_singe_xmov = cur_w; g_singe_jrelx = 0; }
    if (g_singe_ymov > cur_h) { g_singe_ymov = cur_h; g_singe_jrely = 0; }
    if (g_singe_xmov < 0) { g_singe_xmov = abs(g_singe_xmov); g_singe_jrelx = 0; }
    if (g_singe_ymov < 0) { g_singe_ymov = abs(g_singe_ymov); g_singe_jrely = 0; }

    if (g_pSingeOut) {
        g_pSingeOut->sep_do_mouse_move(g_singe_xmov, g_singe_ymov, g_singe_jrelx, g_singe_jrely, NOMOUSE);
    }
}

// game-specific command line arguments handled here
bool singe::handle_cmdline_arg(const char *arg)
{

    bool bResult             = false;
    static bool bInit        = false;
    static bool scriptLoaded = false;
    char s[256]              = {0};
    int i;

    if (!bInit) {
        game::set_32bit_overlay(true);
        upgrade_overlay = bInit = true;
    }

    if (strcasecmp(arg, "-script") == 0) {
        get_next_word(s, sizeof(s));

        if (mpo_file_exists(s)) {
            if (!scriptLoaded) {
                bResult = scriptLoaded = true;
                m_strGameScript = s;
            } else {
                printerror("Only one game script may be loaded at a time!");
                bResult = false;
            }
        } else {
            string strErrMsg = "Script ";
            strErrMsg += s;
            strErrMsg += " does not exist.";
            printerror(strErrMsg.c_str());
        }
    }
    else if (strcasecmp(arg, "-blend_sprites") == 0) {
        video::set_singe_blend_sprite(true);
        bResult = true;
    }
    else if (strcasecmp(arg, "-retropath") == 0) {
        game::set_console_flag(true);
        bResult = true;
    }
    else if (strcasecmp(arg, "-bootsilent") == 0) {
        muteinit = true;
        bResult = true;
    }
    else if (strcasecmp(arg, "-overclock") == 0) {
        singe_oc = singe_ocv = true;
        bResult = true;
    }
    else if (strcasecmp(arg, "-underclock") == 0) {
        singe_oc = true;
        singe_ocv = false;
        bResult = true;
    }
    else if (strcasecmp(arg, "-oversize_overlay") == 0) {

        if (overlay_size) {
            printerror("SINGE: Only one overlay argument allowed");
            return false;
        }
        printline("NOTE : -oversize_overlay deprecated use '-set_overlay oversize'");
        overlay_size = 3;
        bResult = true;
    }
    else if (strcasecmp(arg, "-8bit_overlay") == 0) {
        game::set_32bit_overlay(false);
        upgrade_overlay = false;
        bResult = true;
    }
    else if (strcasecmp(arg, "-set_overlay") == 0) {

        if (overlay_size) {
            printerror("SINGE: Only one overlay argument allowed");
            return false;
        }

        unsigned char r = 0;
        get_next_word(s, sizeof(s));

        if (strcasecmp(s, "half") == 0)     r = 1;
        if (strcasecmp(s, "full") == 0)     r = 2;
        if (strcasecmp(s, "oversize") == 0) r = 3;

        switch(r) {
        case 1:
        case 2:
           overlay_size = r;
           game::set_fullsize_overlay(true);
           fullsize_overlay = true;
           bResult = true;
           break;
        case 3:
           overlay_size = r;
           bResult = true;
           break;
        default:
           printerror("SINGE: -set_overlay expects argument 'full', 'half' or 'oversize'");
           break;
        }
    }
    else if (strcasecmp(arg, "-nocrosshair") == 0) {
        notarget = true;
        bResult = true;
    }
    else if (strcasecmp(arg, "-sinden") == 0) {
        get_next_word(s, sizeof(s));
        i = atoi(s);

        if ((i > 0) && (i < 11)) {
           game::set_sinden_border(i<<1);
           game::set_manymouse(true);
           bResult = true;
        } else {
           printerror("SINGE: border out of scope: <1-10>");
        }

        get_next_word(s, sizeof(s));
        int j = *((int*)(&s));

        if (j != 0x62 && j != 0x67 && j != 0x72 && j != 0x77 && j != 0x78) {
           printerror("SINGE: invalid border color: w, r, g, b or x");
           bResult = false;
        } else {
           game::set_sinden_border_color(j);
        }
    }
    else if (strcasecmp(arg, "-xratio") == 0) {
        get_next_word(s, sizeof(s));
        float f = (float)numstr::ToDouble(s);

        if (f > 0 && f < 100) {
            singe_xratio = (double)floorf(f * 100) / 100;
            bResult = true;
        } else
            printerror("SINGE: ratio should be a float");
    }
    else if (strcasecmp(arg, "-yratio") == 0) {
        get_next_word(s, sizeof(s));
        float f = (float)numstr::ToDouble(s);

        if (f > 0 && f < 100) {
            singe_yratio = (double)floorf(f * 100) / 100;
            bResult = true;
        } else
            printerror("SINGE: ratio should be a float");
    }
    else if (strcasecmp(arg, "-js_range") == 0) {
        get_next_word(s, sizeof(s));
        i = atoi(s);

        if ((i > 0) && (i < 21)) {
           g_singe_js_sen = i;
           bResult = true;
        } else {
           printerror("SINGE: js_range out of scope: <1-20>");
        }
    }
    return bResult;
}

void singe::palette_calculate()
{
    SDL_Color temp_color;

    temp_color.a = 0; // Eliminates a warning.

    // go through all colors and compute the palette
    // (start at 2 because 0 and 1 are a special case)
    for (unsigned int i = 2; i < 256; i++) {
        temp_color.r = i & 0xE0;        // Top 3 bits for red
        temp_color.g = (i << 3) & 0xC0; // Middle 2 bits for green
        temp_color.b = (i << 5) & 0xE0; // Bottom 3 bits for blue
        palette::set_color(i, temp_color);
    }

    // special case: 00 is reserved for transparency, so 01 becomes fully black
    temp_color.r = temp_color.g = temp_color.b = 0;
    palette::set_color(1, temp_color);

    // safety : 00 should never be visible so we'll make it a bright color to
    // help us
    //  catch errors
    temp_color.r = temp_color.g = temp_color.b = 0xFF;
    palette::set_color(0, temp_color);
}

// redraws video
void singe::repaint()
{
    Uint32 cur_w;
    Uint32 cur_h;

    switch(overlay_size) {
    case 2:
       cur_w = g_ldp->get_discvideo_width();
       cur_h = g_ldp->get_discvideo_height();
       break;
    case 3:
       cur_w = SINGE_ABS_OVERLAY_W;
       cur_h = SINGE_ABS_OVERLAY_H;
       break;
    default:
       cur_w = g_ldp->get_discvideo_width()  >> 1;
       cur_h = g_ldp->get_discvideo_height() >> 1;
       break;
    }

    // if the width or height of the mpeg video has changed since we last were
    // here (ie, opening a new mpeg)
    // then reallocate the video overlay buffer
    if ((cur_w != m_video_overlay_width) || (cur_h != m_video_overlay_height)) {
        if (g_ldp->lock_overlay(1000)) {
            m_video_overlay_width  = cur_w;
            m_video_overlay_height = cur_h;

            g_pSingeOut->sep_set_surface(m_video_overlay_width, m_video_overlay_height);

            shutdown_video();
            if (!init_video()) {
                printline(
                    "Fatal Error, trying to re-create the surface failed!");
                game::set_game_errors(SINGE_ERROR_INIT);
                set_quitflag();
            }
            g_ldp->unlock_overlay(1000); // unblock game video overlay
        } else {
            g_pSingeOut->sep_print(
                "Timed out trying to get a lock on the yuv overlay");
            return;
        }
    } // end if dimensions are incorrect

    g_pSingeOut->sep_do_blit(&m_virtual_video_overlay);
}

void singe::set_last_error(const char *cpszErrMsg)
{
    // TODO : figure out reliable way to call printerror (maybe there isn't
    // one?)
    printline(cpszErrMsg);
}

// by RDG2010
void singe::set_keyboard_mode(int thisVal)
{
    if (thisVal != KEYBD_NORMAL && thisVal != KEYBD_FULL) {
        // printline("Singe tried to se an invalid keyboard mode. Defaulting to
        // normal.");
        i_keyboard_mode = KEYBD_NORMAL;
    } else
        i_keyboard_mode = thisVal;
}

double singe::get_xratio() { return singe_xratio; }
double singe::get_yratio() { return singe_yratio; }

int singe::get_keyboard_mode() { return i_keyboard_mode; }

double singe::get_singe_version()
{
    double thisVersion = SINGE_VERSION;
    return thisVersion;
}

// Have SINGE deal directly with SDL input
// This handles when a key is pressed down
void singe::process_keydown(SDL_Keycode key, int keydefs[][2])
{
    /* Normal Hypseus use has the program look for a set of default keys.
     * These are read from daphne.ini (or if daphne.ini is absent, then set
     * a default configuration). The rest of the keyboard is ignored.
     * This is the default mode that works for most gamees.
     *
     * The alternate mode is to scan for all keys.
     * In this mode daphne.ini settings are ignored.
     * The ESCAPE key is hardwired to quit.
     *
     * i_keyboard_mode stores the scanning mode.
     * It is set by default to KEYBD_NORMAL.
     * A singe script can change this to KEYBD_FULL
     * typing "keyboardSetMode" command in the lua
     * scripting side.
     *
     * RDG2010
     *
     * */

    if (i_keyboard_mode == KEYBD_NORMAL) // Using normal keyboard mappings
    { // traverse the keydef array for mapped keys.
        for (Uint8 move = 0; move < SWITCH_COUNT; move++) {
            if ((key == keydefs[move][0]) || (key == keydefs[move][1])) {
                if (move != SWITCH_PAUSE) input_enable(move, NOMOUSE);
            }
        }

    } else { // Using full keyboard access....

        if (key >= SDLK_a && key <= SDLK_z) input_enable(key, NOMOUSE);
        // check to see if key is a number on the top row of the keyboard (not
        // keypad)
        else if (key >= SDLK_MINUS && key <= SDLK_9)
            input_enable(key, NOMOUSE);
        // numeric keypad keys
        else if (key >= SDLK_KP_0 && key <= SDLK_KP_EQUALS)
            input_enable(key, NOMOUSE);
        // arrow keys and insert, delete, home, end, pgup, pgdown
        else if (key >= SDLK_UP && key <= SDLK_PAGEDOWN)
            input_enable(key, NOMOUSE);
        // function keys
        else if (key >= SDLK_F1 && key <= SDLK_F15)
            input_enable(key, NOMOUSE);
        // Key state modifier keys (left and right ctrls, alts)
        else if (key >= SDLK_LCTRL && key <= SDLK_MODE)
            input_enable(key, NOMOUSE);
        else {

            /*
            * SDLK_BACKSPACE, SDLK_TAB, SDLK_RETURN, SDLK_PAUSE,
            * SDLK_SPACE, SDLK_QUOTE, SDLK_COMMA, SDLK_SEMICOLON,
            * SDLK_EQUALS, SDLK_LEFTBRACKET, SDLK_RIGHTBRACKET,
            * SDLK_BACKSLASH, SDLK_SLASH, SDLK_DELETE, SDLK_PERIOD };
            */

            for (int k = 0; k < KEYBD_ARRAY_SIZE; k++) {
                if (key == i_full_keybd_defs[k]) {
                    input_enable(key, NOMOUSE);
                    break;
                } // end if

            } // end for

        } // endif

    } // endif

    // check for ALT-COMMANDS here
    if ((key == SDLK_LALT) || (key == SDLK_RALT)) {
        singe_alt_pressed = true;
    }

    if (singe_alt_pressed) {
        if (key == SDLK_RETURN) video::vid_toggle_fullscreen();
        else if (key == SDLK_BACKSPACE) video::vid_toggle_scanlines();
    }
    // end ALT-COMMAND checks

}

// this is called when the user releases a key
void singe::process_keyup(SDL_Keycode key, int keydefs[][2])
{
    /* Process_keyup is handled differently in SINGE.
     *
     * Array key_defs has the key mappings set in daphne.ini (or defaults).
     *
     * RDG2010
     *
     * */

    if (i_keyboard_mode == KEYBD_NORMAL) // Using normal keyboard mappings
    { // traverse the keydef array for mapped keys.

        // Handle pause and quit keypresses first.
        if (key == keydefs[SWITCH_PAUSE][0] || key == keydefs[SWITCH_PAUSE][1]) {
            toggle_game_pause();
            input_disable(SWITCH_PAUSE, NOMOUSE);

        } else if (key == keydefs[SWITCH_QUIT][0] || key == keydefs[SWITCH_QUIT][1]) {

            set_quitflag();

        } else if (key == keydefs[SWITCH_SCREENSHOT][0]) {

            printline("Screenshot requested!");
            video::set_queue_screenshot(true);

        } else {

            for (Uint8 move = 0; move < SWITCH_COUNT; move++) {
                if ((key == keydefs[move][0]) || (key == keydefs[move][1])) {
                    if (move != SWITCH_PAUSE) input_disable(move, NOMOUSE);
                }

            } // end for

        } // endif

    } else { // Using full keyboard access....

        // Hardwire ESCAPE key to quit
        if (key == SDLK_ESCAPE) set_quitflag();
        // letter keys
        else if (key >= SDLK_a && key <= SDLK_z)
            input_disable(key, NOMOUSE);
        // check to see if key is a number on the top row of the keyboard
        // (not keypad)
        else if (key >= SDLK_MINUS && key <= SDLK_9)
            input_disable(key, NOMOUSE);
        // numeric keypad keys
        else if (key >= SDLK_KP_0 && key <= SDLK_KP_EQUALS)
            input_disable(key, NOMOUSE);
        // arrow keys and insert, delete, home, end, pgup, pgdown
        else if (key >= SDLK_UP && key <= SDLK_PAGEDOWN)
            input_disable(key, NOMOUSE);
        // function keys
        else if (key >= SDLK_F1 && key <= SDLK_F15)
            input_disable(key, NOMOUSE);
        // Key state modifier keys (left and right ctrls, alts)
        else if (key >= SDLK_LCTRL && key <= SDLK_MODE)
            input_disable(key, NOMOUSE);
        else {
            /*
            * SDLK_BACKSPACE, SDLK_TAB, SDLK_RETURN, SDLK_PAUSE,
            * SDLK_SPACE, SDLK_QUOTE, SDLK_COMMA, SDLK_SEMICOLON,
            * SDLK_EQUALS, SDLK_LEFTBRACKET, SDLK_RIGHTBRACKET,
            * SDLK_BACKSLASH, SDLK_SLASH, SDLK_DELETE, SDLK_PERIOD };
            */

            for (int k = 0; k < KEYBD_ARRAY_SIZE; k++) {
                if (key == i_full_keybd_defs[k]) {
                    input_disable(key, NOMOUSE);
                    break;
                } // end if

            } // end for

        } // endif
    }

    // if they are releasing an ALT key
    if ((key == SDLK_LALT) || (key == SDLK_RALT)) {
        singe_alt_pressed = false;
    }

}
