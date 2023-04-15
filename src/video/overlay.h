#pragma once

#include "drawlist.h"
#include "transform.h"

class Overlay
{
public:
  void SetSize(int w, int h);

  int Width() const { return width; }
  int Height() const { return height; }

  DrawList& GetDrawList() {
    return drawlist;
  }

  void Render();

private:
  int width = 0;
  int height = 0;
  DrawList drawlist;
  Transform trans;
};
