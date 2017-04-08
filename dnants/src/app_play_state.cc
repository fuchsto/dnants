
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
  using gos::view::svg_texture;

  GOS__LOG_DEBUG("app_play_state.initialize", "()");
  _active        = true;
  _paused        = true;
  _app           = app;
  _grid_extents  = app->settings().grid_extents;
  _grid_spacing  = app->settings().grid_spacing;
  _game_state    = new gos::state::game_state(_app->settings());

  GOS__LOG_DEBUG("app_play_state.initialize",
                 "grid_spacing:" << _grid_spacing);

  int px = _grid_spacing;
  _sprites[(int)sprite_tag::rock]    = new svg_texture("rock.svg", px);
  _sprites[(int)sprite_tag::sugah_1] = new svg_texture("sugah-1.svg", px);
  _sprites[(int)sprite_tag::sugah_2] = new svg_texture("sugah-2.svg", px);
  _sprites[(int)sprite_tag::sugah_3] = new svg_texture("sugah-3.svg", px);
  _sprites[(int)sprite_tag::sugah_4] = new svg_texture("sugah-4.svg", px);
  _sprites[(int)sprite_tag::ant_1]   = new svg_texture("ant-1.svg", px);
  _sprites[(int)sprite_tag::ant_2]   = new svg_texture("ant-2.svg", px);
  _sprites[(int)sprite_tag::ant_3]   = new svg_texture("ant-3.svg", px);
  _sprites[(int)sprite_tag::ant_4]   = new svg_texture("ant-4.svg", px);

  _active        = true;
  GOS__LOG_DEBUG("app_play_state.initialize", "->");
}

