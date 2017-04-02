#ifndef GOS__STATE_CLIENT_STATE_H__INCLUDED
#define GOS__STATE_CLIENT_STATE_H__INCLUDED

#include <gos/state/ant.h>
#include <gos/state/game_state.h>
#include <gos/state/grid.h>
#include <gos/state/cell.h>


namespace gos {
namespace state {

class client_state {

  gos::state::ant  * _ant;
  gos::state::cell * _cell;

  gos::state::ant::action _req_action;

 public:
   client_state(gos::state::ant & a)
   : _ant(&a)
   , _cell(&a.cell())
   { }

};

} // namespace gos
} // namespace state

#endif // GOS__STATE_CLIENT_STATE_H__INCLUDED
