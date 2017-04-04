
#include <pygos/py_plugin.h>

#include <gos/state/ant_state.h>

#include <pybind11/pybind11.h>



namespace py = pybind11;

using namespace gos;
using namespace gos::state;



PYBIND11_PLUGIN(pygos)
{
  py::module m("pygos", "GOS client");

  py::class_<gos::state::ant_state> ant_state_py(m, "ant_state");
  ant_state_py
  //.def(py::init<gos::state::ant_team &, int, const gos::position &>())

    // Read-only --------------------------------------------------------
    //
    .def_readonly("id",           &ant_state::id)
    .def_readonly("team_id",      &ant_state::team_id)
    .def_readonly("mode",         &ant_state::mode)
    .def_readonly("strength",     &ant_state::strength)
    .def_readonly("num_carrying", &ant_state::num_carrying)
    .def_readonly("damage",       &ant_state::damage)
    .def_readonly("event",        &ant_state::event)
    .def_readonly("dir",          &ant_state::dir)

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

  py::enum_<gos::state::ant_state::ant_mode>(ant_state_py, "ant_mode")
    .value("waiting",      gos::state::ant_state::ant_mode::waiting)
    .value("detour",       gos::state::ant_state::ant_mode::detour)
    .value("scouting",     gos::state::ant_state::ant_mode::scouting)
    .value("fighting",     gos::state::ant_state::ant_mode::fighting)
    .value("tracing",      gos::state::ant_state::ant_mode::tracing)
    .value("eating",       gos::state::ant_state::ant_mode::eating)
    .value("harvesting",   gos::state::ant_state::ant_mode::harvesting)
    .value("dead",         gos::state::ant_state::ant_mode::dead)
    ;

  py::enum_<gos::state::ant_state::ant_event>(ant_state_py, "ant_event")
    .value("none",         gos::state::ant_state::ant_event::none)
    .value("food",         gos::state::ant_state::ant_event::food)
    .value("enemy",        gos::state::ant_state::ant_event::enemy)
    .value("collision",    gos::state::ant_state::ant_event::collision)
    .value("attacked",     gos::state::ant_state::ant_event::attacked)
    ;

  py::enum_<gos::state::ant_state::ant_action>(ant_state_py, "ant_action")
    .value("do_idle",      gos::state::ant_state::ant_action::do_idle)
    .value("do_move",      gos::state::ant_state::ant_action::do_move)
    .value("do_backtrace", gos::state::ant_state::ant_action::do_backtrace)
    .value("do_eat",       gos::state::ant_state::ant_action::do_eat)
    .value("do_harvest",   gos::state::ant_state::ant_action::do_harvest)
    .value("do_drop",      gos::state::ant_state::ant_action::do_drop)
    .value("do_attack",    gos::state::ant_state::ant_action::do_attack)
    .value("do_turn",      gos::state::ant_state::ant_action::do_turn)
    ;

  return m.ptr();
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

