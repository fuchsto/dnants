
#include <gos/app_play_state.h>
#include <gos/state/cell.h>
#include <gos/util/logging.h>

#include <cmath>
#include <cstdlib>


namespace gos {

app_play_state app_play_state::_instance;

void app_play_state::initialize(app_engine * app) {
  GOS__LOG_DEBUG("app_play_state", "initialize()");
  _app = app;
  _num_col_cells = app->settings().grid_cols;
  _num_row_cells = app->settings().grid_rows;
  _grid_spacing  = app->win().view_extents().w / _num_col_cells;
  // back and front game state:
  _grid_front = new gos::state::grid(
                  _num_row_cells,
                  _num_col_cells);
  _grid_back  = new gos::state::grid(
                  _num_row_cells,
                  _num_col_cells);
  // random initialization of the map:
  int  region_ext = _num_col_cells / 4;
  int  num_grass_blobs = 5;
  int  rnd_i;
  for (int cblob = 0; cblob < num_grass_blobs; ++cblob) {
    rnd_i = std::rand();
    int  start_row  = rnd_i % _num_row_cells;
    rnd_i = std::rand();
    int  start_col  = rnd_i % _num_col_cells;
    for (int rx = 0; rx < region_ext; ++rx) {
      for (int ry = 0; ry < region_ext; ++ry) {
        int cell_x = (start_col + rx) - (region_ext / 2);
        int cell_y = (start_row + ry) - (region_ext / 2);
        if (cell_x < 0 || cell_x > _num_col_cells ||
            cell_y < 0 || cell_y > _num_row_cells) {
          continue;
        }
        double center_dist_x = static_cast<double>(std::abs(start_col - rx))
                                 / (region_ext / 2);
        double center_dist_y = static_cast<double>(std::abs(start_row - ry))
                                 / (region_ext / 2);
        double center_dist   = std::sqrt(center_dist_x * center_dist_x +
                                        center_dist_y * center_dist_y)
                              / std::sqrt(2.0);
        GOS__LOG_DEBUG("app_play_state",
                       "initialize: (" << rx << "," << ry << ") " <<
                       "start: " << start_col << "," << start_row << " " <<
                       "cdist: " << center_dist_x << "," << center_dist_y <<
                       "-> "     << center_dist);
        std::srand(std::time(0));
        double rnd = (static_cast<double>(std::rand()) / RAND_MAX);
        if (center_dist < 0.7) {
          _grid_front->at(cell_x, cell_y) = gos::state::grass_cell();
        }
      }
    }
    _grid_front->at(start_col, start_row) = gos::state::grass_cell();
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
