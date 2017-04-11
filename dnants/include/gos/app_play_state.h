#ifndef GOS__APP_PLAY_STATE_H__INCLUDED
#define GOS__APP_PLAY_STATE_H__INCLUDED

#include <SDL.h>

#include <gos/app_state.h>

#include <gos/state/ant.h>
#include <gos/state/cell.h>
#include <gos/state/grid.h>
#include <gos/state/population.h>
#include <gos/state/game_state.h>

#include <gos/view/svg_texture.h>

#include <gos/types.h>


namespace gos {

class app_play_state : public app_state {
  using game_state  = gos::state::game_state;
  using cell_traces = gos::state::cell_state::traces;
  using svg_texture = gos::view::svg_texture;

  enum class res_tag : int {
    commands = 0,
    rock,
    sugah_1,
    sugah_2,
    sugah_3,
    sugah_4,
    ant_1,
    ant_2,
    ant_3,
    ant_4,
    evt_attacked,
    evt_enemy,
    act_attack,
    ico_play,
    ico_pause,
    num_0,
    num_1,
    num_2,
    num_3,
    num_4,
    num_5,
    num_6,
    num_7,
    num_8,
    num_9,
    num_res_tags
  };

 protected:
  app_play_state() { }

 private:
  static app_play_state _instance;

 private:
  bool         _active          = true;
  bool         _paused          = false;
  bool         _step            = false;
  bool         _show_commands   = true;
  bool         _show_in_traces  = true;
  bool         _show_out_traces = true;
  timestamp_t  _last_round_ms   = 0;
  int          _grid_spacing    = 5;
  extents      _grid_extents;
  position     _marked_cell     { -1, -1 };

  app_engine * _app             = nullptr;
  game_state * _game_state      = nullptr;

  std::array<svg_texture *, (size_t)res_tag::num_res_tags> _sprites { };

  rgba         _team_colors[4]  { { 0xff, 0x12, 0x66, 0x88 },
                                  { 0x00, 0xaa, 0x23, 0x88 },
                                  { 0x87, 0x57, 0xe8, 0x88 },
                                  { 0x84, 0xa8, 0x36, 0x88 } };

  rgba         _map_rgb_mode    { 0x00, 0x00, 0x00, 0x00 };

  rgba         _highlight_color { 0xaf, 0x12, 0x12, 0xff };
  rgba         _blocked_color   { 0xff, 0xb9, 0x47, 0xff };
  rgba         _taken_color     { 0x23, 0x45, 0x45, 0xff };
  rgba         _food_color      { 0xfa, 0xb7, 0x05, 0xff };
  rgba         _grass_color     { 0xaa, 0xde, 0x87, 0xff };
 public:
  static app_play_state * get() { return &_instance; }

  virtual ~app_play_state() { }

  virtual void initialize(app_engine * app);
  virtual void finalize();

  virtual void pause()      { }
  virtual void resume()     { }

