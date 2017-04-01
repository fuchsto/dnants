#ifndef GOS__STATE__MAP_GENERATOR_H__INCLUDED
#define GOS__STATE__MAP_GENERATOR_H__INCLUDED

#include <gos/state/cell.h>
#include <gos/state/grid.h>

#include <gos/util/logging.h>


namespace gos {
namespace state {

class population;
class game_state;
class grid;

class map_generator
{
 private:
  typedef gos::state::map_generator self_t;

  gos::state::game_state & _game_state;

  extents      _extents           { 80, 80 };
  int          _num_grass_regions = 0;
  int          _num_food_regions  = 0;
  int          _num_barriers      = 0;
  int          _num_teams         = 0;
  int          _team_size         = 0;

  struct map {
    gos::state::grid       * grid;
    gos::state::population * population;
  };

 public:
  map_generator()                 = delete;

  map_generator(
    gos::state::game_state & g_state,
    extents                  ext);

  self_t & set_num_grass_regions(int nregions) {
    _num_grass_regions = nregions;
    return *this;
  }

  self_t & set_num_food_regions(int nregions) {
    _num_food_regions = nregions;
    return *this;
  }

  self_t & set_num_barriers(int nbarriers) {
    _num_barriers = nbarriers;
    return *this;
  }

  self_t & set_num_teams(int nteams) {
    _num_teams = nteams;
    return *this;
  }

  self_t & set_team_size(int team_size) {
    _team_size = team_size;
    return *this;
  }

  map generate() {
    map gen_map;
    gen_map.grid       = make_grid();
    gen_map.population = make_population(gen_map.grid);
    return gen_map;
  }

 private:
  gos::state::grid       * make_grid();
  gos::state::population * make_population(gos::state::grid * grid_in);

  void add_region(
         gos::state::grid      * grid_in,
         gos::point              region_center,
         gos::extents            region_ext,
         gos::state::cell_type   type);
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__MAP_GENERATOR_H__INCLUDED
