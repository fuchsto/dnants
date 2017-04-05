#ifndef PYGOS__PY_PLUGIN_H__INCLUDED
#define PYGOS__PY_PLUGIN_H__INCLUDED

#include <gos/state/ant_state.h>

namespace gos {

gos::state::ant_state client_callback(
  const gos::state::ant_state & current);

}

#endif // PYGOS__PY_PLUGIN_H__INCLUDED
