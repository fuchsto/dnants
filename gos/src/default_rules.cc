
#include <gos/default_rules.h>

#include <gos/state/ant_state.h>
#include <gos/types.h>

#include <pybind11/eval.h>


namespace gos {

namespace py = pybind11;

gos::state::ant_state py_update_ant(
  const gos::state::ant_state & current)
{
  // py::object scope
  //   = py::module::import("__main__").attr("__dict__");

  auto global
    = py::dict(py::module::import("__main__").attr("__dict__"));
  auto local
    = py::dict();

  gos::state::ant_state next
    = py::eval_file<py::eval_expr>("client.py", global, local)
        .cast<gos::state::ant_state>();

  return next;
}

gos::state::ant_state update_ant(
  const gos::state::ant_state & current)
{
  using gos::state::ant_state;

  gos::state::ant_state next = current;

  switch (current.mode) {
    case ant_state::ant_mode::scouting:
      if (current.events.enemy) {
        next.set_dir(current.enemy_dir);
        next.attack();
      } else if (current.events.collision) {
        int nturn = 1;
        if (current.tick_count - current.last_dir_change == 0) {
          nturn = 2;
        }
        // if (current.rand % 2) { nturn *= -1; }
        next.set_turn(nturn);
        next.move();
      } else if (current.events.food) {
        if (current.strength < 5) {
          next.eat();
          next.set_mode(ant_state::ant_mode::eating);
        } else {
          next.harvest();
          next.set_mode(ant_state::ant_mode::harvesting);
        }
      } else if (current.tick_count - current.last_dir_change > 4) {
        next.set_turn(
          ((next.rand + next.tick_count) % 3) - 1);
        next.move();
      }
      break;
    case ant_state::ant_mode::eating:
      if (current.events.enemy) {
        next.set_dir(current.enemy_dir);
        next.attack();
      } else if (current.events.food) {
        if (current.strength >= 5) {
          next.harvest();
          next.set_mode(ant_state::ant_mode::harvesting);
        } else {
          next.eat();
        }
      } else {
        if (current.num_carrying > 0) {
          next.set_mode(ant_state::ant_mode::tracing);
          next.backtrace();
        } else {
          next.set_mode(ant_state::ant_mode::scouting);
          next.move();
        }
      }
      break;
    case ant_state::ant_mode::harvesting:
      if (current.events.food) {
        if (current.num_carrying < current.strength) {
          next.harvest();
        } else {
          next.set_mode(ant_state::ant_mode::tracing);
          next.backtrace();
        }
      } else {
        if (current.num_carrying > 0) {
          next.set_mode(ant_state::ant_mode::tracing);
          next.backtrace();
        } else {
          next.set_mode(ant_state::ant_mode::scouting);
          next.move();
        }
      }
      break;
    case ant_state::ant_mode::tracing:
      if (current.num_carrying > 0) {
        next.backtrace();
      } else {
        next.set_mode(ant_state::ant_mode::scouting);
        next.move();
      }
      break;
    default:
      break;
  }

  return next;
}

} // namespace gos

