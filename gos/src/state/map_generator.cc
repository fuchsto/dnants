
#include <gos/state/map_generator.h>

#include <gos/app_play_state.h>
#include <gos/state/cell.h>
#include <gos/util/logging.h>

#include <cmath>
#include <cstdlib>


namespace gos {
namespace state {

gos::state::grid * map_generator::make_grid() {
  gos::state::grid * gen_grid = new gos::state::grid(
                                      _num_rows,
                                      _num_cols);
  int  region_ext      = _num_cols / 4;
  int  num_grass_blobs = _num_grass_regions;
  int  rnd_i;
  for (int cblob = 0; cblob < num_grass_blobs; ++cblob) {
    std::srand(std::time(0) + cblob);
    rnd_i = std::rand();
    int  start_row  = 4 + (rnd_i % (_num_rows - 4));
    rnd_i = std::rand();
    int  start_col  = 4 + (rnd_i % (_num_cols - 4));
    for (int rx = 0; rx < region_ext; ++rx) {
      for (int ry = 0; ry < region_ext; ++ry) {
        int cell_x = (start_col + rx) - (region_ext / 2);
        int cell_y = (start_row + ry) - (region_ext / 2);
        if (cell_x < 0 || cell_x >= _num_cols ||
            cell_y < 0 || cell_y >= _num_rows) {
          continue;
        }
        double center_dist_x = static_cast<double>(
                                 std::abs(rx - region_ext / 2)
                               ) / (region_ext / 2);
        double center_dist_y = static_cast<double>(
                                 std::abs(ry - region_ext / 2)
                               ) / (region_ext / 2);
        double center_dist   = std::sqrt(center_dist_x * center_dist_x +
                                         center_dist_y * center_dist_y)
                               / std::sqrt(2.0);
        std::srand(std::time(0) + ry);
        double rnd = (static_cast<double>(std::rand()) / RAND_MAX);
        if (center_dist < 0.3 * rnd) {
          gen_grid->at(cell_x, cell_y) = gos::state::grass_cell();
        } else if (center_dist * rnd < 0.5) {
          gen_grid->at(cell_x, cell_y) = gos::state::food_cell();
        }
      }
    }
    gen_grid->at(start_col, start_row) = gos::state::grass_cell();
  }
  return gen_grid;
}

} // namespace state
} // namespace gos
