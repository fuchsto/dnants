#ifndef GOS__APP_STATE_H__INCLUDED
#define GOS__APP_STATE_H__INCLUDED

#include <gos/app_engine.h>

#include <SDL.h>

#include <chrono>


namespace gos {

class app_state
{
 protected:
  app_state() { }

  unsigned long get_ticks() const { return SDL_GetTicks(); }
  unsigned long get_msecs() const {
    auto ts_now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<
             std::chrono::milliseconds
           >(ts_now.time_since_epoch()).count();
  }


 public:
  virtual ~app_state() { }

  virtual void initialize(app_engine *) = 0;
  virtual void finalize()               = 0;

  virtual void pause()                  = 0;
  virtual void resume()                 = 0;

  virtual void handle_events(app_engine * app) = 0;
  virtual void update(app_engine * app)        = 0;
  virtual void draw(app_engine * app)          = 0;

  void change_state(app_engine * app,
                    app_state  * state) {
    app->change_state(state);
  }

};

} // namespace gos

#endif // GOS__APP_STATE_H__INCLUDED
