
#include <gos/app_play_state.h>
#include <gos/view/svg_texture.h>

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
  _game_state    = new gos::state::game_state(_app->settings());

  _sprites[(int)sprite_tag::rock]    = SDL_LoadBMP("rock_16.bmp");
  _sprites[(int)sprite_tag::sugah_1] = SDL_LoadBMP("sugah-1_16.bmp");
  _sprites[(int)sprite_tag::sugah_2] = SDL_LoadBMP("sugah-2_16.bmp");
  _sprites[(int)sprite_tag::sugah_3] = SDL_LoadBMP("sugah-3_16.bmp");
  _sprites[(int)sprite_tag::sugah_4] = SDL_LoadBMP("sugah-4_16.bmp");
  _sprites[(int)sprite_tag::ant_1]   = SDL_LoadBMP("ant-1_16.bmp");
  _sprites[(int)sprite_tag::ant_2]   = SDL_LoadBMP("ant-2_16.bmp");
  _sprites[(int)sprite_tag::ant_3]   = SDL_LoadBMP("ant-3_16.bmp");
  _sprites[(int)sprite_tag::ant_4]   = SDL_LoadBMP("ant-4_16.bmp");

  _active        = true;
  GOS__LOG_DEBUG("app_play_state", "initialize >");
}

void app_play_state::finalize() {
  _active = false;
  delete _game_state;

  for (SDL_Surface * surface : _sprites) {
    SDL_FreeSurface(surface);
  }
  _sprites = { };
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
  int  radius_outer,
  int  radius_inner,
  gos::orientation ornt,
  const rgba & col)
{
  SDL_SetRenderDrawColor(
    win.renderer(), col.r, col.g, col.b, col.a);
  int r        = radius_outer;
  int center_x = (cell_x * _grid_spacing) + (_grid_spacing / 2);
  int center_y = (cell_y * _grid_spacing) + (_grid_spacing / 2);
  if (ornt == gos::orientation::north ||
      ornt == gos::orientation::northwest ||
      ornt == gos::orientation::northeast) {
    center_y -= radius_outer;
  }
  else if (ornt == gos::orientation::south ||
           ornt == gos::orientation::southwest ||
           ornt == gos::orientation::southeast) {
    center_y += radius_outer;
  }
  if (ornt == gos::orientation::west ||
      ornt == gos::orientation::northwest ||
      ornt == gos::orientation::southwest) {
    center_x -= radius_outer;
  }
  else if (ornt == gos::orientation::east ||
           ornt == gos::orientation::northeast ||
           ornt == gos::orientation::southeast) {
    center_x += radius_outer;
  }
  do {
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
  } while (r-- && r > radius_inner);
}

void app_play_state::draw_cell_rectangle(
  gos::view::window & win,
  int  cell_x,
  int  cell_y,
  int  size,
  const rgba & col)
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
  const rgba & col)
{
  SDL_Rect rect;
  rect.x = (cell_x * _grid_spacing) + (_grid_spacing - size) / 2;
  rect.y = (cell_y * _grid_spacing) + (_grid_spacing - size) / 2;
  rect.w = size;
  rect.h = size;

  SDL_SetRenderDrawBlendMode(
    win.renderer(), SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(
    win.renderer(), col.r, col.g, col.b, col.a);
  SDL_RenderFillRect(
    win.renderer(), &rect);
}

void app_play_state::draw_cell_triangle(
  gos::view::window & win,
  int  cell_x,
  int  cell_y,
  const rgba & col)
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