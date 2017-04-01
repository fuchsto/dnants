#ifndef GOS__WINDOW_H__INCLUDED
#define GOS__WINDOW_H__INCLUDED

#include <gos/types.h>
#include <gos/util/logging.h>
#include <gos/view/utils.h>

#include <SDL.h>

#include <stdexcept>


namespace gos {

class app_engine;


namespace view {

class window {

  gos::app_engine & _app;

  SDL_Window      * _window;
  SDL_Renderer    * _renderer;

 public:
  window(
    gos::app_engine & app,
    std::string       title,
    int               width,
    int               height,
    int               bpp = 0)
  : _app(app)
  {
    GOS__LOG_DEBUG("view::window", "initialize window ...");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Couldn't initialize SDL: %s",
                   SDL_GetError());
      throw std::runtime_error("SDL init failed");
    }
    GOS__LOG_DEBUG("view::window", "SDL initialized ...");
    if (SDL_CreateWindowAndRenderer(
          width,
          height,
          SDL_WINDOW_SHOWN, // | SDL_RENDERER_ACCELERATED,
          &_window,
          &_renderer) != 0) {
      SDL_LogError(
        SDL_LOG_CATEGORY_APPLICATION,
        "Couldn't create window and renderer: %s", SDL_GetError());
      throw std::runtime_error("create window failed");
    }
    GOS__LOG_DEBUG("view::window", "set window position ...");
    SDL_SetWindowPosition(_window, 100, 100);
    SDL_RaiseWindow(_window);
  }

  ~window() {
    GOS__LOG_DEBUG("view::window", "destruct");
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
  }

  SDL_Renderer * renderer() {
    return _renderer;
  }

  extents view_extents() const {
    extents ext;
    SDL_GetRendererOutputSize(_renderer, &ext.w, &ext.h);
    return ext;
  }

};

} // namespace view
} // namespace gos

#endif // GOS__WINDOW_H__INCLUDED
