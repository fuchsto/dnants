#ifndef GOS__APP_PLAY_STATE_H__INCLUDED
#define GOS__APP_PLAY_STATE_H__INCLUDED

#include <SDL.h>

#include <gos/app_state.h>
#include <gos/state.h>
#include <gos/state/game_state.h>
#include <gos/state/ant.h>


namespace gos {

class app_play_state : public app_state {
  using game_state = gos::state::game_state;

 protected:
  app_play_state() { }

 private:
  static app_play_state _instance;

 private:
  bool         _active          = true;
  timestamp_t  _last_round_ms   = 0;
  int          _grid_spacing    = 15;
  extents      _grid_extents;

  app_engine * _app             = nullptr;
  game_state * _game_state      = nullptr;

  rgba         _team_colors[4]  { { 0xc8, 0x37, 0x37, 0xff },
                                  { 0xe9, 0x18, 0x60, 0xff },
                                  { 0x49, 0x48, 0x16, 0xff },
                                  { 0x84, 0xa8, 0x36, 0xff } };

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
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
              app->change_state(app_play_state::get());
            } else if( event.key.keysym.scancode == SDL_SCANCODE_G) {
              app->settings().show_grid = !(app->settings().show_grid);
            } else if( event.key.keysym.scancode == SDL_SCANCODE_S) {
              // speed up
              app->settings().rounds_per_sec++;
            } else if( event.key.keysym.scancode == SDL_SCANCODE_A) {
              // slow down
              app->settings().rounds_per_sec--;
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
      GOS__LOG_DEBUG("app_play_state.update", "round ms: " << ms);
      _last_round_ms = ms;
      _game_state->next();
    }
  }

  virtual void draw(app_engine * app) {
    if (!_active) { return; }

    SDL_SetRenderDrawColor(
      app->win().renderer(), 0xc3, 0xc3, 0xc3, 0x00);
    SDL_RenderClear(
      app->win().renderer());

    render_map(app->win());
    render_objects(app->win());

    if (app->settings().show_grid) {
      render_grid(app->win());
    }

    SDL_RenderPresent(
      app->win().renderer());
  }

 private:
  void render_map(gos::view::window & win) {
    auto         ext   = win.view_extents();
    const auto & grid  = _game_state->grid_state();
    int          nrows = grid.nrows();
    int          ncols = grid.ncols();

    for (int cell_x = 0; cell_x < ncols; ++cell_x) {
      for (int cell_y = 0; cell_y < nrows; ++cell_y) {
        const auto & grid_cell = grid.at(cell_x, cell_y);
        switch (grid_cell.type()) {
          case gos::state::cell_type::grass:
            render_grass_cell(cell_x, cell_y);
            break;
          case gos::state::cell_type::food:
            render_food_cell(cell_x, cell_y);
            break;
          default:
            break;
        }
      }
    }
  }

  void render_objects(gos::view::window & win) {
    auto & population = _game_state->population_state();
    for (auto & team : population.teams()) {
      for (auto & ant : team.ants()) {
        render_ant(ant);
      }
    }
  }

  void render_ant(const gos::state::ant & ant) {
    draw_cell_circle(
      _app->win(),
      ant.pos().x, ant.pos().y,
      _team_colors[ant.team().id()]);
  }

  void render_grass_cell(int cell_x, int cell_y) {
    draw_cell_triangle(
      _app->win(), cell_x, cell_y, rgba { 0x09, 0xa8, 0x56, 0xff });
  }

  void render_food_cell(int cell_x, int cell_y) {
    draw_cell_rectangle(
      _app->win(), cell_x, cell_y, rgba { 0xaf, 0xef, 0x9f, 0xff });
  }

  void render_grid(gos::view::window & win) {
    auto ext     = win.view_extents();
    int  nrows   = ext.h / _grid_spacing;
    int  ncols   = ext.w / _grid_spacing;
    // line color:
    SDL_SetRenderDrawColor(
      win.renderer(), 0xaf, 0xaf, 0xaf, 0xFF);
    // horizontal grid:
    for (int row = 0; row < nrows; row++) {
      SDL_RenderDrawLine(
        win.renderer(),
        0,     row * _grid_spacing, // x1, y1
        ext.w, row * _grid_spacing  // x2, y2
      );
    }
    // vertical grid:
    for (int col = 0; col < ncols; col++) {
      SDL_RenderDrawLine(
        win.renderer(),
        col * _grid_spacing, 0,     // x1, y1
        col * _grid_spacing, ext.h  // x2, y2
      );
    }
  }

  void draw_cell_circle(
    gos::view::window & win,
    int  cell_x,
    int  cell_y,
    rgba col) {
    SDL_SetRenderDrawColor(
      win.renderer(), col.r, col.g, col.b, col.a);
    int spacing = _grid_spacing;
    while (spacing--) {
      if (spacing < _grid_spacing-1) {
        SDL_SetRenderDrawColor(
          win.renderer(), col.r, col.g, col.b, col.a);
      } else {
        SDL_SetRenderDrawColor(
          win.renderer(), 0x12, 0x12, 0x12, 0xff);
      }
      int center_x = (cell_x * _grid_spacing) + (_grid_spacing / 2);
      int center_y = (cell_y * _grid_spacing) + (_grid_spacing / 2);
      SDL_Point points[7] = {
        // top left
        { center_x - (spacing / 3) + 1,
          center_y - (spacing / 2) + 1 },
        // top right
        { center_x + (spacing / 3) + 1,
          center_y - (spacing / 2) + 1 },
        // center right
        { center_x + (spacing / 2) - 1,
          center_y + (spacing / 3) - 1 },
        // bottom right
        { center_x + (spacing / 2) - 1,
          center_y + (spacing / 2) - 1 },
        // bottom left
        { center_x - (spacing / 2) + 1,
          center_y + (spacing / 2) - 1 },
        // center left
        { center_x - (spacing / 2) - 1,
          center_y + (spacing / 3) - 1 },
        // top left
        { center_x - (spacing / 3) + 1,
          center_y - (spacing / 2) + 1 }
      };
      SDL_RenderDrawLines(
        win.renderer(), points, 7);
    }
  }

  void draw_cell_rectangle(
    gos::view::window & win,
    int  cell_x,
    int  cell_y,
    rgba col) {
    SDL_Rect rect;
    rect.x = cell_x * _grid_spacing;
    rect.y = cell_y * _grid_spacing;
    rect.w = _grid_spacing;
    rect.h = _grid_spacing;

    SDL_SetRenderDrawColor(
      win.renderer(), col.r, col.g, col.b, col.a);
    SDL_RenderFillRect(
      win.renderer(), &rect);
  }

  void draw_cell_triangle(
    gos::view::window & win,
    int  cell_x,
    int  cell_y,
    rgba col) {
    int spacing = _grid_spacing;
    SDL_SetRenderDrawColor(
      win.renderer(), col.r, col.g, col.b, col.a);
    while (spacing--) {
    int center_x = (cell_x * _grid_spacing) + (_grid_spacing / 2);
    int center_y = (cell_y * _grid_spacing) + (_grid_spacing / 2);
    SDL_Point points[4] = {
        // top center
        { center_x,
          center_y - (spacing / 2) + 1 },
        // bottom right
        { center_x + (spacing / 2) - 1,
          center_y + (spacing / 2) - 1 },
        // bottom left
        { center_x - (spacing / 2) + 1,
          center_y + (spacing / 2) - 1 },
        // top center
        { center_x,
          center_y - (spacing / 2) + 1 }
      };
      SDL_RenderDrawLines(
        win.renderer(), points, 4);
    }
  }


};

} // namespace gos

#endif // GOS__APP_PLAY_STATE_H__INCLUDED
