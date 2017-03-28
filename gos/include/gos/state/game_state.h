#ifndef GOS__STATE__GAME_STATE_H__INCLUDED
#define GOS__STATE__GAME_STATE_H__INCLUDED

#include <gos/app_engine.h>

#include <gos/types.h>

#include <gos/state/grid.h>
#include <gos/state/population.h>
#include <gos/state/map_generator.h>


namespace gos {
namespace state {

class game_state {
  using grid_state       = gos::state::grid;
  using population_state = gos::state::population;
  using map_generator    = gos::state::map_generator;

  extents            _grid_extents;
  grid_state       * _grid_front    = nullptr;
  grid_state       * _grid_back     = nullptr;
  population_state * _popul_front   = nullptr;
  population_state * _popul_back    = nullptr;
  app_engine       * _app           = nullptr;

 public:
  game_state(app_engine * app)
  : _app(app)
  {
    _grid_extents  = app->settings().grid_extents;
    map_generator map_gen(_grid_extents);
    map_gen.set_num_grass_regions(4).set_num_food_regions(8);
    // back and front game state:
    _grid_front    = map_gen.make_grid();
    _grid_back     = new grid_state(*_grid_front);
  }

  ~game_state() {
    delete _grid_front;
    _grid_front  = nullptr;
    delete _grid_back;
    _grid_back   = nullptr;
    delete _popul_front;
    _popul_front = nullptr;
    delete _popul_back;
    _popul_back  = nullptr;
  }

  /// Blocks until back state is ready, then swaps front and back state
  /// and performs asynchronous update of new back state.
  void               update()     { }

  grid_state       & grid()       { return *_grid_front;  }
  population_state & population() { return *_popul_front; }

};

} // namespace state
} // namespace gos

#endif // GOS__STATE__GAME_STATE_H__INCLUDED
