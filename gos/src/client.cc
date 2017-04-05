
#include <gos/client.h>

#include <gos/state/ant_state.h>
#include <gos/util/logging.h>
#include <gos/types.h>

#include <pybind11/pybind11.h>
#include <pybind11/embedded.h>

#include <sstream>
#include <string>


namespace py = pybind11;

using namespace pybind11::literals;

using namespace gos;
using namespace gos::state;

PYBIND11_ADD_EMBEDDED_MODULE(pygos)(py::module &m)
{
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

    // Attributes  ------------------------------------------------------
    //
    // Read-only
    .def_readonly("id",              &ant_state::id)
    .def_readonly("team_id",         &ant_state::team_id)
    .def_readonly("events",          &ant_state::events)
    .def_readonly("enemy_dir",       &ant_state::enemy_dir)
    .def_readonly("tick_count",      &ant_state::tick_count)
    .def_readonly("rand",            &ant_state::rand)
    .def_readonly("last_dir_change", &ant_state::last_dir_change)
    .def_readonly("num_carrying",    &ant_state::num_carrying)
    .def_readonly("strength",        &ant_state::strength)
    .def_readonly("damage",          &ant_state::damage)
    // Read-write
    .def_readwrite("mode",           &ant_state::mode)
    .def_readwrite("dir",            &ant_state::dir)
    .def_readwrite("action",         &ant_state::action)

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
}

namespace gos {

client::client(int team_id)
{
  std::ostringstream ss;
  ss << "client_" << team_id;

  _module_file = ss.str();
  _module      = py::module::import(_module_file.c_str());
}

gos::state::ant_state client::callback(
  const gos::state::ant_state & current) const
{
  auto locals = py::dict(
                  "current"_a=current,
                  **_module.attr("__dict__"));

  py::eval<py::eval_statements>(R"(
      next = update_state(current))", py::globals(), locals
  );

  return locals["next"].cast<const gos::state::ant_state &>();
}

gos::state::ant_state client::default_callback(
  const gos::state::ant_state & current) const
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

