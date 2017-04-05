
#include <gos/default_rules.h>

#include <gos/state/ant_state.h>
#include <gos/util/logging.h>
#include <gos/types.h>

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/eval.h>
#include <pybind11/embedded.h>


namespace gos {

namespace py = pybind11;
using namespace pybind11::literals;


gos::state::ant_state py_update_ant(
  const gos::state::ant_state & current)
{
  GOS__LOG_DEBUG("py_update_ant", "()");
  
  GOS__LOG_DEBUG("py_update_ant", "load client ...");
  auto module = py::module::import("client");

  GOS__LOG_DEBUG("py_update_ant", "set locals ...");
  auto locals = py::dict(
                  "current"_a=current,
                  "next"_a=current,
                  **module.attr("__dict__"));

  GOS__LOG_DEBUG("py_update_ant", "eval ...");

  py::eval<py::eval_statements>(R"(
      next = update_state(current))", py::globals(), locals
  );

  GOS__LOG_DEBUG("py_update_ant", "->");

  // auto         py_state  = module.attr("Client")(current);
  // const auto & cpp_state = py_state.cast<const gos::state::ant_state &>();
  // return cpp_state;

  return locals["next"].cast<gos::state::ant_state>();
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

