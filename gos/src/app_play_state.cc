
#include <gos/app_play_state.h>

#include <gos/state/cell.h>
#include <gos/state/game_state.h>

#include <gos/util/logging.h>

#include <cmath>
#include <cstdlib>


namespace gos {

app_play_state app_play_state::_instance;

void app_play_state::initialize(app_engine * app) {
  GOS__LOG_DEBUG("app_play_state", "initialize()");
  _active        = true;
  _paused        = false;
  _app           = app;
  _grid_extents  = app->settings().grid_extents;
  _grid_spacing  = app->win().view_extents().w / _grid_extents.w;
  _game_state    = new gos::state::game_state(_app);

  _active        = true;
  GOS__LOG_DEBUG("app_play_state", "initialize >");
}

void app_play_state::finalize() {
  _active = false;
  delete _game_state;
}

void app_play_state::render_grid(gos::view::window & win)
{
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

void app_play_state::draw_cell_circle(
  gos::view::window & win,
  int  cell_x,
  int  cell_y,
  int  radius,
  rgba col)
{
  SDL_SetRenderDrawColor(
    win.renderer(), col.r, col.g, col.b, col.a);
  int r = radius;
  do {
    int center_x = (cell_x * _grid_spacing) + (_grid_spacing / 2);
    int center_y = (cell_y * _grid_spacing) + (_grid_spacing / 2);
    int x0       = (center_x - r + 1);
    int x1       = (center_x - (r / 2) - 1);
    int x2       = (center_x);
    int x3       = (center_x + (r / 2) + 1);
    int x4       = (center_x + r - 1);
    int y0       = (center_y - r + 1);
    int y1       = (center_y - (r / 2) - 1);
    int y2       = (center_y);
    int y3       = (center_y + (r / 2) + 1);
    int y4       = (center_y + r - 1);
    SDL_Point points[9] = {
      { x2, y0 }, // center top
      { x3, y1 },
      { x4, y2 }, // right center
      { x3, y3 },
      { x2, y4 }, // center
      { x1, y3 },
      { x0, y2 }, // left center
      { x1, y1 },
      { x2, y0 }  // center top
    };
    SDL_RenderDrawLines(
      win.renderer(), points, 9);
  } while (r-- > 3);
}

void app_play_state::draw_cell_rectangle(
  gos::view::window & win,
  int  cell_x,
  int  cell_y,
  int  size,
  rgba col)
{
  SDL_Rect rect;
  rect.x = (cell_x * _grid_spacing) + (_grid_spacing - size);
  rect.y = (cell_y * _grid_spacing) + (_grid_spacing - size);
  rect.w = size;
  rect.h = size;

  SDL_SetRenderDrawColor(
    win.renderer(), col.r, col.g, col.b, col.a);
  SDL_RenderDrawRect(
    win.renderer(), &rect);
}

void app_play_state::draw_cell_fill_rectangle(
  gos::view::window & win,
  int  cell_x,
  int  cell_y,
  int  size,
  rgba col)
{
  SDL_Rect rect;
  rect.x = (cell_x * _grid_spacing) + (_grid_spacing - size);
  rect.y = (cell_y * _grid_spacing) + (_grid_spacing - size);
  rect.w = size;
  rect.h = size;

  SDL_SetRenderDrawColor(
    win.renderer(), col.r, col.g, col.b, col.a);
  SDL_RenderFillRect(
    win.renderer(), &rect);
}

void app_play_state::draw_cell_triangle(
  gos::view::window & win,
  int  cell_x,
  int  cell_y,
  rgba col)
{
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

} // namespace gos
