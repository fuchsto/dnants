
#include <gos/state/map_generator.h>
#include <gos/state/game_state.h>
#include <gos/state/cell.h>
#include <gos/types.h>

#include <gos/util/logging.h>
#include <gos/util/timestamp.h>

#include <gos/app_play_state.h>

#include <cmath>
#include <cstdlib>


namespace gos {
namespace state {

map_generator::map_generator(extents ext)
: _extents(ext)
{ }

void map_generator::add_region(
       gos::state::grid      * grid_in,
       gos::point              region_center,
       gos::extents            region_ext,
       gos::state::cell_type   type) {
  int region_idx = 0;
  for (int rx = 0; rx < region_ext.w; ++rx) {
    for (int ry = 0; ry < region_ext.h; ++ry) {
      int cell_x = (region_center.x + rx) - (region_ext.w / 2);
      int cell_y = (region_center.y + ry) - (region_ext.h / 2);
      if (cell_x < 0 || cell_x >= _extents.w ||
          cell_y < 0 || cell_y >= _extents.h) {
        continue;
      }
      double center_dist_x = static_cast<double>(
                               std::abs(rx - region_ext.w / 2)
                             ) / (region_ext.w / 2);
      double center_dist_y = static_cast<double>(
                               std::abs(ry - region_ext.h / 2)
                             ) / (region_ext.h / 2);
      double center_dist   = std::sqrt(center_dist_x * center_dist_x +
                                       center_dist_y * center_dist_y)
                             / std::sqrt(2.0);
      std::srand(gos::timestamp_ns() +
                 ++region_idx +
                 (region_ext.w * rx) + ry);
      double rnd = (static_cast<double>(std::rand()) / RAND_MAX);
      if (center_dist < rnd * 0.8) {
        grid_in->at(cell_x, cell_y) = gos::state::cell(type);
      }
    }
  }
  grid_in->at(region_center.x, region_center.y) = gos::state::cell(type);
}

gos::state::grid * map_generator::make_grid(
  gos::state::game_state & g_state) {
  gos::state::grid * gen_grid = new gos::state::grid(
                                      _extents.w,
                                      _extents.h);
  int rnd_i;
  int region_idx = 0;
  for (int gr = 0; gr < _num_grass_regions; ++gr) {
    std::srand(gos::timestamp_ns() + gr + ++region_idx);
    rnd_i = std::rand();
    int  start_row  = 4 + (rnd_i % (_extents.w - 4));
    std::srand(gos::timestamp_ns() + gr + ++region_idx);
    rnd_i = std::rand();
    int  start_col  = 4 + (rnd_i % (_extents.h - 4));

    add_region(
        gen_grid,
        { start_row,      start_col      },
        { _extents.h / 4, _extents.w / 3 },
        gos::state::cell_type::grass);
  }
  for (int gr = 0; gr < _num_food_regions; ++gr) {
    std::srand(gos::timestamp_ns() + gr + ++region_idx);
    rnd_i = std::rand();
    int  start_row  = 4 + (rnd_i % (_extents.h - 4));
    std::srand(gos::timestamp_ns() + gr + ++region_idx);
    rnd_i = std::rand();
    int  start_col  = 4 + (rnd_i % (_extents.w - 4));

    add_region(
        gen_grid,
        { start_row,      start_col      },
        { _extents.w / 2, _extents.h / 2 },
        gos::state::cell_type::food);
  }
  return gen_grid;
}

gos::state::population * map_generator::make_population(
  gos::state::game_state & g_state) {
  std::vector<ant_team> teams;
  for (int team_idx = 0; team_idx < _num_teams; ++team_idx) {
    position spawn_point {
               _extents.w / 10,
               _extents.h / 10 };
    teams.push_back(ant_team(team_idx,
                             spawn_point,
                             g_state));
  }
  gos::state::population * popul = new gos::state::population(
                                         std::move(teams));
  return popul;
}

} // namespace state
} // namespace gos

