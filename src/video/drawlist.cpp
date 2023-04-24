#include "drawlist.h"
#include "video.h"

void DrawList::Clear()
{
    drawlist.clear();
}

void DrawList::Image(SDL_Texture *texture, int x, int y, int w, int h)
{
    drawlist.push_back({
        .operation = DrawOperation::IMAGE,
        .image = {
                texture,
                x, y, w, h,
                -1,
        },
    });
}

void DrawList::Image(SDL_Texture *texture, int dest_x, int dest_y, int dest_w, int dest_h,
                     int src_x, int src_y, int src_w, int src_h)
{
    drawlist.push_back({.operation = DrawOperation::IMAGE,
                        .image     = {
                            texture,
                            dest_x,
                            dest_y,
                            dest_w,
                            dest_h,
                            src_x,
                            src_y,
                            src_w,
                            src_h,
                        }});
}

void DrawList::Image(SDL_Texture_Ptr texture, int x, int y, int w, int h)
{
    drawlist.push_back({
        .operation = DrawOperation::IMAGE_ONE_TIME,
        .image_one_time = {
                x, y, w, h,
                -1,
        },
        .texture = std::move(texture),
    });
}

void DrawList::Image(Overlay *overlay, const SDL_Rect& dest)
{
    overlay->SetPosition(dest);

    drawlist.push_back({
        .operation = DrawOperation::OVERLAY,
        .overlay = {overlay},
    });
}

void DrawList::Text(std::string_view text, int x, int y)
{
    drawlist.push_back({
        .operation = DrawOperation::TEXT,
        .text = {
            x, y,
        },
        .str = std::string(text),
    });
}

void DrawList::FillRect(int x, int y, int w, int h, uint32_t color)
{
    drawlist.push_back({
        .operation = DrawOperation::RECTANGLE,
        .fillrect = {
            x, y, w, h,
            color,
        }
    });
}

void DrawList::Render() const
{
    for (const DrawItem& item: drawlist)
    {
        if (item.operation == DrawOperation::IMAGE)
        {
            SDL_Rect dest{item.image.dest_x, item.image.dest_y,
                          item.image.dest_w, item.image.dest_h};
            if (item.image.src_x < 0)
            {
                SDL_RenderCopy(video::get_renderer(), (SDL_Texture *)item.image.texture, nullptr, &dest);
            }
            else
            {
                SDL_Rect src{item.image.src_x, item.image.src_y,
                             item.image.src_w, item.image.src_h};
                SDL_RenderCopy(video::get_renderer(), (SDL_Texture *)item.image.texture, &src, &dest);
            }
        }
        else if (item.operation == DrawOperation::IMAGE_ONE_TIME)
        {
            SDL_Rect dest{item.image_one_time.dest_x, item.image_one_time.dest_y,
                          item.image_one_time.dest_w, item.image_one_time.dest_h};
            if (item.image_one_time.src_x < 0)
            {
                SDL_RenderCopy(video::get_renderer(), item.texture.get(), nullptr, &dest);
            }
            else
            {
                SDL_Rect src{item.image_one_time.src_x, item.image_one_time.src_y,
                             item.image_one_time.src_w, item.image_one_time.src_h};
                SDL_RenderCopy(video::get_renderer(), item.texture.get(), &src, &dest);
            }
        }
        else if (item.operation == DrawOperation::TEXT)
        {
            FC_Draw(video::get_tt_font(), video::get_renderer(), item.text.x, item.text.y, item.str.c_str());
        }
        else if (item.operation == DrawOperation::RECTANGLE)
        {
            SDL_Rect dest{item.fillrect.x, item.fillrect.y, item.fillrect.w, item.fillrect.h};
            SDL_SetRenderDrawColor(video::get_renderer(), item.fillrect.color & 0xFF,
                                   (item.fillrect.color >> 8) & 0xFF,
                                   (item.fillrect.color >> 16) & 0xFF,
                                   (item.fillrect.color >> 24) & 0xFF);
            SDL_RenderFillRect(video::get_renderer(), &dest);
        }
        else if (item.operation == DrawOperation::OVERLAY)
        {
            item.overlay.overlay->Render();
        }
    }
}

void DrawList::Render(const Transform& trans) const
{
    for (const DrawItem& item: drawlist)
    {
        if (item.operation == DrawOperation::IMAGE)
        {
            SDL_Rect dest{item.image.dest_x, item.image.dest_y,
                          item.image.dest_w, item.image.dest_h};
            trans.Map(&dest);

            if (item.image.src_x < 0)
            {
                SDL_RenderCopy(video::get_renderer(), (SDL_Texture *)item.image.texture, nullptr, &dest);
            }
            else
            {
                SDL_Rect src{item.image.src_x, item.image.src_y,
                             item.image.src_w, item.image.src_h};
                // trans.Map(&src);

                SDL_RenderCopy(video::get_renderer(), (SDL_Texture *)item.image.texture, &src, &dest);
            }
        }
        else if (item.operation == DrawOperation::IMAGE_ONE_TIME)
        {
            SDL_Rect dest{item.image_one_time.dest_x, item.image_one_time.dest_y,
                          item.image_one_time.dest_w, item.image_one_time.dest_h};
            trans.Map(&dest);

            if (item.image_one_time.src_x < 0)
            {
                SDL_RenderCopy(video::get_renderer(), item.texture.get(), nullptr, &dest);
            }
            else
            {
                SDL_Rect src{item.image_one_time.src_x, item.image_one_time.src_y,
                             item.image_one_time.src_w, item.image_one_time.src_h};
                // trans.Map(&src);

                SDL_RenderCopy(video::get_renderer(), item.texture.get(), &src, &dest);
            }
        }
        else if (item.operation == DrawOperation::TEXT)
        {
            int x = item.text.x;
            int y = item.text.y;
            trans.Map(&x, &y);
            auto [scalex, scaley] = trans.GetScales();

            FC_DrawScale(video::get_tt_font(), video::get_renderer(), x, y, {scalex, scaley}, item.str.c_str());
        }
        else if (item.operation == DrawOperation::RECTANGLE)
        {
            SDL_Rect dest{item.fillrect.x, item.fillrect.y, item.fillrect.w, item.fillrect.h};
            trans.Map(&dest);

            SDL_SetRenderDrawColor(video::get_renderer(), item.fillrect.color & 0xFF,
                                   (item.fillrect.color >> 8) & 0xFF,
                                   (item.fillrect.color >> 16) & 0xFF,
                                   (item.fillrect.color >> 24) & 0xFF);
            SDL_RenderFillRect(video::get_renderer(), &dest);
        }
        else if (item.operation == DrawOperation::OVERLAY)
        {
            item.overlay.overlay->Render(trans);
        }
    }
}