  virtual void handle_events(app_engine * app) {
    SDL_Event event;
    while(SDL_PollEvent(&event))  {
        switch (event.type) {
        case SDL_QUIT:
            app->quit();
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (_paused &&
                event.button.button == SDL_BUTTON_LEFT) {
              position clicked_cell_pos { event.button.x / _grid_spacing,
                                          event.button.y / _grid_spacing };
              GOS__LOG_DEBUG("app_play_state.handle_events",
                             "clicked grid " <<
                             "(" << clicked_cell_pos.x <<
                             "," << clicked_cell_pos.y << ")");
              _marked_cell = clicked_cell_pos;
              log_cell(_marked_cell);
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
              // reset
              app->change_state(app_play_state::get());
              _show_commands = false;
            } else if( event.key.keysym.scancode == SDL_SCANCODE_H) {
              // help
              _show_commands = !_show_commands;
            } else if( event.key.keysym.scancode == SDL_SCANCODE_Q) {
              // quit
              app->quit();
            } else if( event.key.keysym.scancode == SDL_SCANCODE_G) {
              // toggle grid
              app->settings().show_grid = !(app->settings().show_grid);
            } else if( event.key.keysym.scancode == SDL_SCANCODE_T) {
              // toggle traces
              app->settings().show_traces = !(app->settings().show_traces);
            } else if( event.key.keysym.scancode == SDL_SCANCODE_I) {
              // toggle in traces
              _show_in_traces = !_show_in_traces;
            } else if( event.key.keysym.scancode == SDL_SCANCODE_O) {
              // toggle out traces
              _show_out_traces = !_show_out_traces;
            } else if( event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
              // speed up
              if (app->settings().rounds_per_sec <
                  app->settings().frames_per_sec) {
                app->settings().rounds_per_sec++;
              }
            } else if( event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
              // slow down
              if (app->settings().rounds_per_sec > 1) {
                app->settings().rounds_per_sec--;
              }
            } else if( event.key.keysym.scancode == SDL_SCANCODE_P) {
              // pause
              _paused = !_paused;
              if (!_paused) {
                _marked_cell = { -1, -1 };
              }
              _show_commands = false;
            } else if( event.key.keysym.scancode == SDL_SCANCODE_N) {
              // single-step
              _step   = true;
              _paused = false;
            }
            break;
        default:
            break;
        }
    }
  }

  virtual void update(app_engine * app) {
    long ms             = gos::timestamp_ns() / 1000000;
    long rounds_per_sec = app->settings().rounds_per_sec;
    long ms_per_round   = 1000 / rounds_per_sec;
    if (ms - static_cast<long>(_last_round_ms) >= ms_per_round) {
      _last_round_ms = ms;
      if (!_paused) {
        GOS__LOG_DEBUG("app_play_state.update",
                       "round: " << _game_state->round_count());
        _game_state->next();
      }
      if (_step) {
        _paused = true;
        _step   = false;
      }
    }
  }

  virtual void draw(app_engine * app) {
    if (!_active) { return; }

    SDL_SetRenderDrawColor(
      app->win().renderer(), 0xc3, 0xc3, 0xc3, 0x00);
    SDL_RenderClear(
      app->win().renderer());

    SDL_SetRenderDrawBlendMode(
      app->win().renderer(), SDL_BLENDMODE_BLEND);

    render_map(app->win());
    render_objects(app->win());

    if (app->settings().show_grid) {
      render_grid(app->win());
    }

    if (_marked_cell.x != -1 && _marked_cell.y != -1) {
      render_highlight_cell(_marked_cell);
    }

    render_statusbar(app->win());

    if (_show_commands) {
      render_commands_overlay(app->win());
    }

    SDL_RenderPresent(
      app->win().renderer());
  }

 private:
  void render_map(gos::view::window & win) {
    auto         tick_now = _game_state->round_count();
    const auto & grid     = _game_state->grid_state();
    const auto & grid_ext = grid.extents();
    const auto & popul    = _game_state->population_state();

    for (int cell_x = 0; cell_x < grid_ext.w; ++cell_x) {
      for (int cell_y = 0; cell_y < grid_ext.h; ++cell_y) {
        const auto & grid_cell = grid.at(cell_x, cell_y);
        switch (grid_cell.type()) {
          case gos::state::cell_type::grass:
          //render_grass_cell(cell_x, cell_y);
            break;
          case gos::state::cell_type::food:
            render_food_cell(cell_x, cell_y);
            break;
          case gos::state::cell_type::plain:
            render_food_cell(cell_x, cell_y);
            break;
          case gos::state::cell_type::barrier:
            render_barrier_cell(cell_x, cell_y);
            break;
          default:
            break;
        }
        if (_app->settings().show_traces) {
          for (const auto & team : popul.teams()) {
            int team_id             = team.id();
            const auto & in_traces  = grid_cell.state().in_traces(team_id);
            const auto & out_traces = grid_cell.state().out_traces(team_id);
            for (int oidx = 0; oidx < 8; ++oidx) {
              direction tdir    = gos::or2dir(gos::int2or(oidx));
              int in_trace_val  = in_traces[oidx]  - tick_now;
              int out_trace_val = out_traces[oidx] - tick_now;
              if (_show_in_traces && in_trace_val > 0) {
                render_cell_in_trace(
                  cell_x, cell_y,
                  in_trace_val,
                  tdir,
                  team_id);
              }
              if (_show_out_traces && out_trace_val > 0) {
                render_cell_out_trace(
                  cell_x, cell_y,
                  out_trace_val,
                  tdir,
                  team_id);
              }
            }
          }
        }
      }
    }
  }

