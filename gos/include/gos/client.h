#ifndef GOS__CLIENT_H__INCLUDED
#define GOS__CLIENT_H__INCLUDED

#include <gos/state/ant_state.h>

#include <pybind11/pybind11.h>
#include <pybind11/embedded.h>

#include <string>


namespace gos {

namespace state {
  class grid;
}

class client {
  std::string              _module_file;
  pybind11::module         _module;

 public:
  client()                               = delete;
  client(const client & other)           = delete;
  client & operator=(const client & rhs) = delete;
  client(client && other)                = default;
  client & operator=(client && rhs)      = default;

  client(int team_id, const std::string & module_name);

  gos::state::ant_state callback(
    const gos::state::ant_state & current,
    const gos::state::grid      & grid_state) const;
};

} // namespace gos

#endif // GOS__CLIENT_H__INCLUDED
