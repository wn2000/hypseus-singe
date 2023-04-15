#include "transform.h"

void Transform::SetLayout(const SDL_Rect& canvas_rect, const SDL_Rect& logical_rect)
{
    canvas_center_x = canvas_rect.x + canvas_rect.w / 2;
    canvas_center_y = canvas_rect.y + canvas_rect.h / 2;
    logical_center_x = logical_rect.x + logical_rect.w / 2;
    logical_center_y = logical_rect.y + logical_rect.h / 2;

    float canvas_aspect = (float)canvas_rect.w / (float)canvas_rect.h;
    float logical_aspect = (float)logical_rect.w / (float)logical_rect.h;

    if (logical_aspect > canvas_aspect)
    {
        // letterbox in upper/lower portion
        scale = (float)canvas_rect.w / (float)logical_rect.w;
    }
    else
    {
        // letterbox in left/right portion
        scale = (float)canvas_rect.h / (float)logical_rect.h;
    }
}

void Transform::Map(int *x, int *y) const
{
    *x = (*x - logical_center_x) * scale + canvas_center_x;
    *y = (*y - logical_center_y) * scale + canvas_center_y;
}

void Transform::Map(int *x, int *y, int *w, int *h) const
{
    *x = (*x - logical_center_x) * scale + canvas_center_x;
    *y = (*y - logical_center_y) * scale + canvas_center_y;
    *w *= scale;
    *h *= scale;
}
