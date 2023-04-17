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
}

void Overlay::SetPosition(const SDL_Rect& dest)
{
    m_trans.Reset();
    m_trans.Update(width, height, dest);
}

void Overlay::Render()
{
    drawlist.Render(m_trans);
}

void Overlay::Render(const Transform& trans)
{
    Transform tmpTrans = m_trans;
    tmpTrans.Update(trans);
    drawlist.Render(tmpTrans);
}
