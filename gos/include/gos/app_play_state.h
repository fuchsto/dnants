#ifndef GOS__APP_PLAY_STATE_H__INCLUDED
#define GOS__APP_PLAY_STATE_H__INCLUDED

#include <gos/app_state.h>

#include <gos/rule_engine.h>
#include <gos/state.h>


namespace gos {

class app_play_state : public app_state {
 protected:
  app_play_state() { }

 private:
  static app_play_state _instance;

 private:
  int     _grid_spacing    = 15;

 public:
  static app_play_state * get() { return &_instance; }

  virtual ~app_play_state() { }

  virtual void initialize() { }
  virtual void finalize()   { }

  virtual void pause()      { }
  virtual void resume()     { }

  virtual void handle_events(app_engine * app) {
    SDL_Event event;
    if(SDL_PollEvent(&event))  {
        switch (event.type) {
        case SDL_QUIT:
            app->quit();
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            break;
        default:
            break;
        }
    }
  }

  virtual void update(app_engine * app) {
  }

  virtual void draw(app_engine * app) {
    SDL_SetRenderDrawColor(
      app->win().renderer(), 0xc3, 0xc3, 0xc3, 0x00);
    SDL_RenderClear(
      app->win().renderer());

    render_objects(app->win());
    render_grid(app->win());

    SDL_RenderPresent(
      app->win().renderer());
  }

 private:
  void render_objects(gos::view::window & win) {
    auto ext     = win.view_extents();
    int  nrows   = ext.h / _grid_spacing;
    int  ncols   = ext.w / _grid_spacing;

    Uint32 ticks = SDL_GetTicks();

    int  cell_x  = ticks % ncols;
    int  cell_y  = ticks % nrows;

    draw_cell_rectangle(
      win, cell_x, cell_y, rgba { 0xaf, 0xef, 0x9f, 0xff });
    draw_cell_triangle(
      win, cell_x, cell_y, rgba { 0x34, 0x98, 0x9f, 0x99 });
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
