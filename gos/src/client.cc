
#include <gos/client.h>

#include <gos/state/ant_state.h>
#include <gos/state/cell_state.h>
#include <gos/state/grid.h>

#include <gos/util/logging.h>
#include <gos/types.h>

#include <pybind11/pybind11.h>
#include <pybind11/embedded.h>
#include <pybind11/stl.h>

#include <sstream>
#include <string>
#include <array>


namespace py = pybind11;

using namespace pybind11::literals;

using namespace gos;
using namespace gos::state;

PYBIND11_ADD_EMBEDDED_MODULE(pygos)(py::module &m)
{
  py::class_<direction> direction_py(m, "direction");
  direction_py
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
           (void                 (ant_state::*)(ant_mode))
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
    ;

  py::class_<ant_state::state_events>(ant_state_py, "state_events")
    .def_readonly("collision",   &ant_state::state_events::collision)
    .def_readonly("attacked",    &ant_state::state_events::attacked)
    .def_readonly("enemy",       &ant_state::state_events::enemy)
    .def_readonly("food",        &ant_state::state_events::food)
    ;

  py::enum_<ant_mode>(m, "ant_mode")
    .value("waiting",      ant_mode::waiting)
    .value("scouting",     ant_mode::scouting)
    .value("backtracing",  ant_mode::backtracing)
    .value("eating",       ant_mode::eating)
    .value("harvesting",   ant_mode::harvesting)
    .value("dead",         ant_mode::dead)
    .export_values()
    ;

  py::enum_<ant_action>(m, "ant_action")
    .value("do_idle",      ant_action::do_idle)
    .value("do_move",      ant_action::do_move)
    .value("do_eat",       ant_action::do_eat)
    .value("do_harvest",   ant_action::do_harvest)
    .value("do_drop",      ant_action::do_drop)
    .value("do_attack",    ant_action::do_attack)
    .value("do_turn",      ant_action::do_turn)
    .export_values()
    ;

  py::class_<neighbor_grid> neighbor_grid_py(m, "neighbor_grid");
  neighbor_grid_py
    .def("at",
           (const cell_state & (neighbor_grid::*)(int,int))
                               &neighbor_grid::state_at,
           "access cell at grid coordinates")
    ;

  py::class_<cell_state> cell_state_py(m, "cell_state");
  cell_state_py
    // Attributes  ------------------------------------------------------
    //
    // Read-only
    .def_readonly("id",              &ant_state::id)
    //
    .def("is_taken",
           (bool                       (cell_state::*)(void))
                                       &cell_state::is_taken,
           "whether the cell is occupied by an ant")
    .def("is_obstacle",
           (bool                       (cell_state::*)(void))
                                       &cell_state::is_obstacle,
           "whether the cell is an obstacle")
    .def("ant",
           (ant_id                     (cell_state::*)(void))
                                       &cell_state::ant,
           "identifier of ant in cell")
    .def("team_traces",
           (const cell_state::traces & (cell_state::*)(int))
                                       &cell_state::team_traces,
           "trace directions of specified team")
    .def("num_food",
           (int                        (cell_state::*)(void))
                                       &cell_state::num_food,
           "amount of food in the cell")
    .def("take_food",
           (int                        (cell_state::*)(void))
                                       &cell_state::take_food,
           "harvest one food unit from the cell")
    .def("drop_food",
           (void                       (cell_state::*)(int))
                                       &cell_state::drop_food,
           "drop food in the cell")
    .def("type",
           (cell_type                  (cell_state::*)(void))
                                       &cell_state::type,
           "type of the cell")
    ;

  py::class_<cell_state::trace>(cell_state_py, "trace")
    .def_readonly("intensity",         &cell_state::trace::intensity)
    .def_readonly("last_visit",        &cell_state::trace::last_visit)
    ;
}

namespace gos {

client::client(int team_id)
{
  std::ostringstream ss;
  ss << "client_" << team_id;

  _module_file = ss.str();

  GOS__LOG_DEBUG("client", "loading client module " << _module_file);

  _module      = py::module::import(_module_file.c_str());
}

gos::state::ant_state client::callback(
  const gos::state::ant_state & current,
  const gos::state::grid      & grid_state) const
{
  auto locals = py::dict(
                  "current"_a=current,
                  "grid_state"_a=neighbor_grid(grid_state, current.pos),
                  **_module.attr("__dict__"));

  return py::eval<py::eval_expr>(
           R"(update_state(current,grid_state))", py::globals(), locals
         ).cast<const gos::state::ant_state &>();
}

} // namespace gos

