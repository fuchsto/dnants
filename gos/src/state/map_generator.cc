
#include <gos/state/map_generator.h>
#include <gos/state/population.h>
#include <gos/state/game_state.h>
#include <gos/state/cell.h>
#include <gos/types.h>

#include <gos/util/logging.h>
#include <gos/util/timestamp.h>
#include <gos/util/random.h>

#include <cmath>
#include <cstdlib>


namespace gos {
namespace state {

map_generator::map_generator(
  gos::state::game_state & g_state,
  extents                  ext)
: _game_state(g_state)
, _extents(ext)
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
        grid_in->set_cell_type({ cell_x, cell_y }, type);
      }
    }
  }
  grid_in->set_cell_type({ region_center.x, region_center.y }, type);
}

gos::state::grid * map_generator::make_grid()
{
  gos::state::grid * gen_grid = new gos::state::grid(_extents);

  int rnd_i;
  int region_idx = 0;
  GOS__LOG_DEBUG("map_generator.make_grid", "grass regions ...");
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
        { _extents.h / 2, _extents.w / 3 },
        gos::state::cell_type::grass);
  }
  GOS__LOG_DEBUG("map_generator.make_grid", "food regions ...");
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
        { _extents.w / 4, _extents.h / 4 },
        gos::state::cell_type::food);
  }

  GOS__LOG_DEBUG("map_generator.make_grid", "barriers ...");

  position max_excenter { _extents.w / 4,
                          _extents.h / 4 };
  int barrier_length = max_excenter.x;
  for (int br = 0; br < _num_barriers; ++br) {
    rnd_i = static_cast<int>(gos::random());
    gos::position  barrier_cell { _extents.w / 2,
                                  _extents.h / 2 };
    gos::direction barrier_dir  { (rnd_i % 3) - 1,
                                  (rnd_i % 3) - 1 };
    barrier_cell.x += (gos::random() % max_excenter.x) -
                      (max_excenter.x / 2);
    barrier_cell.y += (gos::random() % max_excenter.y) -
                      (max_excenter.y / 2);
    for (int bl = 0; bl < barrier_length; ++bl) {
      barrier_cell.x += barrier_dir.dx;
      barrier_cell.y += barrier_dir.dy;
      if (gen_grid->contains_position(barrier_cell)) {
        gen_grid->set_cell_type(barrier_cell, 
                                gos::state::cell_type::barrier);
      } else {
        rnd_i = static_cast<int>(gos::random());
        barrier_dir = { (rnd_i % 3) - 1,
                        (rnd_i % 3) - 1 };
        continue;
      }
      rnd_i = static_cast<int>(gos::random());
      if (bl % 3 == 0) {
        barrier_dir.dx = ((rnd_i / 2) % 3) - 1;
      } else if (bl % 5 == 0) {
        barrier_dir.dy = ((rnd_i * 7) % 3) - 1;
      }
    }
  }

  if (_symmetric) {
    for (int y = 0; y < _extents.h; ++y) {
      for (int x = 0; x < _extents.w - y; ++x) {
        position upper_pos { x, y };
        position lower_pos { _extents.w - x - 1, _extents.h - y - 1 };
        if ((*gen_grid)[upper_pos].type() != cell_type::spawn_point) {
          (*gen_grid)[lower_pos] = (*gen_grid)[upper_pos];
        }
      }
    }
  }

  return gen_grid;
}

gos::state::population * map_generator::make_population(
  gos::state::grid * grid_in)
{
  GOS__LOG_DEBUG("map_generator", "make_population()");
  std::vector<ant_team> teams;
  for (int team_idx = 0; team_idx < _num_teams; ++team_idx) {
    position spawn_point { 1, 1 };
    spawn_point.x += (team_idx % 2) * 3;
    spawn_point.y += (team_idx % 2) * 3;
    spawn_point.x *= _extents.w / 5;
    spawn_point.y *= _extents.h / 5;
    spawn_point.x -= 1;
    spawn_point.y -= 1;
    GOS__LOG_DEBUG("map_generator.make_population",
                   "team "  << team_idx << " " <<
                   "spawn:" << spawn_point.x << "," << spawn_point.y);
    teams.emplace_back(ant_team(team_idx,
                                _team_size,
                                spawn_point,
                                _game_state));
    grid_in->set_cell_type(
      spawn_point,
      cell_type::spawn_point);
  }
  gos::state::population * popul = new gos::state::population(
                                         std::move(teams));
  GOS__LOG_DEBUG("map_generator", "make_population >");
  return popul;
}

} // namespace state
} // namespace gos

