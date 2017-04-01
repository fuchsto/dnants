
#include <gos/state/ant.h>
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
  if (_game_state->round_count() % 20) {
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
    if (!(_game_state->grid_state()[spawn_pos].is_taken())) {
      if (_ants.size() >= _team_size) { return; }
      add_ant_at(spawn_pos);
    }
  }
}


void ant::on_home_cell(gos::state::spawn_cell_state & home_cell) {
  GOS__LOG_DEBUG("ant.on_home_cell", "delivered " << _num_carrying);
  _num_carrying = 0;
  switch_mode(mode::scouting);
}

void ant::on_food_cell(gos::state::food_cell_state & food_cell) {
  if (food_cell.amount_left() <= 0) {
    return;
  }
  if (_mode == mode::harvesting) {
    return;
  }
  if (_strength < max_strength()) {
    switch_mode(mode::eating);
  } else {
    switch_mode(mode::harvesting);
  }
}

void ant::on_collision() {
  if (_rand % 2) { turn(-2); }
  else           { turn(2);  }
}



void ant::attack(gos::state::ant & enemy) noexcept {
  GOS__LOG_DEBUG("ant.attack", "enemy: " <<
                 "t:"  << enemy.team_id() << " " <<
                 "id:" << enemy.id());
  switch_mode(mode::fighting);
  enemy.attacked_by(*this);
}

void ant::attacked_by(gos::state::ant & enemy) noexcept {
  GOS__LOG_DEBUG("ant.attacked_by", "enemy: " <<
                 "t:"  << enemy.team_id() << " " <<
                 "id:" << enemy.id());
  _attack_str += enemy.strength();
  if (_num_carrying == 0) {
    switch_mode(mode::fighting);
  }
}

void ant::die() noexcept {
  GOS__LOG_DEBUG("ant.die", "ant died: " <<
                 "t:"   << team_id() << " " <<
                 "id:"  << id() << " " <<
                 "at (" << pos().x << "," << pos().y << ")");
  _mode = mode::dead;
  this->game_state().grid_state()[_pos]
                    .leave(*this, this->game_state());
}

void ant::update_position() noexcept {
  if (!is_alive()) { return; }
  auto rc     = this->game_state().round_count();
  _rand       = gos::random();
  _attack_str = 0;
  ++_nticks_not_fed;
  switch (_mode) {
    case ant::mode::eating:
      eat();
      break;
    case ant::mode::harvesting:
      harvest();
      break;
    case ant::mode::tracing:
      trace_back();
      move();
      break;
    case ant::mode::scouting:
      if (_rand % 4 == 0) {
      // int dx = ((_rand + rc / 2) % 3) - 1;
      // int dy = ((_rand + rc * 7) % 3) - 1;
      // set_direction(direction { dx, dy });
        turn(((_rand + rc * 7) % 5) - 2);
      }
      move();
      break;
    default:
      break;
  }
  if (_strength > 1 && ((_nticks_not_fed + 1) % 100) == 0) {
    --_strength;
  }
}

void ant::update_action() noexcept {
  if (!is_alive())       { return; }
  // Cannot attack while carrying:
  if (_num_carrying > 0) { return; }
  gos::state::ant_id enemy_id { -1, -1 };
  gos::state::ant *  enemy = nullptr;
  for (int y = -1; enemy_id.id == -1 && y <= 1; ++y) {
    for (int x = -1; enemy_id.id == -1 && x <= 1; ++x) {
      position adj_pos { _pos.x + x,
                         _pos.y + y };
      if ((x == 0 && y == 0) ||
          !this->game_state().grid_state().contains_position(adj_pos)) {
        continue;
      }
      enemy_id = this->game_state().grid_state()[adj_pos].ant();

      if (enemy_id.id != -1 &&
          enemy_id.team_id != team_id()) {
        enemy = &(this->game_state().population_state()
                                  .teams()[enemy_id.team_id]
                                  .ants()[enemy_id.id]);
        set_direction(direction { x, y });
        break;
      }
    }
  }
  if (enemy != nullptr) {
    attack(*enemy);
  }
  if (_mode == mode::fighting && (enemy == nullptr || !enemy->is_alive())) {
    switch_mode(mode::scouting);
  }
}

void ant::update_reaction() noexcept {
  if (!is_alive()) { return; }
  if (_strength <= _attack_str) {
    if (_strength > 1) {
      --_strength;
    } else {
      die();
    }
  }
}

void ant::eat() noexcept {
  if (!is_alive()) { return; }
  int consumed = 0;
  gos::state::cell & pos_cell = this->game_state().grid_state()[_pos];
  if (pos_cell.type() == gos::state::cell_type::food) {
    gos::state::cell_state      * c_state   = pos_cell.state();
    gos::state::food_cell_state * food_cell =
      reinterpret_cast<gos::state::food_cell_state *>(c_state);
    consumed = food_cell->consume();
    if (consumed > 0) {
      _nticks_not_fed = 0;
      _strength       = std::min(max_strength(), _strength + consumed);
    }
  }
  if (consumed == 0 || _strength >= max_strength() / 2) {
    switch_mode(mode::scouting);
  }
}

void ant::harvest() noexcept {
  if (!is_alive()) { return; }
  int consumed = 0;
  gos::state::cell & pos_cell = this->game_state().grid_state()[_pos];
  if (pos_cell.type() == gos::state::cell_type::food) {
    gos::state::cell_state      * c_state   = pos_cell.state();
    gos::state::food_cell_state * food_cell =
      reinterpret_cast<gos::state::food_cell_state *>(c_state);
    consumed = food_cell->consume();
    if (consumed > 0) {
      _num_carrying += consumed;
    }
  }
  if (consumed == 0 || _num_carrying == strength() - 1) {
    switch_mode(mode::tracing);
  }
}

void ant::trace_back() noexcept {
  if (!is_alive()) { return; }
  int consumed = 0;
  gos::state::cell & pos_cell = this->game_state().grid_state()[_pos];

  direction max_backtrace_dir { };
  direction sec_max_backtrace_dir { };
  int max_trace_intensity     = 0;
  int sec_max_trace_intensity = 0;
  for (int y = -1; y <= 1; ++y) {
    for (int x = -1; x <= 1; ++x) {
      position adj_pos { _pos.x + x,
                         _pos.y + y };
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
    set_direction(sec_max_backtrace_dir);
  } else {
    // no trace found, just turn around:
    if (num_no_dir_change() > 4) {
      turn((this->game_state().round_count() % 4) - 2);
    }
  }
}

void ant::move() noexcept {
  if (!is_alive()) { return; }
  position pos_next { _pos.x + _dir.dx,
                      _pos.y + _dir.dy };
  if (pos_next == _pos) {
    return;
  }
  if (this->game_state().grid_state().allows_move_to(pos_next)) {
  this->game_state().grid_state()[_pos]
                    .leave(*this, this->game_state());
  this->game_state().grid_state()[pos_next]
                    .enter(*this, this->game_state());
    _pos = pos_next;
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

std::ostream & operator<<(
  std::ostream          & os,
  const gos::state::ant & a)
{
  std::ostringstream ss;
  ss << "ant { " << a.id()
     << "-t"   << a.team_id()  << " "
     << "mod:" << a.ant_mode() << " "
     << "str:" << a.strength() << " "
     << "car:" << a.num_carrying() << " "
     << "dir:" << "(" << a.dir().dx << "," << a.dir().dy << ") "
     << "ldc:" << a.num_no_dir_change() << " "
     << "att:" << a.attacker_strength() << " "
     << "}";
  return operator<<(os, ss.str());
}

} // namespace state
} // namespace gos
