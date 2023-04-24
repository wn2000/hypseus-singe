#ifndef OVERLAY_SCOREBOARD_H
#define OVERLAY_SCOREBOARD_H

#include "scoreboard_interface.h"
#include "../video/overlay.h"

class OverlayScoreboard : public IScoreboard
{
	friend class ScoreboardFactory;
public:
	void DeleteInstance();

	void Invalidate();

	bool RepaintIfNeeded();

	bool ChangeVisibility(bool bVisible);

	bool set_digit(unsigned int uValue, WhichDigit which);

	bool is_repaint_needed();

	bool get_digit(unsigned int &uValue, WhichDigit which);

private:
	OverlayScoreboard();

	static IScoreboard *GetInstance(SDL_Surface *(*pFuncGetActiveOverlay)(), bool bThayers);

    void update_player_score(Overlay *overlay, int player, int start_digit,
                             unsigned int values[], int num_digits) const;
    void update_player_lives(Overlay *overlay, int player, unsigned int lives) const;
    void update_credits(Overlay *overlay) const;

	// callback function to get pointer to active video overlay surface
	SDL_Surface *(*m_pFuncGetActiveOverlay)();

	// is the current game Thayer's Quest? if so, we need to draw the scoreboard differently
	bool m_bThayers;

	// whether this overlay is visible
	bool m_bVisible;

};

#endif // OVERLAY_SCOREBOARD_H
