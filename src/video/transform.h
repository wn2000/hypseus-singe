#pragma once

#include <SDL.h>
#include <boost/numeric/ublas/matrix.hpp>

// Maps logical rects to canvas rects.
class Transform
{
public:
  // Adds a new transformation on top of the current one.
  // The new transformation scales a w x h rect (positioned at (0, 0)) to fill
  // the dest rect (centered with letterbox padding).
  void Update(int src_w, int src_h, const SDL_Rect& dest);

  void Map(int *x, int *y) const;
  void Map(int *x, int *y, int *w, int *h) const;
  void Map(SDL_Rect *rect) const {
    Map(&rect->x, &rect->y, &rect->w, &rect->h);
  }

private:
  boost::numeric::ublas::c_matrix<float, 3, 3> m_trans_mat =
      boost::numeric::ublas::identity_matrix<float>(3);
};
