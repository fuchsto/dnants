
#include <gos/state/cell.h>
#include <gos/state/ant.h>
#include <gos/state/game_state.h>

namespace gos {
namespace state {

void cell_state::on_enter(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  _taken = true;
  _ant   = &a;
}

void cell_state::on_exit(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  if (a.is_alive()) {
    add_trace(
      a.team().id(),
      dir2or(a.dir()),
      gs.round_count() - 1);
  }
  _taken = false;
  _ant   = nullptr;
}

void resource_cell_state::on_enter(
  gos::state::ant              & a,
  const gos::state::game_state & gs)
{
  cell_state::on_enter(a, gs);
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
  a.on_food_cell(*this);
}

} // namespace state
} // namespace gos
