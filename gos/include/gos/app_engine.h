#ifndef GOS__APP_ENGINE_H__INCLUDED
#define GOS__APP_ENGINE_H__INCLUDED

#include <gos/view/window.h>

#include <vector>
#include <string>

#include <SDL.h>


namespace gos {

class app_state;
class rule_engine;

class app_engine
{
  std::vector<app_state *>   _states;
  gos::view::window          _window;
//SDL_Surface              * _screen;
  bool                       _is_running      = false;
  Uint32                     _frame_time      = 0;
  Uint32                     _last_frame_time = 0;
  Uint32                     _delta_time      = 0;

  Uint32                     _fps             = 1;
  Uint32                     _min_frame_time  = 1000 / _fps;

 public:
  app_engine(
    std::string title,
    int         width,
    int         height,
    app_state * init_state);

  ~app_engine();

  void change_state(
         app_state  * state);
  void push_state(
         app_state  * state);
  void pop_state();

  void handle_events();
  void update();
  void draw();

  bool is_running()         const { return _is_running;  }
  gos::view::window & win()       { return _window;   }
  void quit()                     { _is_running = false; }
};

} // namespace gos

#endif // GOS__APP_ENGINE_H__INCLUDED
