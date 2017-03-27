
#include <gos/app_play_state.h>

#include <gos/state/cell.h>
#include <gos/state/map_generator.h>

#include <gos/util/logging.h>

#include <cmath>
#include <cstdlib>


namespace gos {

app_play_state app_play_state::_instance;

void app_play_state::initialize(app_engine * app) {
  GOS__LOG_DEBUG("app_play_state", "initialize()");
  _app           = app;
  _num_col_cells = app->settings().grid_extents.w;
  _num_row_cells = app->settings().grid_extents.h;
  _grid_spacing  = app->win().view_extents().w / _num_col_cells;
  
  gos::state::map_generator map_gen(_num_row_cells,
                                    _num_col_cells);
  map_gen.set_num_grass_regions(4).set_num_food_regions(8);
  // back and front game state:
  _grid_front    = map_gen.make_grid();
  _grid_back     = new gos::state::grid(*_grid_front);

  _active        = true;
  GOS__LOG_DEBUG("app_play_state", "initialize >");
}

void app_play_state::finalize() {
  _active = false;

  delete _grid_front;
  _grid_front = nullptr;

  delete _grid_back;
  _grid_back = nullptr;
}

} // namespace gos
