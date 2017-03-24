
#include <gos/app_engine.h>
#include <gos/app_state.h>
#include <gos/util/logging.h>

#include <SDL.h>

#include <string>


namespace gos {

app_engine::app_engine(
  std::string title,
  int         width,
  int         height,
  int         bpp)
: _window(*this, title, width, height, bpp) {
  GOS__LOG_DEBUG("app_engine", "app_engine()");
//SDL_WM_SetCaption(title, title);
//int flags = 0;
//_screen   = SDL_SetVideoMode(width, height, bpp, flags);
  _is_running = true;
  GOS__LOG_DEBUG("app_engine", "app_engine >");
}

app_engine::~app_engine() {
  GOS__LOG_DEBUG("app_engine", "~app_engine()");
  while (!_states.empty()) {
    _states.back()->finalize();
    _states.pop_back();
  }
  // reset video mode, for example to exit from fullscreen
  // mode:
//_screen = SDL_SetVideoMode(640, 480, 0, 0);
//SDL_Quit();
  GOS__LOG_DEBUG("app_engine", "~app_engine >");
}

void app_engine::change_state(app_state * state) {
  if (!_states.empty()) {
    _states.back()->finalize();
    _states.pop_back();
  }
  _states.push_back(state);
  _states.back()->initialize();
}

void app_engine::push_state(app_state * state) {
  if (!_states.empty()) {
    _states.back()->pause();
  }
  _states.push_back(state);
  _states.back()->initialize();
}

void app_engine::pop_state() {
  if (!_states.empty()) {
    _states.back()->finalize();
    _states.pop_back();
  }
  if (!_states.empty()) {
    _states.back()->resume();
  }
}

void app_engine::handle_events() {
  GOS__LOG_DEBUG("app_engine", "handle_events");
  if (_states.empty()) { return; }

  _states.back()->handle_events(this);
}

void app_engine::update() {
  GOS__LOG_DEBUG("app_engine", "update");
  if (_states.empty()) { return; }

  _states.back()->update(this);
}

void app_engine::draw() {
  GOS__LOG_DEBUG("app_engine", "draw");
  if (_states.empty()) { return; }

  _frame_time      = SDL_GetTicks();
  _delta_time      = _frame_time - _last_frame_time;
  _last_frame_time = _frame_time;

  _states.back()->draw(this);

  if ((SDL_GetTicks() - _frame_time) < _min_frame_time) {
    SDL_Delay(_min_frame_time - (SDL_GetTicks() - _frame_time));
  }
}

} // namespace gos

