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

// by Matt Ownby
// Part of the DAPHNE emulator

#ifndef BITMAP_H
#define BITMAP_H

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#define ASPECTSD 133
#define ASPECTWS 178 // Round up
#define NOSQUARE 0x2D0

#include "overlay.h"
#include "SDL_FontCache.h"
#include <SDL.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace video
{
// 16 is normal, the 17th is for the 'A' in SAE
static const uint8_t LED_RANGE          = 17;
static const uint8_t OVERLAY_LED_WIDTH  = 8;
static const uint8_t OVERLAY_LED_HEIGHT = 13;
// width of each LDP1450 overlay character
static const uint8_t OVERLAY_LDP1450_WIDTH = 16;
// height of each LDP1450 overlay character
static const uint8_t OVERLAY_LDP1450_HEIGHT = 16;
// spacing between LDP1450 overlay characters
static const uint8_t OVERLAY_LDP1450_CHARACTER_SPACING = 15;
// spacing between LDP1450 overlay lines
static const uint8_t OVERLAY_LDP1450_LINE_SPACING = 16;
static const uint8_t OVERLAY_LDP1450_LINES = 16;
static const uint8_t ANUN_CHAR_HEIGHT = 15;
static const uint8_t ANUN_LEVELS = 3;

typedef struct LDP1450_CharStruct {
    bool enable = false;
    char* OVERLAY_LDP1450_String;
    float x;
    float y;
} LDP1450_CharStruct;

enum {
    B_DL_PLAYER1,
    B_DL_PLAYER2,
    B_DL_LIVES,
    B_DL_CREDITS,
    B_HYPSEUS_SAVEME,
    B_GAMENOWOOK,
    B_OVERLAY_LEDS,
    B_OVERLAY_LDP1450,
    B_ANUN_OFF,
    B_ANUN_ON,
    B_ACE_SPACE,
    B_ACE_CAPTAIN,
    B_ACE_CADET,
    B_EMPTY
}; // bitmaps

bool init_display();

// MAC: YUV surface protection block: it needs protection because it's accessed from both
// the main "hypseus" thread and the vldp thread.

bool init_display();
bool deinit_display();

SDL_Texture *vid_create_yuv_texture (int width, int height);
void vid_setup_yuv_overlay (int width, int height);
// MAC : REMEMBER, vid_update_yuv_overlay() ONLY updates the YUV surface. The YUV texture is updated on vid_blit()
int vid_update_yuv_overlay (uint8_t *Yplane, uint8_t *Uplane, uint8_t *Vplane, int Ypitch, int Upitch, int Vpitch);
int vid_update_yuv_texture (uint8_t *Yplane, uint8_t *Uplane, uint8_t *Vplane, int Ypitch, int Upitch, int Vpitch);
void vid_blank_yuv_texture (bool value);
void vid_free_yuv_overlay ();

void vid_blit();
// MAC: sdl_video_run thread block ends here

#ifdef USE_OPENGL
bool init_opengl();
#endif // USE_OPENGL

void shutdown_display();
void resize_cleanup();

// flips the video buffers (if in double buffering mode)
void vid_flip();

// blanks the back video buffer (makes it black)
void vid_blank();

void display_repaint();
bool load_bmps();
bool draw_led(int, int, int);
void draw_overlay_leds(const unsigned int *led_values, int num_values, int x, int y, Overlay *overlay);
void draw_singleline_LDP1450(char *LDP1450_String, int start_x, int y);
void draw_charline_LDP1450(char *LDP1450_String, int start_x, int y);
bool draw_othergfx(int which, int x, int y);
void free_bmps();
SDL_Texture *load_one_bmp(const char *);
SDL_Texture *load_one_png(const char *);
void free_one_bmp(SDL_Texture *);
void draw_rectangle(short x, short y, unsigned short w, unsigned short h,
                    unsigned char red, unsigned char green, unsigned char blue);
SDL_Window *get_window();
SDL_Renderer *get_renderer();
SDL_Texture *get_yuv_screen();
Overlay *get_screen_leds();
Overlay *get_score_lives_credit_leds();
FC_Font *get_font();
FC_Font *get_tt_font();
bool get_opengl();
bool get_vulkan();
bool get_fullscreen();
bool get_use_old_osd();
bool get_singe_blend_sprite();
bool get_video_timer_blank();
void set_opengl(bool value);
void set_vulkan(bool value);
int get_textureaccess();

void set_virtual_screen_overlay(::Overlay *overlay);
void set_textureaccess(int value);
void set_grabmouse(bool value);
void set_vsync(bool value);
void set_yuv_blue(bool value);
void set_fullscreen(bool value);
void set_fakefullscreen(bool value);
void set_fullscreen_scale_nearest(bool value);
void set_force_aspect_ratio(bool bEnabled);
void set_ignore_aspect_ratio(bool bEnabled);
void set_scanlines(bool value);
void set_shunt(int value);
void set_alpha(int value);
void set_yuv_video_blank(bool value);
void set_video_timer_blank(bool value);
int get_scalefactor();           // by RDG2010
void set_scalefactor(int value); // by RDG2010
void set_rotate_degrees(float fDegrees);
void set_sboverlay_characterset(int value);
Uint16 get_video_width();
void set_video_width(Uint16);
Uint16 get_video_height();
void set_video_height(Uint16);
void draw_scanlines(int, int, int);
void draw_border(int, int);
void draw_string(const char *, int, int, SDL_Surface *);
void draw_string(const char *, int, int, Overlay *);
void draw_subtitle(char *, bool ins);
void draw_LDP1450_overlay();
void vid_toggle_fullscreen();
void vid_toggle_scanlines();
void vid_scoreboard_switch();
void set_aspect_ratio(int fRatio);
void set_detected_height(int pHeight);
void set_detected_width(int pWidth);
void set_subtitle_enabled(bool bEnabled);
void set_subtitle_display(char *);
void set_LDP1450_enabled(bool bEnabled);
void set_singe_blend_sprite(bool bEnabled);
void set_bezel_file(const char *);
void set_aspect_change(int aspectWidth, int aspectHeight);
void set_sb_window_position(int, int);
void set_annun_bezel_position(int, int);
void set_score_bezel(bool bEnabled);
void set_score_bezel_alpha(bool bEnabled);
void set_score_bezel_scale(int value);
void set_ace_annun_scale(int value);
void set_tq_keyboard(bool bEnabled);
void set_annun_bezel(bool bEnabled);

bool draw_ranks();
bool draw_annunciator(int which);

void take_screenshot();
void set_queue_screenshot(bool bEnabled);

unsigned int get_draw_width();
unsigned int get_draw_height();

int get_yuv_overlay_width();
int get_yuv_overlay_height();

bool get_yuv_overlay_ready();

}
#endif
