/*
* ____ DAPHNE COPYRIGHT NOTICE ____
*
* Copyright (C) 2001 Matt Ownby
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

/* DAPHNE : Dragon's Lair / Space Ace Emulator
 * Started by Matt Ownby, many people have contributed since then
 * Begun August 3rd, 1999
 *
 * ORIGINAL GOAL (we've obviously done much more than this now hehe):
 *
 * To emulate the original Dragon's Lair / Space Ace game such that the original
 * ROM's are executed properly, the joystick and buttons are emulated, and the
 * Laserdisc Player is properly controlled.
 * In short, so that the original game may be played with the following
 * requirements:
 * - Original Dragon's Lair Laserdisc
 * - Original Dragon's Lair ROM's (disk files, not the physical ROM's heh)
 * - A laserdisc player
 * - A common modern computer (i386 Win32, i386 Linux most likely)
 * No other original hardware (motherboard, power supply, harness) required!
 */

#include "config.h"

#include <string>
#include "io/my_stdio.h"

using namespace std;

#ifdef MAC_OSX
#include "config.h"
#endif

#include <SDL_main.h>
#include <SDL_image.h>

#ifdef WIN32
// win32 doesn't have regular chdir apparently
#define chdir _chdir
#include <direct.h>
#endif

#ifdef UNIX
#include <unistd.h> // for chdir
#endif

#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <iostream>
#include "io/homedir.h"
#include "io/input.h"
#include "hypseus.h"
#include "timer/timer.h"
#include "sound/sound.h"
#include "io/conout.h"
#include "io/cmdline.h"
#include "io/network.h"
#include "video/video.h"
#include "video/led.h"
#include "ldp-out/ldp.h"
#include "io/error.h"
#include "manymouse/manymouse.h"
#include "cpu/cpu-debug.h"
#include "cpu/cpu.h"
#include "game/game.h"

#include "globals.h"
// some global data is stored in this file

#ifdef DEBUG
#define LOGLEVEL plog::verbose
#else
#define LOGLEVEL plog::info
#endif

// -------------------------------------------------------------------------------------------------

const char *get_hypseus_version() { return hypseus_VERSION; }

unsigned char get_filename(char *s, unsigned char n)
// prepares a filename using any wildcards we may have
// returns filename in s
// returns 1 if this filename contains wild characters
// returns 0 if this filename does not contain any wild characters
{

    unsigned int i       = 0;
    unsigned char result = 0;

    for (i = 0; i < strlen(s); i++) {
        if (s[i] == '+') {
            result = 1;
            s[i]   = (char)(n + 0x30); // convert wildcard to number
        }
    }

    return (result);
}

// sets the quit flag
void set_quitflag() { quitflag = 1; }

// returns the quit flag
unsigned char get_quitflag() { return (quitflag); }

bool change_dir(const char *cpszNewDir)
{
    int i = chdir(cpszNewDir);
    return (i == 0);
}

// sets current directory based on the full path of the executable
// This solves the problem of someone running hypseus without first being in the
// hypseus directory
void set_cur_dir(const char *exe_loc)
{
    int index   = strlen(exe_loc) - 1; // start on last character
    string path = "";

    // locate the preceeding / or \ character
    while ((index >= 0) && (exe_loc[index] != '/') && (exe_loc[index] != '\\')) {
        index--;
    }

    // if we found a leading / or \ character
    if (index >= 0) {
        path = exe_loc;
        path.erase(index); // erase from here to the end
        change_dir(path.c_str());
    }
}

/////////////////////// MAIN /////////////////////

