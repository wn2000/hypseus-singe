/*
 * ____ DAPHNE COPYRIGHT NOTICE ____
 *
 * Copyright (C) 2003 Warren Ondras
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

// cobraconv.h
// by Warren Ondras, based on bega.h by Mark Broadhead

#include "game.h"

#define COBRACONV_OVERLAY_W 256 // width of overlay
#define COBRACONV_OVERLAY_H 256 // height of overlay

#define COBRACONV_COLOR_COUNT 32 // # of colors in palette
#define COBRACONV_GAMMA 1.0      // brightness adjustment

class cobraconv : public game
{
  public:
    cobraconv();
    void do_nmi();                                // does an NMI tick
    void do_irq(unsigned int);                    // does an IRQ tick
    Uint8 cpu_mem_read(Uint16 addr);              // memory read routine
    void cpu_mem_write(Uint16 addr, Uint8 value); // memory write routine
    void input_enable(Uint8, Sint8);
    void input_disable(Uint8, Sint8);
    void OnVblank();
    void OnLDV1000LineChange(bool bIsStatus, bool bIsEnabled);
    void palette_calculate();
    void repaint(); // function to repaint video
    bool set_bank(unsigned char, unsigned char);

  protected:
    Uint8 m_sounddata_latch;
    Uint8 m_soundchip_id;
    Uint8 m_soundchip_address_latch;
    std::array<Uint8, 0x10000> m_cpumem2{}; // 64k of space for the sound cpu
    void draw_8x8(int, Uint8 *, int, int, int, int, int);
    void draw_16x32(int, Uint8 *, int, int, int, int, int);
    void draw_sprites(int, Uint8 *);
    Uint8 ldp_status;
    std::array<Uint8, 0x6000> character1{};
    std::array<Uint8, 0x6000> character2{};
    std::array<Uint8, 0x8000> character{};
    std::array<Uint8, 0x200> color_prom{};
    std::array<Uint8, 0x400> miscprom{}; // stores unused proms, to make sure no one strips
                                         // them out

    bool palette_updated; // whether our color ram has been written to

    // switch banks
    // bank 0 is switches
    // bank 1 is dip switch 1
    // bank 2 is dip switch 2
    // bank 3 is vblank/ld/coin/tilt
    std::array<Uint8, 4> banks{0XFF, 0xFF, 0xFF, 0xFF};
};
