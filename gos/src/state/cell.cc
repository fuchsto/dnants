
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
  if (!a.is_alive()) {
    _state._taken  = false;
    _state._ant_id = { -1, -1 };
    return;
  }
  _state._ant_id = { a.team_id(), a.id() };
  _state._taken  = true;

  add_in_trace(
    a.team_id(),
    dir2or({ -a.dir().dx, -a.dir().dy }),
    gs.round_count() - 1);

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
  _state._taken  = false;
  _state._ant_id = { -1, -1 };

  if (a.is_alive()) {
    add_out_trace(
      a.team_id(),
      dir2or(a.dir()),
      gs.round_count() - 1);
  }
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
  ss << "traces: {";
  for (int team_id = 0; team_id < 2; ++team_id) {
    ss << " team " << team_id << ": { ";
    const auto & in_traces  = c.state().in_traces(team_id);
    const auto & out_traces = c.state().out_traces(team_id);
    ss << "in: ( ";
    for (int oidx = 0; oidx < 8; ++oidx) {
      int in_trace_val  = in_traces[oidx];
      if (in_trace_val > 0) {
        ss << oidx << ":" << in_trace_val << " ";
      }
    }
    ss << ") out: ( ";
    for (int oidx = 0; oidx < 8; ++oidx) {
      int out_trace_val = out_traces[oidx];
      if (out_trace_val > 0) {
        ss << oidx << ":" << out_trace_val << " ";
      }
    }
    ss << "} )";
  }
  ss << " }";
  ss << " }";
  return operator<<(os, ss.str());
}

} // namespace state
} // namespace gos
