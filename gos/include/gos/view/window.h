#ifndef GOS__WINDOW_H__INCLUDED
#define GOS__WINDOW_H__INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <stdexcept>

#include <gos/view/utils.h>


namespace gos {
namespace view {

class window {

  SDL_Window   * _window;
  SDL_Renderer * _renderer;
  SDL_Event      _event;

  Uint32         _fps             = 60;

  Uint32         _min_frame_time  = 1000 / _fps;
  Uint32         _frame_time      = 0;
  Uint32         _last_frame_time = 0;
  Uint32         _delta_time      = 0;

  bool           _quit            = false;

  int            _grid_spacing    = 10;

  TTF_Font     * _font            = nullptr;

  LTexture       _text_texture;

  typedef struct {
    int w;
    int h;
  } extents;

 public:
  window(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Couldn't initialize SDL: %s",
                   SDL_GetError());
      throw std::runtime_error("SDL init failed");
    }
    if (SDL_CreateWindowAndRenderer(
          width,
          height,
          SDL_WINDOW_RESIZABLE,
          &_window,
          &_renderer) != 0) {
      SDL_LogError(
        SDL_LOG_CATEGORY_APPLICATION,
        "Couldn't create window and renderer: %s", SDL_GetError());
      throw std::runtime_error("create window failed");
    }
    SDL_SetWindowPosition(_window, 100, 100);

    SDL_Delay(1000);

    render_loop();
  }

  ~window() {
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
  }

 private:
  void render_state() {
    render_grid();
    render_objects();
  }

  void render_objects() {
  }

  void render_grid() {
    auto ext     = view_extents();
    int  nrows   = ext.h / _grid_spacing;
    int  ncols   = ext.w / _grid_spacing;
    // line color:
    SDL_SetRenderDrawColor(
      _renderer, 0xaf, 0xaf, 0xaf, 0xFF);
    // horizontal grid:
    for (int row = 0; row < nrows; row++) {
      SDL_RenderDrawLine(
        _renderer,
        0,     row * _grid_spacing, // x1, y1
        ext.w, row * _grid_spacing  // x2, y2
      );
    }
    // vertical grid:
    for (int col = 0; col < ncols; col++) {
      SDL_RenderDrawLine(
        _renderer,
        col * _grid_spacing, 0,     // x1, y1
        col * _grid_spacing, ext.h  // x2, y2
      );
    }
  }

  void render_loop() {
    while (!_quit) {
      _frame_time = SDL_GetTicks();
      event_loop();

      _delta_time      = _frame_time - _last_frame_time;
      _last_frame_time = _frame_time;

      SDL_SetRenderDrawColor(_renderer, 0xc3, 0xc3, 0xc3, 0x00);
      SDL_RenderClear(_renderer);

      render_state();

      SDL_RenderPresent(_renderer);

      if ((SDL_GetTicks() - _frame_time) < _min_frame_time) {
        SDL_Delay(_min_frame_time - (SDL_GetTicks() - _frame_time));
      }
    }
  }

  void event_loop() {
    while(!_quit && SDL_PollEvent(&_event))  {
        switch (_event.type) {
        case SDL_QUIT:
            _quit = true;
            break;
        case SDL_KEYDOWN:
            if (_event.key.keysym.sym == SDLK_ESCAPE)
            //  _quit = true;
            break;
        default:
            break;
        }
    }
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
