#pragma once

#include <SDL.h>

// Maps logical rects to canvas rects.
class Transform
{
public:
  void SetLayout(const SDL_Rect& canvas_rect, const SDL_Rect& logical_rect);
  void Map(int *x, int *y) const;
  void Map(int *x, int *y, int *w, int *h) const;
  void Map(SDL_Rect *rect) const {
    Map(&rect->x, &rect->y, &rect->w, &rect->h);
  }

private:
  int canvas_center_x = 0;
  int canvas_center_y = 0;
  int logical_center_x = 0;
  int logical_center_y = 0;
  float scale = 1;
};