// the main function for both Windows and Linux <grin>
int main(int argc, char **argv)
{
    int result_code = 1; // assume an error unless we find otherwise
    int imgflags = IMG_INIT_PNG | IMG_INIT_JPG;

    set_cur_dir(argv[0]); // set active directory

    // initialize SDL without any subsystems but with the no parachute option so
    // 1 - we can initialize either audio or video first
    // 2 - we can trace segfaults using a debugger
    if (SDL_Init(SDL_INIT_NOPARACHUTE) < 0) {
        printerror("Could not initialize SDL!");
        exit(1);
    }

    if (IMG_Init(imgflags) != imgflags) {
        printerror("Could not initialize SDL IMG!");
        SDL_Quit();
        exit(1);
    }

    if (TTF_Init() != 0) {
        printerror("Could not initialize SDL TTF!");
        IMG_Quit();
        SDL_Quit();
        exit(1);
    }

    // parse the command line (which allocates game and ldp) and continue if no
    // errors this is important!  if game_type or ldp_type fails to allocate
    // g_game and g_ldp, then the program will segfault and hypseus must NEVER
    // segfault!  hehe
    if (!parse_cmd_line(argc, argv)) {
        printerror("Bad command line or initialization problem.");
        goto cleanup;
    }

    // MATT : we have to wait until after the command line is parsed before
    // we do anything with the LEDs in case the
    // user does not enable them
    remember_leds();                 // memorizes the status of keyboard leds
    change_led(false, false, false); // turns all keyboard leds off

    // MAC : we also have to wait after the command line is parsed before
    // knowing if we want loging or not.
    if (!log_was_disabled) reset_logfile(argc, argv);

    // check if the display initialized properly
    if (!video::load_bmps()) {
        printerror("Video initialization failed!");
        goto cleanup;
    }

    if (!sound::init()) {
        printerror("Sound initialization failed!");
        goto cleanup;
    }

    if (!SDL_input_init()) {
        printerror("Could not initialize input!");
        sound::shutdown();
        goto cleanup;
    }

    // check if the roms were loaded successfully
    if (!g_game->load_roms()) {
        printerror("Could not load ROM images! You must supply these.");
        SDL_input_shutdown();
        sound::shutdown();
        goto cleanup;
    }

    // check if the video was initialized successfully
    if (!g_game->init_video()) {
        printerror("Game-specific video initialization failed!");
        SDL_input_shutdown();
        sound::shutdown();
        goto cleanup;
    }

    // if the game has some problems, notify the user
    // before the user plays the game
    if (g_game->get_issues()) { printnowookin(g_game->get_issues()); }

    // delay for a bit before the LDP is intialized to
    // make sure all video is done getting drawn before
    // VLDP is initialized
    SDL_Delay(1000);

    // check if the laserdisc player was initialized properly
    if (!g_ldp->pre_init()) {
        printerror("Could not initialize laserdisc player!");
        g_game->shutdown_video();
        SDL_input_shutdown();
        sound::shutdown();
        goto cleanup;
    }

    if (g_game->pre_init()) // initialize all cpu's
    {
        LOGD << "Booting ROM ...";
        g_game->start(); // HERE IS THE MAIN LOOP
                         // RIGHT HERE
        g_game->pre_shutdown();

        // Send our game/ldp type to server to
        // create stats.
        // This was moved to after the game has
        // finished running because it creates a
        // slight delay (like 1 second), which
        // throws off the think_delay function in
        // the LDP class.

        // MAC: Commented to speed up testing
        // without internet connection.
        // net_send_data_to_server();

        result_code = g_game->get_game_errors();
        // hypseus will exit with
        // error codes
    }

    // exit if returns an error but don't print
    // error message to avoid repetition

    g_ldp->pre_shutdown();
    g_game->shutdown_video();
    SDL_input_shutdown();
    sound::shutdown();

    // MAC : DON'T do this here, it's too soon
    // to call SDL_Quit(VIDEO), we don't want segfaults on exit.
    // video::shutdown_display();

cleanup:
    // if our g_game class was allocated
    if (g_game) {
        if (g_game->get_manymouse()) ManyMouse_Quit();
        delete (g_game);
    }

    // if g_ldp was allocated, de-allocate it
    if (g_ldp) {
        delete (g_ldp);
    }

    video::free_bmps(); // always do this no matter what
    video::deinit_display();
    
    video::shutdown_display(); // shut down the display. MAC: DON'T do this (calls SDL_Quit(VIDEO)!!)
                               // until you have ended the sdl_video_run thread AND freed renderer, etc.

    restore_leds(); // sets keyboard leds back how they were (this is safe even
                    // if we have the led's disabled)

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    exit(result_code);
}

void set_search_offset(int i) { search_offset = i; }

int get_search_offset() { return (search_offset); }

unsigned char get_frame_modifier() { return (frame_modifier); }

void set_frame_modifier(unsigned char value) { frame_modifier = value; }

void set_scoreboard(unsigned char value) { realscoreboard = value; }

void set_scoreboard_usb_port(unsigned char value) { scoreboard_usb_port = value; }
void set_scoreboard_usb_impl(unsigned char value) { scoreboard_usb_impl = value; }
void set_scoreboard_usb_baud(unsigned int value) { scoreboard_usb_baud = value; }

unsigned char get_scoreboard() { return (realscoreboard); }

void set_scoreboard_port(unsigned int value) { rsb_port = value; }

unsigned int get_scoreboard_port() { return (rsb_port); }

void reset_logfile(int argc, char **argv)
{
    int i = 0;
    string str;
    char logname[64];

    snprintf(logname, sizeof(logname), "logs%shypseus.log", PATH_SEPARATOR);
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(logname, 50000, 3);
    plog::init(LOGLEVEL, &consoleAppender).addAppender(&fileAppender);

    LOGI << "Version " << get_hypseus_version();
    for (i = 0; i < argc; i++) {
        str = str + argv[i] + " ";
    }
    LOGI << "Command line: " << str;
    LOGI << "CPU : " << get_cpu_name() << " || Mem : " << get_sys_mem() << " megs";
    LOGI << "OS : " << get_os_description() << " || Video : " << get_video_description();
    LOGI << "RGB2YUV Function: "
#ifdef USE_MMX
    << "MMX";
#else
    << "C";
#endif
    LOGI << "Line Blending Function: "
#ifdef USE_MMX
    << "MMX";
#else
    << "C";
#endif // blend MMX

    LOGI << "Audio Mixing Function: "
#ifdef USE_MMX
    << "MMX";
#else
    << "C";
#endif // blend MMX
}

// added by JFA for -idleexit
void set_idleexit(unsigned int value) { idleexit = value; }

unsigned int get_idleexit() { return (idleexit); }
// end edit

// added by JFA for -startsilent
void set_startsilent(unsigned char value) { startsilent = value; }

unsigned char get_usb_port() { return (scoreboard_usb_port); }
unsigned char get_usb_impl() { return (scoreboard_usb_impl); }
unsigned int get_usb_baud() { return (scoreboard_usb_baud); }

unsigned char get_startsilent() { return (startsilent); }
// end edit

// added by MAC for -nolog
void set_log_was_disabled (bool value) { log_was_disabled = value; }
// end edit
