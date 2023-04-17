#include "transform.h"
#include <boost/numeric/ublas/assignment.hpp>

namespace ublas = boost::numeric::ublas;

void Transform::Update(int src_w, int src_h, const SDL_Rect& dest)
{
    float dest_aspect = (float)dest.w / (float)dest.h;
    float src_aspect = (float)src_w / (float)src_h;
    float scale = src_aspect > dest_aspect ? (float)dest.w / (float)src_w
                                           : (float)dest.h / (float)src_h;

    ublas::c_matrix<float, 3, 3> translateSrc;
    translateSrc <<= 1, 0, - src_w / 2,
                     0, 1, - src_h / 2,
                     0, 0, 1;
    
    ublas::c_matrix<float, 3, 3> scaling;
    scaling <<= scale, 0, 0,
                0, scale, 0,
                0, 0, 1;
    
    ublas::c_matrix<float, 3, 3> translateDest;
    translateDest <<= 1, 0, dest.x + dest.w / 2,
                      0, 1, dest.y + dest.h / 2,
                      0, 0, 1;

    m_trans_mat = ublas::prod(translateSrc, m_trans_mat);
    m_trans_mat = ublas::prod(scaling, m_trans_mat);
    m_trans_mat = ublas::prod(translateDest, m_trans_mat);
}

void Transform::Map(int *x, int *y) const
{
    ublas::c_vector<int, 3> v;
    v <<= *x, *y, 1;

    v = ublas::prod(m_trans_mat, v);

    *x = v[0];
    *y = v[1];
}

void Transform::Map(int *x, int *y, int *w, int *h) const
{
    assert(m_trans_mat(0, 1) == 0 && m_trans_mat(1, 0) == 0);

    Map(x, y);
    *w *= m_trans_mat(0, 0);
    *h *= m_trans_mat(1, 1);
}
