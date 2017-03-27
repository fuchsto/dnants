
#include <gos/state/map_generator.h>

#include <gos/app_play_state.h>
#include <gos/state/cell.h>
#include <gos/util/logging.h>
#include <gos/util/timestamp.h>

#include <cmath>
#include <cstdlib>


namespace gos {
namespace state {

void map_generator::add_region(
       gos::state::grid      * grid_in,
       gos::point              region_center,
       gos::state::cell_type   type) {
  int region_ext = _num_cols / 4;
  int region_idx = 0;
  for (int rx = 0; rx < region_ext; ++rx) {
    for (int ry = 0; ry < region_ext; ++ry) {
      int cell_x = (region_center.x + rx) - (region_ext / 2);
      int cell_y = (region_center.y + ry) - (region_ext / 2);
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
      std::srand(gos::timestamp_ns() + (++region_idx + (region_ext * rx) + ry));
      double rnd = (static_cast<double>(std::rand()) / RAND_MAX);
      if (center_dist < rnd * 0.8) {
        grid_in->at(cell_x, cell_y) = gos::state::cell(type);
      }
    }
  }
  grid_in->at(region_center.x, region_center.y) = gos::state::cell(type);
}

gos::state::grid * map_generator::make_grid() {
  gos::state::grid * gen_grid = new gos::state::grid(
                                      _num_rows,
                                      _num_cols);
  int rnd_i;
  int region_idx = 0;
  for (int gr = 0; gr < _num_grass_regions; ++gr) {
    std::srand(gos::timestamp_ns() + gr + ++region_idx);
    rnd_i = std::rand();
    int  start_row  = 4 + (rnd_i % (_num_rows - 4));
    std::srand(gos::timestamp_ns() + gr + ++region_idx);
    rnd_i = std::rand();
    int  start_col  = 4 + (rnd_i % (_num_cols - 4));

    add_region(
        gen_grid,
        { start_row, start_col },
        gos::state::cell_type::grass);
  }
  for (int gr = 0; gr < _num_food_regions; ++gr) {
    std::srand(gos::timestamp_ns() + gr + ++region_idx);
    rnd_i = std::rand();
    int  start_row  = 4 + (rnd_i % (_num_rows - 4));
    std::srand(gos::timestamp_ns() + gr + ++region_idx);
    rnd_i = std::rand();
    int  start_col  = 4 + (rnd_i % (_num_cols - 4));

    add_region(
        gen_grid,
        { start_row, start_col },
        gos::state::cell_type::food);
  }
  return gen_grid;
}

} // namespace state
} // namespace gos
