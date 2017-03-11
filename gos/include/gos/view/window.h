#ifndef GOS__WINDOW_H__INCLUDED
#define GOS__WINDOW_H__INCLUDED

#include <SDL2/SDL.h>

#include <stdexcept>


namespace gos {
namespace view {

class window {

  SDL_Window   * _window;
  SDL_Renderer * _renderer;
 public:
  window(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
      throw std::runtime_error("SDL_Init failed");
    }
    SDL_CreateWindowAndRenderer(
      width,
      height,
      SDL_WINDOW_OPENGL,
      &_window,
      &_renderer);
    SDL_SetWindowPosition(_window, 100, 100);
    SDL_RenderClear(_renderer);
    SDL_RenderPresent(_renderer);

    SDL_Delay(2000);
  }

  ~window() {
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
  }

};

} // namespace view
} // namespace gos

#endif // GOS__WINDOW_H__INCLUDED