  void render_objects(gos::view::window & win) {
    auto & population = _game_state->population_state();
    for (const auto & team : population.teams()) {
      for (const auto & spawn_point : team.spawn_points()) {
        draw_cell_circle(
          _app->win(),
          spawn_point.x, spawn_point.y,
          _grid_spacing - 1,
          _grid_spacing - 3,
          gos::orientation::none,
          _team_colors[team.id()]
        );
      }
      for (const auto & ant : team.ants()) {
        render_ant(ant);
      }
    }
  }

  void render_commands_overlay(gos::view::window & win) {
    const svg_texture * commands_layer = _sprites[(int)res_tag::commands];
    SDL_Rect commands_rect;
    commands_rect.w = 320;
    commands_rect.h = 320;
    commands_rect.x = (win.map_rect().w / 2) - (commands_rect.w / 2);
    commands_rect.y = (win.map_rect().h / 2) - (commands_rect.h / 2);
    commands_layer->render(win.renderer(), commands_rect);
  }

  void render_grid(gos::view::window & win);

  void render_statusbar(gos::view::window & win);

  void render_number(
    gos::view::window & win,
    const SDL_Rect    & rect,
    const rgba        & col,
    int                 num);

  void render_ant(const gos::state::ant & ant);

  void render_cell_in_trace(
    int       cell_x,
    int       cell_y,
    int       trace_value,
    direction dir,
    int       team_id);

  void render_cell_out_trace(
    int       cell_x,
    int       cell_y,
    int       trace_value,
    direction dir,
    int       team_id);

  void render_grass_cell(int cell_x, int cell_y) {
    draw_cell_fill_rectangle(
      _app->win(),
      cell_x, cell_y, _grid_spacing,
      _grass_color);
  }

  void render_food_cell(int cell_x, int cell_y);

  void render_barrier_cell(int cell_x, int cell_y);

  void render_highlight_cell(const position & cell_pos) {
    draw_cell_rectangle(
      _app->win(),
      cell_pos.x, cell_pos.y,
      _grid_spacing,
      _highlight_color);
  }

  void draw_cell_circle(
    gos::view::window & win,
    int  cell_x,
    int  cell_y,
    int  radius_outer,
    int  radius_inner,
    gos::orientation ornt,
    const rgba & col);

  void draw_cell_rectangle(
    gos::view::window & win,
    int  cell_x,
    int  cell_y,
    int  size,
    const rgba & col);

  void draw_cell_fill_rectangle(
    gos::view::window & win,
    int  cell_x,
    int  cell_y,
    int  size,
    const rgba & col);

  void draw_cell_triangle(
    gos::view::window & win,
    int  cell_x,
    int  cell_y,
    int  size,
    const rgba & col);

  void log_cell(const position & cell_pos) {
    const auto & grid      = _game_state->grid_state();
    const auto & grid_cell = grid.at(cell_pos.x, cell_pos.y);
    std::cout << grid_cell << '\n';
    if (grid_cell.is_taken()) {
      gos::state::ant_id aid = grid_cell.ant();
      const auto & popul     = _game_state->population_state();
      const auto & cell_ant  = popul.teams()[aid.team_id].ants()[aid.id];
      std::cout << cell_ant << '\n';
    }
  }
};

} // namespace gos

#endif // GOS__APP_PLAY_STATE_H__INCLUDED