void app_play_state::finalize() {
  _active = false;
  delete _game_state;

  for (auto * surface : _sprites) {
    delete surface;
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

void app_play_state::render_ant(const gos::state::ant & ant)
{
  if (!ant.is_alive()) {
    return;
  }

  int strength_qurt = (((ant.strength() * 100) /
                         ant.max_strength()) / 25
                      ) + 1;

  const rgba & col = _team_colors[ant.team_id()];

  int size   = _grid_spacing;
  int cell_x = ant.pos().x;
  int cell_y = ant.pos().y;
  SDL_Point center { (cell_x * _grid_spacing),
                     (cell_y * _grid_spacing) };
  SDL_Rect dst_rect;
  dst_rect.x = center.x + (_grid_spacing - size) / 2;
  dst_rect.y = center.y + (_grid_spacing - size) / 2;
  dst_rect.w = _grid_spacing;
  dst_rect.h = _grid_spacing;

  SDL_SetRenderDrawBlendMode(
    _app->win().renderer(),
    SDL_BLENDMODE_BLEND);

  sprite_tag sprite;
  if      (strength_qurt >= 4) { sprite = sprite_tag::ant_4; }
  else if (strength_qurt >= 3) { sprite = sprite_tag::ant_3; }
  else if (strength_qurt >= 2) { sprite = sprite_tag::ant_2; }
  else                         { sprite = sprite_tag::ant_1; }

  SDL_Surface * surface = _sprites[(int)sprite]->surface();
  // SDL_SetColorKey(
  //   surface, SDL_TRUE,
  //   SDL_MapRGB(
  //     surface->format,
  //     _map_rgb_mode.r,
  //     _map_rgb_mode.g,
  //     _map_rgb_mode.b));
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

  if (ant.num_carrying() > 0) {
    int size = _grid_spacing;
    SDL_Rect dst_rect;
    dst_rect.x = center.x + ant.dir().dx * (_grid_spacing / 4);
    dst_rect.y = center.y + ant.dir().dy * (_grid_spacing / 4);
    dst_rect.w = size;
    dst_rect.h = size;
    SDL_SetRenderDrawBlendMode(
      _app->win().renderer(),
      SDL_BLENDMODE_BLEND);

    SDL_Surface * surface = _sprites[(int)sprite_tag::sugah_2]->surface();
    // SDL_SetColorKey(
    //   surface, SDL_TRUE,
    //   SDL_MapRGB(
    //     surface->format,
    //     _map_rgb_mode.r,
    //     _map_rgb_mode.g,
    //     _map_rgb_mode.b));
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
  if (ant.state().events.collision) {
    draw_cell_rectangle(
      _app->win(),
      ant.pos().x, ant.pos().y,
      _grid_spacing - 1,
      _blocked_color);
  }
}

void app_play_state::render_barrier_cell(
  int cell_x,
  int cell_y)
{
  SDL_Point center { (cell_x * _grid_spacing),
                     (cell_y * _grid_spacing) };
  int size = _grid_spacing;
  SDL_Rect dst_rect;
  dst_rect.x = center.x + (_grid_spacing - size) / 2;
  dst_rect.y = center.y + (_grid_spacing - size) / 2;
  dst_rect.w = size;
  dst_rect.h = size;
  SDL_SetRenderDrawBlendMode(
    _app->win().renderer(),
    SDL_BLENDMODE_BLEND);

  SDL_Surface * surface = _sprites[(int)sprite_tag::rock]->surface();
  // SDL_SetColorKey(
  //   surface, SDL_TRUE,
  //   SDL_MapRGB(
  //     surface->format,
  //     _map_rgb_mode.r,
  //     _map_rgb_mode.g,
  //     _map_rgb_mode.b));
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

void app_play_state::render_cell_in_trace(
  int       cell_x,
  int       cell_y,
  int       trace_value,
  direction dir,
  int       team_id)
{
  // draw trace from ingoing direction to cell center:

  int center_x = (cell_x * _grid_spacing) + (_grid_spacing / 2);
  int center_y = (cell_y * _grid_spacing) + (_grid_spacing / 2);

  const rgba & col           = _team_colors[team_id];
  const int    max_intensity = 300;
  Uint8 tcol_a = static_cast<Uint8>(
                   ((trace_value * 256 * 4) / (max_intensity)) / 5);
  SDL_SetRenderDrawColor(
    _app->win().renderer(),
    col.r, col.g, col.b, tcol_a);

  int       off = (_grid_spacing / 2);
  int       len = (_grid_spacing / 4);
  position from { center_x + (dir.dx * off), center_y + (dir.dy * off) };
  position to   { from.x   - (dir.dx * len), from.y   - (dir.dy * len) };

  SDL_RenderDrawLine(
    _app->win().renderer(),
    from.x, from.y,
    to.x,   to.y
  );
}

void app_play_state::render_cell_out_trace(
  int       cell_x,
  int       cell_y,
  int       trace_value,
  direction dir,
  int       team_id)
{
  // draw trace from cell center in trace direction:

  int center_x = (cell_x * _grid_spacing) + (_grid_spacing / 2);
  int center_y = (cell_y * _grid_spacing) + (_grid_spacing / 2);

  const rgba & col           = _team_colors[team_id];
  const int    max_intensity = 300;
  Uint8 tcol_a = static_cast<Uint8>(
                   ((trace_value * 256 * 4) / (max_intensity)) / 5);
  SDL_SetRenderDrawColor(
    _app->win().renderer(),
    col.r, col.g, col.b, tcol_a);

  int       len = (_grid_spacing / 4);
  position from { center_x                 , center_y                  };
  position to   { center_x + (dir.dx * len), center_y + (dir.dy * len) };

  SDL_RenderDrawLine(
    _app->win().renderer(),
    from.x, from.y,
    to.x,   to.y
  );
}

void app_play_state::render_food_cell(int cell_x, int cell_y) {
  const auto & cell_state = _game_state->grid_state()
                               .at(cell_x, cell_y)
                               .state();
  int amount_left = cell_state.num_food();
  int amount_max  = 4;
  int amount_qurt = (((amount_left * 100) / amount_max) / 25) + 1;
  if (amount_left == 0) { return; }

  sprite_tag sprite;
  if      (amount_qurt >= 4) { sprite = sprite_tag::sugah_4; }
  else if (amount_qurt >= 3) { sprite = sprite_tag::sugah_3; }
  else if (amount_qurt >= 2) { sprite = sprite_tag::sugah_2; }
  else                       { sprite = sprite_tag::sugah_1; }

  SDL_Point center { (cell_x * _grid_spacing),
                     (cell_y * _grid_spacing) };
  int size = _grid_spacing;
  SDL_Rect dst_rect;
  dst_rect.x = center.x + (_grid_spacing - size) / 2;
  dst_rect.y = center.y + (_grid_spacing - size) / 2;
  dst_rect.w = size;
  dst_rect.h = size;
  SDL_SetRenderDrawBlendMode(
    _app->win().renderer(),
    SDL_BLENDMODE_BLEND);

  SDL_Surface * surface = _sprites[(int)sprite]->surface();
  // SDL_SetColorKey(
  //   surface, SDL_TRUE,
  //   SDL_MapRGB(
  //     surface->format,
  //     _map_rgb_mode.r,
  //     _map_rgb_mode.g,
  //     _map_rgb_mode.b));
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
