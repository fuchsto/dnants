
#include <gos/state/ant.h>
#include <gos/state/client_state.h>

#include <pybind11/pybind11.h>



namespace py = pybind11;

using namespace gos;
using namespace gos::state;



PYBIND11_PLUGIN(gos_client)
{
  py::module m("gos_client", "GOS client");

  py::class_<gos::state::ant> ant_py(m, "ant");
  ant_py
    .def(py::init<gos::state::ant_team &, int, const gos::position &>())
    .def("id",             (int        (ant::*)(void))      &ant::id,
         "unique identifier within the ant's team")
    .def("team_id",        (int        (ant::*)(void))      &ant::team_id,
         "the ant's team id")
    .def("ant_mode",       (ant::mode  (ant::*)(void))      &ant::ant_mode,
         "current mode")
    .def("switch_mode",    (void       (ant::*)(ant::mode)) &ant::switch_mode,
         "changes mode for the next round")
    .def("strength()",     (int        (ant::*)(void))      &ant::strength,
         "the ant's current strength")
    .def("num_carrying()", (int        (ant::*)(void))      &ant::strength,
         "number of food units carried by the ant")
    .def("evt()",          (ant::event (ant::*)(void))      &ant::evt,
         "number of food units carried by the ant")
    .def("dir()",          (direction  (ant::*)(void))      &ant::dir,
         "number of food units carried by the ant")
    .def("damage()",       (int        (ant::*)(void))      &ant::damage,
         "total damage taken in this round")
    ;

  py::enum_<gos::state::ant::mode>(ant_py, "mode")
    .value("waiting",      gos::state::ant::mode::waiting)
    .value("detour",       gos::state::ant::mode::detour)
    .value("scouting",     gos::state::ant::mode::scouting)
    .value("fighting",     gos::state::ant::mode::fighting)
    .value("tracing",      gos::state::ant::mode::tracing)
    .value("eating",       gos::state::ant::mode::eating)
    .value("harvesting",   gos::state::ant::mode::harvesting)
    ;

  py::enum_<gos::state::ant::event>(ant_py, "event")
    .value("none",         gos::state::ant::event::none)
    .value("food",         gos::state::ant::event::food)
    .value("enemy",        gos::state::ant::event::enemy)
    .value("collision",    gos::state::ant::event::collision)
    .value("attacked",     gos::state::ant::event::attacked)
    ;

  py::enum_<gos::state::ant::action>(ant_py, "action")
    .value("do_idle",      gos::state::ant::action::do_idle)
    .value("do_move",      gos::state::ant::action::do_move)
    .value("do_backtrace", gos::state::ant::action::do_backtrace)
    .value("do_eat",       gos::state::ant::action::do_eat)
    .value("do_harvest",   gos::state::ant::action::do_harvest)
    .value("do_attack",    gos::state::ant::action::do_attack)
    .value("do_turn",      gos::state::ant::action::do_turn)
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

