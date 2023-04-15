#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "../my_unique_ptr.h"
#include "transform.h"

class DrawList
{
public:
  void Clear();

  void Image(void *img, int x, int y, int w, int h);
  void Image(void *img, const SDL_Rect& rect)
  {
      Image(img, rect.x, rect.y, rect.w, rect.h);
  }

  void Image(void *img, int src_x, int src_y, int src_w, int src_h, int dest_x, int dest_y, int dest_w, int dest_h);
  void Image(void *img, const SDL_Rect& src, const SDL_Rect& dest)
  {
    Image(img, src.x, src.y, src.w, src.h, dest.x, dest.y, dest.w, dest.h);
  }

  void Image(SDL_Texture_Ptr img, int x, int y, int w, int h);
  void Image(SDL_Texture_Ptr img, const SDL_Rect& rect)
  {
      Image(std::move(img), rect.x, rect.y, rect.w, rect.h);
  }

  void Text(std::string_view text, int x, int y);
  void FillRect(int x, int y, int w, int h, uint32_t color);
  void FillRect(const SDL_Rect& rect, uint32_t color)
  {
    FillRect(rect.x, rect.y, rect.w, rect.h, color);
  }

  void Render();
  void Render(const Transform& trans);

private:
  enum class DrawOperation {
      IMAGE,
      IMAGE_ONE_TIME,
      TEXT,
      RECTANGLE,
  };

  struct ImageData {
    void *texture;
    int dest_x, dest_y, dest_w, dest_h;
    int src_x, src_y, src_w, src_h;
  };

  struct ImageOneTimeData {
    int dest_x, dest_y, dest_w, dest_h;
    int src_x, src_y, src_w, src_h;
  };

  struct TextData {
    int x, y;
  };

  struct FillRectData {
    int x, y, w, h;
    uint32_t color;
  };

  struct DrawItem {
    DrawOperation operation;

    union {
        ImageData image;
        ImageOneTimeData image_one_time;
        TextData text;
        FillRectData fillrect;
    };

    std::string str;
    SDL_Texture_Ptr texture;
  };

  std::vector<DrawItem> drawlist;
};
