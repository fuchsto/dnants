
#include <gos/app_engine.h>
#include <gos/app_state.h>
#include <gos/util/logging.h>

#include <SDL.h>

#include <string>


namespace gos {

app_engine::app_engine(
  std::string          title,
  const app_settings & settings,
  int                  max_width,
  int                  max_height)
: _settings(settings)
, _window(
    *this, title,
    settings.grid_extents.w * 16, // (max_width / settings.grid_extents.w),
    settings.grid_extents.h * 16, // (max_width / settings.grid_extents.w),
    0) {
  GOS__LOG_DEBUG("app_engine", "app_engine()");
  GOS__LOG_DEBUG("app_engine", "app_engine >");
}

void app_engine::run(
  app_state * init_state)
{
  GOS__LOG_DEBUG("app_engine", "run()");
  _is_running = true;
  change_state(init_state);
  GOS__LOG_DEBUG("app_engine", "run >");
}

app_engine::~app_engine() {
  GOS__LOG_DEBUG("app_engine", "~app_engine()");
  while (!_states.empty()) {
    _states.back()->finalize();
    _states.pop_back();
  }
  GOS__LOG_DEBUG("app_engine", "~app_engine >");
}

void app_engine::change_state(app_state * state) {
  if (!_states.empty()) {
    _states.back()->finalize();
    _states.pop_back();
  }
  _states.push_back(state);
  _states.back()->initialize(this);
}

void app_engine::push_state(app_state * state) {
  if (!_states.empty()) {
    _states.back()->pause();
  }
  _states.push_back(state);
  _states.back()->initialize(this);
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
  if (_states.empty()) { return; }

  _states.back()->handle_events(this);
}

void app_engine::update() {
  if (_states.empty()) { return; }

  _states.back()->update(this);
}

void app_engine::draw() {
  if (_states.empty()) { return; }

  _frame_time      = SDL_GetTicks();
  _delta_time      = _frame_time - _last_frame_time;
  _last_frame_time = _frame_time;

  _states.back()->draw(this);

  Uint32 min_frame_time = 1000 / _settings.frames_per_sec;

  if ((SDL_GetTicks() - _frame_time) < min_frame_time) {
    SDL_Delay(min_frame_time - (SDL_GetTicks() - _frame_time));
  }
}

} // namespace gos

