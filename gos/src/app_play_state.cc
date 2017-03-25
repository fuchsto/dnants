
#include <gos/app_play_state.h>
#include <gos/state/cell.h>
#include <gos/util/logging.h>


namespace gos {

app_play_state app_play_state::_instance;

void app_play_state::initialize(app_engine * app) {
  GOS__LOG_DEBUG("app_play_state", "initialize()");
  _app = app;
  // back and front game state:
  _grid_front = new gos::state::grid(
                  _num_row_cells,
                  _num_col_cells);
  _grid_back  = new gos::state::grid(
                  _num_row_cells,
                  _num_col_cells);
  // random initialization of the map:
  auto rnd       = this->get_msecs();
  int  start_row = rnd % _num_row_cells;
  int  start_col = rnd % _num_col_cells;
  for (int ph = 0; ph < 20; ++ph) {
    int radius = ph / 8; // number of cells in von-Neumann neighborhood
    int phase  = ph % 8;
    int cell_x = start_col + (phase % 3);
    int cell_y = start_row + (phase % 3);
    cell_x = std::max(cell_x, 0);
    cell_x = std::min(cell_x, _num_col_cells - 1);
    cell_y = std::max(cell_y, 0);
    cell_y = std::min(cell_y, _num_row_cells - 1);

    GOS__LOG_DEBUG("app_play_state",
                   "grid.at(" << cell_x << "," << cell_y << ")");
    _grid_front->at(cell_x, cell_y) = gos::state::grass_cell();
  }
  GOS__LOG_DEBUG("app_play_state", "initialize >");
}

void app_play_state::finalize() {
  delete _grid_front;
  _grid_front = nullptr;

  delete _grid_back;
  _grid_back = nullptr;
}

} // namespace gos
