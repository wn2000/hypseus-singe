// From https://stackoverflow.com/a/51274008

#pragma once

#include <memory>
#include <SDL.h>

template <auto fn>
struct deleter_from_fn
{
    template <typename T>
    constexpr void operator()(T* arg) const {
         fn(arg);
    }
};

template <typename T, auto fn>
using my_unique_ptr = std::unique_ptr<T, deleter_from_fn<fn>>;

typedef my_unique_ptr<SDL_Texture, SDL_DestroyTexture> SDL_Texture_Ptr;
typedef my_unique_ptr<SDL_Surface, SDL_FreeSurface> SDL_Surface_Ptr;
