#ifndef GOS__APP_STATE_H__INCLUDED
#define GOS__APP_STATE_H__INCLUDED

#include <gos/app_engine.h>


namespace gos {

class app_state
{
 protected:
  app_state() { }
 public:
  virtual ~app_state() { }

  virtual void initialize() = 0;
  virtual void finalize()   = 0;

  virtual void pause()      = 0;
  virtual void resume()     = 0;

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
