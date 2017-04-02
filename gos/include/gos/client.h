#ifndef GOS__CLIENT_H__INCLUDED
#define GOS__CLIENT_H__INCLUDED

#include <gos/state/client_state.h>

#include <pybind11/pybind11.h>


namespace gos {

class client {

 public:
   client() {
   }

   void update_state(gos::state::client_state & state);
};

} // namespace

#endif // GOS__CLIENT_H__INCLUDED
