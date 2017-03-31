
#include <gos/state/game_state.h>
#include <gos/state/population.h>
#include <gos/state/grid.h>
#include <gos/state/map_generator.h>


namespace gos {
namespace state {

game_state::game_state(
  app_engine * app)
: game_state(app, 2)
{ }

game_state::game_state(
  app_engine * app,
  int          nteams)
: _app(app)
, _nteams(nteams)
{
  GOS__LOG_DEBUG("game_state", "game_state()");
  _grid_extents  = app->settings().grid_extents;

  GOS__LOG_DEBUG("game_state", "map_gen ...");
  map_generator map_gen(_grid_extents);
  map_gen.set_num_grass_regions(4)
         .set_num_food_regions(8)
         .set_num_barriers(8)
         .set_num_teams(_nteams);

  // back and front game state:
  GOS__LOG_DEBUG("game_state", "map_gen.make_grid ...");
  _grid_front    = map_gen.make_grid(
                     *this);
//_grid_back     = new grid(*_grid_front);
  GOS__LOG_DEBUG("game_state", "map_gen.make_population ...");
  _popul_front   = map_gen.make_population(
                     *this, app->settings().init_team_size);
//_popul_back    = new population(*_popul_front);
  GOS__LOG_DEBUG("game_state", "game_state >");
}

game_state::~game_state() {
  delete _grid_front;
  _grid_front  = nullptr;
  delete _grid_back;
  _grid_back   = nullptr;
  delete _popul_front;
  _popul_front = nullptr;
  delete _popul_back;
  _popul_back  = nullptr;
}

void game_state::next() {
  ++_round_count;
  population_state().update();
}

} // namespace gos
} // namespace state

