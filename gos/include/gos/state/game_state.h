#ifndef GOS__STATE__GAME_STATE_H__INCLUDED
#define GOS__STATE__GAME_STATE_H__INCLUDED

#include <gos/app_engine.h>
#include <gos/types.h>



namespace gos {
namespace state {

class population;
class grid;

class game_state {
  app_engine       * _app           = nullptr;
  int                _nteams;
  extents            _grid_extents;
  size_t             _round_count   = 0;
  grid             * _grid_front    = nullptr;
  grid             * _grid_back     = nullptr;
  population       * _popul_front   = nullptr;
  population       * _popul_back    = nullptr;

 public:
  game_state(app_engine * app);
  game_state(app_engine * app, int nteams);
  ~game_state();

  /// Blocks until back state is ready, then swaps front and back state
  /// and performs asynchronous update of new back state.
  void next();

  size_t                   round_count() const { return _round_count;  }
  gos::state::grid       & grid_state()        { return *_grid_front;  }
  gos::state::population & population_state()  { return *_popul_front; }

};

} // namespace state
} // namespace gos

#endif // GOS__STATE__GAME_STATE_H__INCLUDED
