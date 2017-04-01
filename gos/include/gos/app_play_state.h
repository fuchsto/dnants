#ifndef GOS__APP_PLAY_STATE_H__INCLUDED
#define GOS__APP_PLAY_STATE_H__INCLUDED

#include <SDL.h>

#include <gos/app_state.h>
#include <gos/state.h>
#include <gos/state/game_state.h>
#include <gos/state/ant.h>


namespace gos {

class app_play_state : public app_state {
  using game_state  = gos::state::game_state;
  using cell_traces = gos::state::cell_state::traces;

  enum class sprite_tag : int {
    rock = 0,
    sugah_1,
    sugah_2,
    sugah_3,
    sugah_4,
    ant_1,
    ant_2,
    ant_3,
    ant_4
  };

 protected:
  app_play_state() { }

 private:
  static app_play_state _instance;

 private:
  bool         _active          = true;
  bool         _paused          = false;
  timestamp_t  _last_round_ms   = 0;
  int          _grid_spacing    = 5;
  extents      _grid_extents;
  position     _marked_cell     { -1, -1 };

  app_engine * _app             = nullptr;
  game_state * _game_state      = nullptr;

  std::array<SDL_Surface *, 9> _sprites { };

  rgba         _team_colors[4]  { { 0xff, 0x12, 0x66, 0xff },
                                  { 0x00, 0xaa, 0x23, 0xff },
                                  { 0x87, 0x57, 0xe8, 0xff },
                                  { 0x84, 0xa8, 0x36, 0xff } };

