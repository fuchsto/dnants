#ifndef GOS__STATE__CELL_H__INCLUDED
#define GOS__STATE__CELL_H__INCLUDED

#include <gos/state/ant.h>
#include <gos/state/cell_state.h>

#include <array>
#include <memory>
#include <iostream>


namespace gos {
namespace state {

/**
 * Single cell in the grid.
 *
 */
class cell {
  cell_state _state;

  friend cell_state;

 public:
  cell(cell_type ct = cell_type::plain)
  : _state(ct, (ct == cell_type::food ? 4 : 0))
  { }

  bool is_taken()    const { return _state.is_taken();            }
  bool is_obstacle() const { return type() == cell_type::barrier; }

  gos::state::ant_id ant()      const { return _state.ant();      }

  cell_type          type()     const { return _state.type();     }
  cell_state       & state()          { return _state;            }
  const cell_state & state()    const { return _state;            }

  void enter(gos::state::ant & a, const gos::state::game_state & gs);
  void leave(gos::state::ant & a, const gos::state::game_state & gs);

  void add_trace(
    int              team_id,
    gos::orientation ort,
    int              round_count) noexcept {
    _state._traces[team_id][or2int(ort)] = round_count;
  }
};

std::ostream & operator<<(
  std::ostream           & os,
  const gos::state::cell & c);

} // namespace state
} // namespace gos

#endif // GOS__STATE__CELL_H__INCLUDED
