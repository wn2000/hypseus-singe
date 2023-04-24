#include "config.h"

#include "overlay_scoreboard.h"
#include "../hypseus.h"
#include "../video/video.h"	// for draw_string

#define OVERLAY_LED_WIDTH 8
#define OVERLAY_LED_HEIGHT 13

void OverlayScoreboard::DeleteInstance()
{
	delete this;
}

void OverlayScoreboard::Invalidate()
{
	m_bNeedsRepaint = true;
}

bool OverlayScoreboard::RepaintIfNeeded()
{
	if (get_scoreboard() & 0x03) m_bVisible = false;

    if (!m_bNeedsRepaint) return false;

    m_bNeedsRepaint = false;

    Overlay *leds_overlay = video::get_score_lives_credit_leds();
	leds_overlay->GetDrawList().Clear();

    if (!m_bVisible) {
		return true;
	}

	int w = leds_overlay->Width();

    // for Dragon's Lair/Space Ace
    if (!m_bThayers) {
        // Draw all DL/SA scoreboard labels.
        if (video::get_use_old_osd()) {
            video::draw_string("Credits", w / 12 - (w == 360 ? 4 : 3), 0, leds_overlay);
            video::draw_string("Player 1: ", 1, 0, leds_overlay);
            video::draw_string("Player 2: ", w / 6 - 19, 0, leds_overlay);
            video::draw_string("Lives: ", 1, 14, leds_overlay);
            video::draw_string("Lives: ", w / 6 - 10 + 1, 14, leds_overlay);
        } else {
            video::draw_string("Credits", w / 12 - (w == 360 ? 4 : 3) + 5, 1, leds_overlay);
            video::draw_string("Player 1: ", 2, 1, leds_overlay);
            video::draw_string("Player 2: ", w / 6 - 19 + 7, 1, leds_overlay);
            video::draw_string("Lives: ", 2, 15, leds_overlay);
            video::draw_string("Lives: ", w / 6 - 10 + 10, 15, leds_overlay);
        }

        // Update Player Scores
        update_player_score(leds_overlay, 0, 0, m_DigitValues + this->PLAYER1_0, 6);
        update_player_score(leds_overlay, 1, 0, m_DigitValues + this->PLAYER2_0, 6);

        // Update Player Lives
        update_player_lives(leds_overlay, 0, m_DigitValues[this->LIVES0]);
        update_player_lives(leds_overlay, 1, m_DigitValues[this->LIVES1]);
    }
    // for Thayer's Quest
    else {
        // Thayer's Quest only uses "Credits" portion of the DL/SA //
        // scoreboard.
        if (video::get_use_old_osd())
            video::draw_string("Time", w / 12 - 2, 0, leds_overlay);
        else
            video::draw_string("Time", w / 12 + 3, 1, leds_overlay);
    }

    update_credits(leds_overlay);

    return true;
}

void OverlayScoreboard::update_player_score(Overlay *overlay, int player, int start_digit,
                                            unsigned int values[], int num_digits) const
{
	int x = start_digit * OVERLAY_LED_WIDTH;

	// Player1 position is static, but Player2 will be affected
	// by different MPEG widths (640x480, 720x480 known so far).
	x += player == 0 ? 65 : (overlay->Width() - 7 * OVERLAY_LED_WIDTH);

	video::draw_overlay_leds(values, num_digits, x, 0, overlay);
}

void OverlayScoreboard::update_player_lives(Overlay *overlay, int player, unsigned int lives) const
{
    // Value of lives was validated in caller, so charge right ahead.
    video::draw_overlay_leds(&lives, 1, player == 0 ? 48 : (overlay->Width() - 2 * OVERLAY_LED_WIDTH),
                             OVERLAY_LED_HEIGHT, overlay);
}

void OverlayScoreboard::update_credits(Overlay *overlay) const
{
    int w = overlay->Width();

    // need to shift a bit to look exactly centered
    int fudge = 0;
    if (m_bThayers) {
        fudge = (w == 360 ? 4 : 3);
    } else {
        fudge = (w == 360 ? 2 : 0);
    }

    video::draw_overlay_leds(m_DigitValues + this->CREDITS1_0, 2,
                             w / 2 - (OVERLAY_LED_WIDTH + fudge),
                             OVERLAY_LED_HEIGHT, overlay);
}

bool OverlayScoreboard::ChangeVisibility(bool bVisible)
{
	bool bChanged = false;

	// if the visibility has changed
	if (bVisible != m_bVisible)
	{
		m_bVisible = bVisible;
		m_bNeedsRepaint = true;
		bChanged = true;
	}

	return bChanged;
}

bool OverlayScoreboard::set_digit(unsigned int uValue, WhichDigit which)
{
	return set_digit_w_sae(uValue, which);
}

bool OverlayScoreboard::is_repaint_needed()
{
	return m_bNeedsRepaint;
}

bool OverlayScoreboard::get_digit(unsigned int &uValue, WhichDigit which)
{
	bool bRes = true;
	uValue = m_DigitValues[which];
	return bRes;
}

//

OverlayScoreboard::OverlayScoreboard() :
m_pFuncGetActiveOverlay(NULL),
m_bThayers(false),
m_bVisible(true)
{
}

IScoreboard *OverlayScoreboard::GetInstance(SDL_Surface *(*pFuncGetActiveOverlay)(), bool bThayers)
{
	OverlayScoreboard *pInstance = new OverlayScoreboard();
	pInstance->m_bThayers = bThayers;
	pInstance->m_pFuncGetActiveOverlay = pFuncGetActiveOverlay;
	if (!pInstance->Init())
	{
		pInstance->DeleteInstance();
		pInstance = 0;
	}

	return pInstance;
}
