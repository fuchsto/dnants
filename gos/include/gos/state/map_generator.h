#ifndef GOS__STATE__MAP_GENERATOR_H__INCLUDED
#define GOS__STATE__MAP_GENERATOR_H__INCLUDED

#include <gos/app_play_state.h>
#include <gos/state/cell.h>
#include <gos/util/logging.h>


namespace gos {
namespace state {

class map_generator
{
 private:
  typedef gos::state::map_generator self_t;

  int _num_rows          = 30;
  int _num_cols          = 30;
  int _num_grass_regions = 0;
  int _num_food_regions  = 0;
  int _num_teams         = 0;
 public:
  map_generator()        = delete;

  map_generator(int nrows, int ncols)
  : _num_rows(nrows)
  , _num_cols(ncols)
  { }

  self_t & set_num_grass_regions(int nregions) {
    _num_grass_regions = nregions;
    return *this;
  }

  self_t & set_num_food_regions(int nregions) {
    _num_food_regions = nregions;
    return *this;
  }

  self_t & set_num_teams(int nteams) {
    _num_teams = nteams;
    return *this;
  }

  gos::state::grid * make_grid();

 private:
  void add_region(
         gos::state::grid      * grid_in,
         gos::point              region_center,
         gos::state::cell_type   type);
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__MAP_GENERATOR_H__INCLUDED
