#pragma once

#include "drawlist.h"
#include "transform.h"

// An overlay is a rectangular canvas that can contain various graphical
// elements, such as images, text, rectangles, and other overlays. When an
// element is added to an overlay, it is not rendered immediately. Instead, all
// drawing operations are cached until the `Render()` method is called. At that
// point, each individual element is rendered directly to the screen. An overlay
// can be positioned and scaled into a container, which can be another overlay
// or the screen. The overlay's `Render()` method applies a built-in
// transformation to each member element, so that they are properly positioned
// in the container.
class Overlay
{
public:
  // Set the logical size of this overlay.
  // All member elements are positioned relative to the (0, 0, w, h) rect.
  void SetSize(int w, int h);

  // Position this overlay in its container, which can be another overlay, or
  // the screen.
  void SetPosition(const SDL_Rect& dest);

  int Width() const { return width; }
  int Height() const { return height; }

  DrawList& GetDrawList() {
    return drawlist;
  }

  // Use this when container is the screen.
  void Render();

  // Apply an additional transformation on top of the built-in one when
  // rendering. Use this when the container is nested inside another container.
  // Typically called by the container's `Render()` method.
  void Render(const Transform& trans);

private:
  int width = 0;
  int height = 0;
  DrawList drawlist;
  Transform m_trans;
};
