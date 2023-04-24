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

  // Apply another transform on top of self.
  void Update(const Transform& trans);

  // Resets to identity transform.
  void Reset();

  void Map(int *x, int *y) const;
  void Map(int *x, int *y, int *w, int *h) const;
  void Map(SDL_Rect *rect) const {
    Map(&rect->x, &rect->y, &rect->w, &rect->h);
  }

  std::pair<float, float> GetScales() const { return {m_trans_mat(0, 0), m_trans_mat(1, 1)}; }

private:
  boost::numeric::ublas::c_matrix<float, 3, 3> m_trans_mat =
      boost::numeric::ublas::identity_matrix<float>(3);
};
