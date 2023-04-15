#include "overlay.h"
#include "video.h"
#include <SDL.h>

void Overlay::SetSize(int w, int h)
{
    // width = w;
    // height = h;

    // hack for legacy
    width = 320;
    height = 240;

    int canvas_w = 0;
    int canvas_h = 0;
    SDL_RenderGetLogicalSize(video::get_renderer(), &canvas_w, &canvas_h);
    if (!canvas_w || !canvas_h)
    {
        SDL_GetRendererOutputSize(video::get_renderer(), &canvas_w, &canvas_h);
    }

    trans.SetLayout({0, 0, canvas_w, canvas_h},
                    {0, 0, width, height});
}

void Overlay::Render()
{
    drawlist.Render(trans);
}