  rgba         _highlight_color { 0xaf, 0x12, 0x12, 0xff };
  rgba         _blocked_color   { 0xff, 0x12, 0x12, 0xff };
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
            } else if( event.key.keysym.scancode == SDL_SCANCODE_Q) {
              // quit
              app->quit();
            } else if( event.key.keysym.scancode == SDL_SCANCODE_G) {
              // toggle grid
              app->settings().show_grid = !(app->settings().show_grid);
            } else if( event.key.keysym.scancode == SDL_SCANCODE_T) {
              // toggle traces
              app->settings().show_traces = !(app->settings().show_traces);
            } else if( event.key.keysym.scancode == SDL_SCANCODE_S) {
              // speed up
              if (app->settings().rounds_per_sec <
                  app->settings().frames_per_sec) {
                app->settings().rounds_per_sec++;
              }
            } else if( event.key.keysym.scancode == SDL_SCANCODE_A) {
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
            }
            break;
        default:
            break;
        }
    }
  }

  virtual void update(app_engine * app) {
    auto ms             = gos::timestamp_ns() / 1000000;
    auto rounds_per_sec = app->settings().rounds_per_sec;
    auto ms_per_round   = 1000 / rounds_per_sec;
    if (ms - _last_round_ms >= ms_per_round) {
      _last_round_ms = ms;
      if (!_paused) {
        GOS__LOG_DEBUG("app_play_state.update",
                       "round: " << _game_state->round_count());
        _game_state->next();
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

    SDL_RenderPresent(
      app->win().renderer());
  }

 private:
  void render_map(gos::view::window & win) {
    auto         ext      = win.view_extents();
    int          n_trace  = _app->settings().trace_rounds;
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
          case gos::state::cell_type::barrier:
            render_barrier_cell(cell_x, cell_y);
            break;
          default:
            break;
        }
        if (_app->settings().show_traces) {
          for (const auto & team : popul.teams()) {
            int team_id          = team.id();
            const auto & traces  = grid_cell.state()->get_traces(team_id);
            for (int oidx = 0; oidx < 8; ++oidx) {
              int trace_rc    = traces[oidx];
              orientation ort = gos::int2or(oidx);
              int trace_age   = _game_state->round_count() - trace_rc;
              int trace_val   = n_trace - trace_age;
              if (trace_rc > 0 && trace_val > 0) {
                render_trace_cell(
                  cell_x, cell_y,
                  trace_val, ort,
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

  void render_ant(const gos::state::ant & ant) {
    if (!ant.is_alive()) {
      return;
    }
    int ant_size = 
      std::max<int>(
        std::min<int>(
          ((ant.strength() * _grid_spacing) /
           gos::state::ant::max_strength()),
          _grid_spacing)
          - 2,
        _grid_spacing / 5);

    int strength_qurt = (((ant.strength() * 100) /
                           ant.max_strength()) / 25
                        ) + 1;

    const rgba & col = _team_colors[ant.team().id()];

    int size   = 16;
    int cell_x = ant.pos().x;
    int cell_y = ant.pos().y;
    SDL_Point center { (cell_x * _grid_spacing),
                       (cell_y * _grid_spacing) };
    SDL_Rect dst_rect;
    dst_rect.x = center.x + (_grid_spacing - size) / 2;
    dst_rect.y = center.y + (_grid_spacing - size) / 2;
    dst_rect.w = 16;
    dst_rect.h = 16;

    SDL_SetRenderDrawBlendMode(
      _app->win().renderer(),
      SDL_BLENDMODE_BLEND);

    sprite_tag sprite;
    if      (strength_qurt >= 4) { sprite = sprite_tag::ant_4; }
    else if (strength_qurt >= 3) { sprite = sprite_tag::ant_3; }
    else if (strength_qurt >= 2) { sprite = sprite_tag::ant_2; }
    else                         { sprite = sprite_tag::ant_1; }

    SDL_Surface * surface = _sprites[(int)sprite];
    SDL_SetColorKey(
      surface, SDL_TRUE,
      SDL_MapRGB(surface->format, 255, 0, 255));
    SDL_Texture * texture =
      SDL_CreateTextureFromSurface(
        _app->win().renderer(),
        surface);
    SDL_SetTextureColorMod(
      texture, col.r, col.g, col.b);

    SDL_RenderCopyEx(_app->win().renderer(),
                     texture,
                     0,
                     &dst_rect,
                     gos::or2deg(ant.orientation()),
                     0,
                     SDL_FLIP_NONE);

    SDL_DestroyTexture(texture);
  }

  void render_trace_cell(
    int          cell_x,
    int          cell_y,
    int          trace_value,
    orientation  ort,
    int          team_id)
  {
    int max_trace_rounds = _app->settings().trace_rounds;
    int center_x = (cell_x * _grid_spacing) + (_grid_spacing / 2);
    int center_y = (cell_y * _grid_spacing) + (_grid_spacing / 2);

    if (team_id % 2) {
      center_x -= 2;
      center_y -= 2;
    } else {
      center_x += 2;
      center_y += 2;
    }

    const rgba & col = _team_colors[team_id];
    Uint8 tcol_a = static_cast<Uint8>(
                     ( ((trace_value * 256) / max_trace_rounds)
                       * 4)
                     / 5);
    SDL_SetRenderDrawColor(
      _app->win().renderer(),
      col.r, col.g, col.b, tcol_a);

    direction dir = gos::or2dir(ort);
    int       off = (_grid_spacing / 6);
    int       len = (_grid_spacing * 3) / 4;
    position from { center_x + (dir.dx * off), center_y + (dir.dy * off) };
    position to   { center_x + (dir.dx * len), center_y + (dir.dy * len) };

    SDL_RenderDrawLine(
      _app->win().renderer(),
      from.x, from.y,
      to.x,   to.y
    );
  }

  void render_grass_cell(int cell_x, int cell_y) {
    draw_cell_fill_rectangle(
      _app->win(),
      cell_x, cell_y, _grid_spacing,
      _grass_color);
  }

  void render_food_cell(int cell_x, int cell_y) {
    const auto & cell = _game_state->grid_state().at(cell_x, cell_y);
    const gos::state::food_cell_state * cell_state =
            reinterpret_cast<const gos::state::food_cell_state *>(
              cell.state());
    int amount_left = cell_state->amount_left();
    int amount_max  = cell_state->max_amount();
    int amount_qurt = (((amount_left * 100) / amount_max) / 25) + 1;
    if (amount_left == 0) { return; }

    sprite_tag sprite;
    if      (amount_qurt >= 4) { sprite = sprite_tag::sugah_4; }
    else if (amount_qurt >= 3) { sprite = sprite_tag::sugah_3; }
    else if (amount_qurt >= 2) { sprite = sprite_tag::sugah_2; }
    else                       { sprite = sprite_tag::sugah_1; }

    SDL_Point center { (cell_x * _grid_spacing),
                       (cell_y * _grid_spacing) };
    int size = 16;
    SDL_Rect dst_rect;
    dst_rect.x = center.x + (_grid_spacing - size) / 2;
    dst_rect.y = center.y + (_grid_spacing - size) / 2;
    dst_rect.w = size;
    dst_rect.h = size;
    SDL_SetRenderDrawBlendMode(
      _app->win().renderer(),
      SDL_BLENDMODE_BLEND);

    SDL_Surface * surface = _sprites[(int)sprite];
    SDL_SetColorKey(
      surface, SDL_TRUE,
      SDL_MapRGB(surface->format, 255, 0, 255));
    SDL_Texture * texture =
      SDL_CreateTextureFromSurface(
        _app->win().renderer(),
        surface);

    SDL_RenderCopy(_app->win().renderer(),
                   texture,
                   0,
                   &dst_rect);

    SDL_DestroyTexture(texture);
  }

  void render_barrier_cell(int cell_x, int cell_y) {
    SDL_Point center { (cell_x * _grid_spacing),
                       (cell_y * _grid_spacing) };
    int size = 16;
    SDL_Rect dst_rect;
    dst_rect.x = center.x + (_grid_spacing - size) / 2;
    dst_rect.y = center.y + (_grid_spacing - size) / 2;
    dst_rect.w = size;
    dst_rect.h = size;
    SDL_SetRenderDrawBlendMode(
      _app->win().renderer(),
      SDL_BLENDMODE_BLEND);

    SDL_Surface * surface = _sprites[(int)sprite_tag::rock];
    SDL_SetColorKey(
      surface, SDL_TRUE,
      SDL_MapRGB(surface->format, 255, 0, 255));
    SDL_Texture * texture =
      SDL_CreateTextureFromSurface(
        _app->win().renderer(),
        surface);

    SDL_RenderCopy(_app->win().renderer(),
                   texture,
                   0,
                   &dst_rect);

    SDL_DestroyTexture(texture);
  }

  void render_grid(gos::view::window & win);


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
    const rgba & col);

  void log_cell(const position & cell_pos) {
    const auto & grid      = _game_state->grid_state();
    const auto & grid_cell = grid.at(cell_pos.x, cell_pos.y);
    GOS__LOG_DEBUG("app_play_state", "marked cell: " << grid_cell);
    if (grid_cell.is_taken()) {
      gos::state::ant_id aid = grid_cell.ant();
      const auto & popul     = _game_state->population_state();
      const auto & cell_ant  = popul.teams()[aid.team_id].ants()[aid.id];
      GOS__LOG_DEBUG("app_play_state", "marked ant: " << cell_ant);
    }
  }
};

} // namespace gos

#endif // GOS__APP_PLAY_STATE_H__INCLUDED
