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

// esh.h
// by Matt Ownby

#include "game.h"

#define ESH_OVERLAY_W 256 // width of overlay
#define ESH_OVERLAY_H 256 // height of overlay

// # of colors in the esh's color palette
#define ESH_COLOR_COUNT 256

#define ESH_GAMMA 4.0

class esh : public game
{
  public:
    esh();
    void do_nmi();                                // does an NMI tick
    void do_irq(unsigned int);                    // does an IRQ tick
    void cpu_mem_write(Uint16 addr, Uint8 value); // memory write routine
    Uint8 port_read(Uint16 port);
    void port_write(Uint16 port, Uint8 value);
    void input_enable(Uint8, Sint8);
    void input_disable(Uint8, Sint8);
    void palette_calculate();
    void repaint(); // function to repaint video
    void patch_roms();
    void set_version(int);

  protected:
    std::array<Uint8, 0x8000> character{};
    std::array<Uint8, 0x200> miscprom{};
    std::array<Uint8, 0x200> color_prom{};

  private:
    bool m_needlineblink, m_needcharblink;
    int blank_count;
    Uint8 palette_high_bit;

    // esh's banks
    // bank 1 is switches
    // bank 2 is switches
    // bank 3 is dip switch 1
    // bank 4 is dip switch 2
    std::array<Uint8, 4> banks{0xFF, 0xFF, 0xFF, 0xFF};
};
