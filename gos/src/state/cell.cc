
#include <gos/state/cell.h>
#include <gos/state/ant.h>
#include <gos/state/game_state.h>

#include <iostream>
#include <sstream>


namespace gos {
namespace state {

void cell::enter(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  _state._ant_id = { a.team_id(), a.id() };
  _state._taken  = true;
  if (_state.num_food() > 0) {
    a.on_food_cell(state());
  }
  if (_state._type == cell_type::spawn_point) {
    a.on_home_cell(state());
  }
}

void cell::leave(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  _state._taken = false;
  if (a.is_alive()) {
    add_trace(
      a.team_id(),
      dir2or(a.dir()),
      gs.round_count() - 1);
  }
  _state._ant_id = { -1, -1 };
}

std::ostream & operator<<(
  std::ostream           & os,
  const gos::state::cell & c)
{
  std::ostringstream ss;
  ss << "cell { ";
  if (c.is_obstacle()) {
    ss << "obstacle ";
  }
  if (c.is_taken()) {
    ss << "ant:" << c.ant().id
       << "-t"   << c.ant().team_id << " ";
  }
  ss << "food:" << c.state().num_food() << " ";
  ss << "}";
  return operator<<(os, ss.str());
}

} // namespace state
} // namespace gos
