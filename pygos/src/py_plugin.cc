
#include <pygos/py_plugin.h>

#include <gos/state/ant_state.h>

#include <pybind11/pybind11.h>
#include <pybind11/embedded.h>



namespace py = pybind11;

using namespace gos;
using namespace gos::state;



// PYBIND11_PLUGIN(pygos)
PYBIND11_ADD_EMBEDDED_MODULE(pygos)(py::module &m)
{
  // py::module m("pygos", "GOS client");

  py::class_<direction> direction_py(m, "direction");
  direction_py
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
           (void                 (ant_state::*)(direction))
                                 &ant_state::set_dir,
           "set direction for the next round")
    .def("set_turn",
           (void                 (ant_state::*)(int))
                                 &ant_state::set_turn,
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

#if 0
py::object import(
  const std::string & module,
  const std::string & path,
  py::object        & globals)
{
  py::dict locals;
  locals["module_name"] = py::cast(module);
  locals["path"]        = py::cast(path);

  py::eval<py::eval_statements>(
      "import imp\n"
      "new_module = imp.load_module("
                      "module_name, "
                      "open(path), path, "
                      "('py', 'U', imp.PY_SOURCE))\n",
      globals,
      locals);

  return locals["new_module"];
}
#endif

