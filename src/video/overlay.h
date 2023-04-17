#pragma once

#include "drawlist.h"
#include "transform.h"

class Overlay
{
public:
  void SetSize(int w, int h);
  // Position this overlay in its container, which can be another overlay, or
  // the screen.
  void SetPosition(const SDL_Rect& dest);

  int Width() const { return width; }
  int Height() const { return height; }

  DrawList& GetDrawList() {
    return drawlist;
  }

  void Render();
  void Render(const Transform& trans);

private:
  int width = 0;
  int height = 0;
  DrawList drawlist;
  Transform m_trans;
};
