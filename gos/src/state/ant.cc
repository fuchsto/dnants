
#include <gos/state/ant.h>

#include <gos/state/ant_state.h>
#include <gos/state/game_state.h>
#include <gos/state/grid.h>
#include <gos/state/cell.h>
#include <gos/state/population.h>

#include <gos/types.h>
#include <gos/util/random.h>

#include <iostream>
#include <sstream>


namespace gos {
namespace state {

void ant_team::update() {
  if (_game_state->round_count() % 20 == 0) {
    spawn_ants();
  }
}

ant & ant_team::add_ant_at(position && pos) {
  _ants.emplace_back(ant(*this, _ants.size(), std::move(pos)));
  return _ants.back();
}

ant & ant_team::add_ant_at(const position & pos) {
  _ants.emplace_back(ant(*this, _ants.size(), pos));
  return _ants.back();
}

void ant_team::spawn_ants() {
  for (auto & spawn_pos : _spawn_points) {
    auto & base_cell  = _game_state->grid_state()[spawn_pos];
    auto   base_state = reinterpret_cast<spawn_cell_state *>(
                          base_cell.state());
    if (base_state->amount_left() > 0) {
      _team_size += base_state->consume();
    }
    if (!(base_cell.is_taken())) {
      if (_ants.size() >= _team_size) { return; }
      add_ant_at(spawn_pos);
    }
  }
}



void ant::on_home_cell(gos::state::spawn_cell_state & home_cell) noexcept {
  GOS__LOG_DEBUG("ant.on_home_cell", "delivered " << num_carrying());
  home_cell.drop(num_carrying());
  _state.num_carrying = 0;
  switch_mode(ant_state::ant_mode::scouting);
}

void ant::on_food_cell(gos::state::resource_cell_state & food_cell) noexcept {
  _state.event = ant_state::ant_event::food;
  // Client code
  if (mode() == ant_state::ant_mode::harvesting) {
    return;
  }
  if (strength() < max_strength()) {
    switch_mode(ant_state::ant_mode::eating);
  } else {
    switch_mode(ant_state::ant_mode::harvesting);
  }
}

void ant::on_enemy(
  gos::state::ant & enemy) noexcept {
  _state.event = ant_state::ant_event::enemy;
  // Client code
  if (strength() > 3) {
    attack(enemy);
  } else {
    // too weak, run away
    turn(4);
    if (mode() == ant_state::ant_mode::fighting) {
      switch_mode(ant_state::ant_mode::scouting);
    }
  }
  if (mode() == ant_state::ant_mode::fighting && !enemy.is_alive()) {
    switch_mode(ant_state::ant_mode::scouting);
  }
}

void ant::on_attacked(gos::state::ant & enemy) noexcept {
  _state.event = ant_state::ant_event::attacked;
  // Client code
  GOS__LOG_DEBUG("ant.on_attacked", "enemy: " <<
                 "t:"  << enemy.team_id() << " " <<
                 "id:" << enemy.id());
  _state.damage += enemy.strength();
}

void ant::on_collision() noexcept {
  _state.event = ant_state::ant_event::collision;
  // Client code
  if (rand() % 6 <= 2) { turn(-1); }
  else                { turn(1);  }
}



void ant::attack(gos::state::ant & enemy) noexcept {
  GOS__LOG_DEBUG("ant.attack", "enemy: " <<
                 "t:"  << enemy.team_id() << " " <<
                 "id:" << enemy.id());
  if (num_carrying() > 0) {
    return;
  }
  switch_mode(ant_state::ant_mode::fighting);
  enemy.on_attacked(*this);
}

void ant::die() noexcept {
  GOS__LOG_DEBUG("ant.die", "ant died: " <<
                 "t:"   << team_id() << " " <<
                 "id:"  << id() << " " <<
                 "at (" << pos().x << "," << pos().y << ")");
  _state.mode = ant_state::ant_mode::dead;
  this->game_state().grid_state()[_state.pos]
                    .leave(*this, this->game_state());
}

void ant::update_position() noexcept {
  if (!is_alive()) { return; }
  auto rc       = this->game_state().round_count();
  _state.rand   = gos::random();
  _state.damage = 0;
  ++_state.nticks_not_fed;
  switch (_state.mode) {
    case ant_state::ant_mode::eating:
      eat();
      break;
    case ant_state::ant_mode::harvesting:
      harvest();
      break;
    case ant_state::ant_mode::tracing:
      trace_back();
      move();
      break;
    case ant_state::ant_mode::scouting:
      if (num_no_dir_change() > 4) {
        turn(((_state.rand + rc * 7) % 3) - 1);
      }
      move();
      break;
    default:
      break;
  }
  if (_state.strength > max_strength() / 2 &&
      ((_state.nticks_not_fed + 1) % 100) == 0) {
    --_state.strength;
  }
}

void ant::update_action() noexcept {
  if (!is_alive())       { return; }
  // Cannot attack while carrying:
  gos::state::ant_id enemy_id  { -1, -1 };
  for (int y = -1; enemy_id.id == -1 && y <= 1; ++y) {
    for (int x = -1; enemy_id.id == -1 && x <= 1; ++x) {
      position adj_pos { pos().x + x,
                         pos().y + y };
      if ((x == 0 && y == 0) ||
          !this->game_state().grid_state().contains_position(adj_pos)) {
        continue;
      }
      enemy_id = this->game_state().grid_state()[adj_pos].ant();
      if (enemy_id.id != -1 &&
          enemy_id.team_id != team_id()) {
        return on_enemy(this->game_state().population_state()
                                          .teams()[enemy_id.team_id]
                                          .ants()[enemy_id.id]);
      }
    }
  }
  // No enemy in range
  if (_state.mode == ant_state::ant_mode::fighting) {
    switch_mode(ant_state::ant_mode::scouting);
  }
}

void ant::update_reaction() noexcept {
  if (!is_alive()) { return; }
  if (strength() <= damage()) {
    if (strength() > 1) {
      --_state.strength;
    } else {
      die();
    }
    if (num_carrying() > 0) {
      gos::state::cell & pos_cell = this->game_state().grid_state()[pos()];
      auto pos_cell_state = reinterpret_cast<resource_cell_state *>(
                              pos_cell.state());
      pos_cell_state->drop(num_carrying());
      _state.num_carrying = 0;
    }
  }
}

void ant::eat() noexcept {
  if (!is_alive()) { return; }
  int consumed = 0;
  gos::state::cell & pos_cell = this->game_state().grid_state()[pos()];
  if (pos_cell.type() == gos::state::cell_type::food) {
    gos::state::cell_state      * c_state   = pos_cell.state();
    gos::state::food_cell_state * food_cell =
      reinterpret_cast<gos::state::food_cell_state *>(c_state);
    consumed = food_cell->consume();
    if (consumed > 0) {
      _state.nticks_not_fed = 0;
      _state.strength       = std::min(max_strength(),
                                       strength() + consumed);
    }
  }
  if (consumed == 0 || strength() >= max_strength() / 2) {
    switch_mode(ant_state::ant_mode::scouting);
  }
}

void ant::harvest() noexcept {
  if (!is_alive()) { return; }
  if (_state.num_carrying >= strength() - 1) {
    switch_mode(ant_state::ant_mode::tracing);
  }
  int consumed = 0;
  gos::state::cell & pos_cell = this->game_state().grid_state()[pos()];
  if (pos_cell.type() == gos::state::cell_type::food) {
    gos::state::cell_state      * c_state   = pos_cell.state();
    gos::state::food_cell_state * food_cell =
      reinterpret_cast<gos::state::food_cell_state *>(c_state);
    consumed = food_cell->consume();
    if (consumed > 0) {
      _state.num_carrying += consumed;
    }
  }
  if (consumed == 0 || num_carrying() >= strength() - 1) {
    switch_mode(ant_state::ant_mode::tracing);
  }
}

void ant::trace_back() noexcept {
  if (!is_alive()) { return; }
  int consumed = 0;
  gos::state::cell & pos_cell = this->game_state().grid_state()[pos()];

  direction max_backtrace_dir { };
  direction sec_max_backtrace_dir { };
  int max_trace_intensity     = 0;
  int sec_max_trace_intensity = 0;
  for (int y = -1; y <= 1; ++y) {
    for (int x = -1; x <= 1; ++x) {
      position adj_pos { pos().x + x,
                         pos().y + y };
      if ((x == 0 && y == 0) ||
          !this->game_state().grid_state().contains_position(adj_pos)) {
        continue;
      }
      // find traces in adjacent cells that leads to the ant's current
      // cell; from those, move to the cell with the most recent trace:
      const auto & adj_cell   = this->game_state().grid_state()[adj_pos];
      const auto & adj_traces = adj_cell.state()->get_traces(team_id());
      // for example, the trace in the north-west adjacent cell leading
      // to this cell has orientation south-east.
      gos::orientation adj_cell_ort = dir2or(direction { -x, -y });
      gos::orientation in_trace_ort = dir2or(direction { -x, -y });
      int in_trace_ort_idx          = or2int(in_trace_ort);
      int adj_cell_ort_idx          = or2int(adj_cell_ort);
      int in_trace_intensity        = adj_traces[in_trace_ort_idx];
      // follow second highest intensity as highest intensity is
      // would toggle back to last backtraced cell:
      if (in_trace_intensity > max_trace_intensity) {
        sec_max_trace_intensity = max_trace_intensity;
        sec_max_backtrace_dir   = max_backtrace_dir;
        max_trace_intensity     = in_trace_intensity;
        max_backtrace_dir       = { x, y };
      }
    }
  }
  if (sec_max_backtrace_dir.dx != 0 && sec_max_backtrace_dir.dy != 0) {
    // trace found:
    if (num_no_dir_change() > 1) {
      set_dir(sec_max_backtrace_dir);
    }
  } else {
    // no trace found, just turn around:
    if (num_no_dir_change() > 8) {
      turn((this->game_state().round_count() % 2) - 1);
    }
  }
}

void ant::move() noexcept {
  if (!is_alive()) { return; }
  position pos_next { pos().x + dir().dx,
                      pos().y + dir().dy };
  if (pos_next == pos()) {
    return;
  }
  if (this->game_state().grid_state().allows_move_to(pos_next)) {
    this->game_state().grid_state()[_state.pos]
                      .leave(*this, this->game_state());
    this->game_state().grid_state()[pos_next]
                      .cell::enter(*this, this->game_state());
    set_pos(pos_next);
  } else {
    on_collision();
  }
/*
  // Bresenham differential:
  int d  = 2 * _dir.dy - _dir.dx;
  int d0 = 2 * _dir.dy;
  int dn = 2 * (_dir.dy - _dir.dx);
  if (d <= 0) { d += d0; }
  else        { d += dn; ++_pos.y; }
  ++_pos.x;
*/
}

gos::state::cell & ant::cell() noexcept {
  return this->game_state().grid_state()[pos()];
}

const gos::state::cell & ant::cell() const noexcept {
  return this->game_state().grid_state()[pos()];
}

std::ostream & operator<<(
  std::ostream          & os,
  const gos::state::ant & a)
{
  std::ostringstream ss;
  ss << "ant { " << a.id()
     << "-t"   << a.team_id()      << " "
     << "mod:" << a.mode()         << " "
     << "str:" << a.strength()     << " "
     << "car:" << a.num_carrying() << " "
     << "dir:" << "(" << a.dir().dx << "," << a.dir().dy << ") "
     << "ldc:" << a.num_no_dir_change() << " "
     << "att:" << a.damage() << " "
     << "}";
  return operator<<(os, ss.str());
}

} // namespace state
} // namespace gos
