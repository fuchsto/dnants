
#include <gos/state/cell.h>
#include <gos/state/ant.h>
#include <gos/state/game_state.h>

#include <iostream>
#include <sstream>


namespace gos {
namespace state {

void cell_state::on_enter(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  _ant_id = { a.team_id(), a.id() };
  _taken  = true;
}

void cell_state::on_exit(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  _taken = false;
  if (a.is_alive()) {
    add_trace(
      a.team().id(),
      dir2or(a.dir()),
      gs.round_count() - 1);
  }
  _ant_id = { -1, -1 };
}

void resource_cell_state::on_enter(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  cell_state::on_enter(a, gs);
  if (amount_left() > 0) {
    a.on_food_cell(*this);
  }
}

void resource_cell_state::on_exit(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  cell_state::on_exit(a, gs);
}

void food_cell_state::on_enter(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  resource_cell_state::on_enter(a, gs);
}

void spawn_cell_state::on_enter(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  resource_cell_state::on_enter(a, gs);
  a.on_home_cell(*this);
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
  if (c.type() == cell_type::food ||
      c.type() == cell_type::plain) {
    const gos::state::resource_cell_state * cs =
      reinterpret_cast<const gos::state::resource_cell_state *>(
        c.state());
    ss << "food:" << cs->amount_left() << " ";
  }
  ss << "}";
  return operator<<(os, ss.str());
}

} // namespace state
} // namespace gos
