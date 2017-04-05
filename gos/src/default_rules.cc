
#include <gos/default_rules.h>

#include <gos/state/ant_state.h>
#include <gos/util/logging.h>
#include <gos/types.h>

// #include <pygos/py_plugin.h>

#include <pybind11/pybind11.h>
#include <pybind11/embedded.h>



namespace py = pybind11;

using namespace pybind11::literals;

using namespace gos;
using namespace gos::state;

// PYBIND11_PLUGIN(pygos)
PYBIND11_ADD_EMBEDDED_MODULE(pygos)(py::module &m)
{
  // py::module m("pygos", "GOS client");

  py::class_<direction> direction_py(m, "direction");
  direction_py
  //.def(py::init<int, int>())
    .def(py::init<>())
    .def_readwrite("dx", &direction::dx)
    .def_readwrite("dy", &direction::dy)
    ;

  py::class_<ant_state> ant_state_py(m, "ant_state");
  ant_state_py
    // Constructor
    //
    .def(py::init<>())

    // Read-only --------------------------------------------------------
    //
    // Attributes
    .def_readonly("id",           &ant_state::id)
    .def_readonly("team_id",      &ant_state::team_id)
    .def_readonly("mode",         &ant_state::mode)
    .def_readonly("strength",     &ant_state::strength)
    .def_readonly("num_carrying", &ant_state::num_carrying)
    .def_readonly("damage",       &ant_state::damage)
    .def_readonly("dir",          &ant_state::dir)
    // Events
    .def_readonly("events",       &ant_state::events)

    // Modifiers --------------------------------------------------------
    //
    .def("set_mode",
           (void                 (ant_state::*)(ant_state::ant_mode))
                                 &ant_state::set_mode,
           "set mode for the next round")
    .def("set_dir",
           (void                 (ant_state::*)(int, int))
                                 &ant_state::set_dir,
           "set direction for the next round")
    .def("turn_dir",
           (void                 (ant_state::*)(int))
                                 &ant_state::turn_dir,
           "turn direction for the next round")

    // Actions ----------------------------------------------------------
    //
    .def("move",
           (void                 (ant_state::*)(void))
                                 &ant_state::move,
           "move action")
    .def("attack",
           (void                 (ant_state::*)(void))
                                 &ant_state::attack,
           "attack action")
    .def("eat",
           (void                 (ant_state::*)(void))
                                 &ant_state::eat,
           "eat action")
    .def("harvest",
           (void                 (ant_state::*)(void))
                                 &ant_state::harvest,
           "harvest action")
    .def("drop",
           (void                 (ant_state::*)(void))
                                 &ant_state::drop,
           "drop action")
    .def("backtrace",
           (void                 (ant_state::*)(void))
                                 &ant_state::backtrace,
           "backtrace action")
    ;

  py::class_<ant_state::state_events>(ant_state_py, "state_events")
    .def_readonly("collision",   &ant_state::state_events::collision)
    .def_readonly("attacked",    &ant_state::state_events::attacked)
    .def_readonly("enemy",       &ant_state::state_events::enemy)
    .def_readonly("food",        &ant_state::state_events::food)
    ;

  py::enum_<ant_state::ant_mode>(ant_state_py, "ant_mode")
    .value("waiting",      ant_state::ant_mode::waiting)
    .value("detour",       ant_state::ant_mode::detour)
    .value("scouting",     ant_state::ant_mode::scouting)
    .value("tracing",      ant_state::ant_mode::tracing)
    .value("eating",       ant_state::ant_mode::eating)
    .value("harvesting",   ant_state::ant_mode::harvesting)
    .value("dead",         ant_state::ant_mode::dead)
    .export_values()
    ;

  py::enum_<ant_state::ant_action>(ant_state_py, "ant_action")
    .value("do_idle",      ant_state::ant_action::do_idle)
    .value("do_move",      ant_state::ant_action::do_move)
    .value("do_backtrace", ant_state::ant_action::do_backtrace)
    .value("do_eat",       ant_state::ant_action::do_eat)
    .value("do_harvest",   ant_state::ant_action::do_harvest)
    .value("do_drop",      ant_state::ant_action::do_drop)
    .value("do_attack",    ant_state::ant_action::do_attack)
    .value("do_turn",      ant_state::ant_action::do_turn)
    .export_values()
    ;

  // return m.ptr();
}

namespace gos {

gos::state::ant_state client_callback(
  const gos::state::ant_state & current)
{
  GOS__LOG_DEBUG("client_callback", "()");
  
  GOS__LOG_DEBUG("client_callback", "load client ...");
  auto module = py::module::import("client");

  GOS__LOG_DEBUG("client_callback", "set locals ...");
  auto locals = py::dict(
                  "current"_a=current,
                  "next"_a=current,
                  **module.attr("__dict__"));

  GOS__LOG_DEBUG("client_callback", "eval ...");

  py::eval<py::eval_statements>(R"(
      next = update_state(current))", py::globals(), locals
  );

  GOS__LOG_DEBUG("client_callback", ">");
  return locals["next"].cast<gos::state::ant_state>();
}

gos::state::ant_state py_update_ant(
  const gos::state::ant_state & current)
{
  return gos::client_callback(current);
}

gos::state::ant_state update_ant(
  const gos::state::ant_state & current)
{
  using gos::state::ant_state;

  gos::state::ant_state next = current;

  switch (current.mode) {
    case ant_state::ant_mode::scouting:
      if (current.events.enemy) {
        next.set_dir(current.enemy_dir.dx, current.enemy_dir.dy);
        next.attack();
      } else if (current.events.collision) {
        int nturn = 1;
        if (current.tick_count - current.last_dir_change == 0) {
          nturn = 2;
        }
        // if (current.rand % 2) { nturn *= -1; }
        next.turn_dir(nturn);
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
        next.turn_dir(
          ((next.rand + next.tick_count) % 3) - 1);
        next.move();
      }
      break;
    case ant_state::ant_mode::eating:
      if (current.events.enemy) {
        next.set_dir(current.enemy_dir.dx, current.enemy_dir.dy);
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

