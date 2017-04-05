#ifndef GOS__CLIENT_H__INCLUDED
#define GOS__CLIENT_H__INCLUDED

#include <gos/state/ant_state.h>

#include <pybind11/pybind11.h>
#include <pybind11/embedded.h>

#include <string>


namespace gos {

class client {
  std::string      _module_file;
  pybind11::module _module;

 public:
  client(int team_id);

  gos::state::ant_state callback(
    const gos::state::ant_state & current) const;

  gos::state::ant_state default_callback(
    const gos::state::ant_state & current) const;
};

} // namespace gos

#endif // GOS__CLIENT_H__INCLUDED
