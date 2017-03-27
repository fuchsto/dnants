
#include <gos/state/cell.h>
#include <gos/state/ant.h>

namespace gos {
namespace state {

void cell_state::on_enter(ant &) {
  _taken = true;
}

void cell_state::on_exit(ant &) {
  _taken = false;
}

void resource_cell_state::on_enter(ant & a) {
  cell_state::on_enter(a);
  if (_amount > 0) { --_amount; }
}

void resource_cell_state::on_exit(ant & a) {
  cell_state::on_exit(a);
}

} // namespace state
} // namespace gos
