
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
}

void resource_cell_state::on_exit(ant & a) {
  cell_state::on_exit(a);
}

void food_cell_state::on_enter(ant & a) {
  resource_cell_state::on_enter(a);
  a.on_food_cell(*this);
}

} // namespace state
} // namespace gos
