/*
 * ____ DAPHNE COPYRIGHT NOTICE ____
 *
 * Copyright (C) 2001 Mark Broadhead
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

// firefox.h
// by Mark Broadhead

#include "game.h"

#define FIREFOX_OVERLAY_W 512 // width of overlay
#define FIREFOX_OVERLAY_H 512 // height of overlay
#define FIREFOX_COLORS 256

class firefox : public game
{
  public:
    firefox();
    bool init();
    void do_nmi();                                // does an NMI tick
    void do_irq(unsigned int);                    // does an IRQ tick
    void do_firq();                               // does a FIRQ tick
    Uint8 cpu_mem_read(Uint16 addr);              // memory read routine
    void cpu_mem_write(Uint16 addr, Uint8 value); // memory write routine
    void input_enable(Uint8, Sint8);
    void input_disable(Uint8, Sint8);
    void OnVblank();
    bool set_bank(unsigned char, unsigned char);
    void palette_calculate();
    void repaint(); // function to repaint video

  protected:
    int ad_converter_channel;
    bool palette_modified;
    void recalc_palette(void);
    int current_bank;
    void display_update();
    std::array<Uint8, 0x2000> character{};
    std::array<Uint8, 0x10000> rombank{};

    // firefox's banks
    // bank 0 is RDIN0
    // bank 1 is RDIN1
    // bank 2 is dip switch 1
    // bank 3 is dip switch 2
    // bank 4 is a/d channel 0
    // bank 5 is a/d channel 1
    std::array<Uint8, 6> banks{0xFF, 0xFF, 0xFF, 0xFF};

    // buffer that holds byte to be sent to LDP (8-bits)
    Uint8 m_u8DskLatch;

    // true = FIRQ is enabled
    bool m_bFIRQLatch;

    // true = IRQ is enabled
    bool m_bIRQLatch;
};

class firefoxa : public firefox
{
  public:
    firefoxa();
    //	bool load_roms();
};
