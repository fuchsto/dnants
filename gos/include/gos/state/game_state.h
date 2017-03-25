#ifndef GOS__STATE__GAME_STATE_H__INCLUDED
#define GOS__STATE__GAME_STATE_H__INCLUDED

#include <gos/state/grid.h>
#include <gos/state/population.h>


namespace gos {
namespace state {

class game_state {
  using grid_state       = gos::state::grid;
  using population_state = gos::state::population;

  grid_state       _grid;
  population_state _population;

 public:
  grid_state       & grid()       { return _grid;       }
  population_state & population() { return _population; }

};

} // namespace state
} // namespace gos

#endif // GOS__STATE__GAME_STATE_H__INCLUDED
