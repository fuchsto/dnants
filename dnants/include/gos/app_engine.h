#ifndef GOS__APP_ENGINE_H__INCLUDED
#define GOS__APP_ENGINE_H__INCLUDED

#include <gos/view/window.h>

#include <gos/types.h>

#include <vector>
#include <string>


namespace gos {

class app_state;
class rule_engine;

class app_engine
{
  gos::app_settings _settings {
                       { 30, 30 }, // grid extents
                       16,         // cell pixels (16 or 32)
                       30,         // fps
                       5,          // rps
                       30,         // trace rounds
                       50,         // initial team size
                       2,          // number of teams
                       true,       // show grid
                       true        // show traces
                    };

  std::vector<app_state *>   _states;
  gos::view::window          _window;
  bool                       _is_running      = false;
  uint32_t                   _frame_time      = 0;
  uint32_t                   _last_frame_time = 0;
  uint32_t                   _delta_time      = 0;

 public:
  app_engine(
    std::string               title,
    const gos::app_settings & settings);

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
