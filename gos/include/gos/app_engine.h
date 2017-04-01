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
  bool                       _is_running      = false;
  Uint32                     _frame_time      = 0;
  Uint32                     _last_frame_time = 0;
  Uint32                     _delta_time      = 0;

 public:
  struct app_settings {
    extents grid_extents;
    int     frames_per_sec;
    int     rounds_per_sec;
    int     trace_rounds;
    int     init_team_size;
    bool    show_grid;
    bool    show_traces;
  };

 private:
  app_settings _settings {
                  { 30, 30 }, // grid extents
                  30,         // fps
                  5,          // rps
                  30,         // trace rounds
                  50,         // initial team size
                  false,      // show grid
                  false       // show traces
               };

 public:
  app_engine(
    std::string title,
    int         width,
    int         height);

  ~app_engine();

  void run(app_state * init_state);

  void change_state(
         app_state  * state);
  void push_state(
         app_state  * state);
  void pop_state();

  void handle_events();
  void update();
  void draw();

  app_settings & settings() { return _settings;    }

  bool is_running()   const { return _is_running;  }
  gos::view::window & win() { return _window;      }
  void quit()               { _is_running = false; }
};

} // namespace gos

#endif // GOS__APP_ENGINE_H__INCLUDED
