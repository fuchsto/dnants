
#include <gos/state/ant.h>

#include <gos/state/ant_state.h>
#include <gos/state/game_state.h>
#include <gos/state/grid.h>
#include <gos/state/cell.h>
#include <gos/state/population.h>

#include <gos/types.h>
#include <gos/default_rules.h>

#include <gos/util/random.h>

#include <iostream>
#include <sstream>


namespace gos {
namespace state {

void ant_team::update() {
  if (_game_state->round_count() % 20 == 1) {
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
  _state.events.food = true;
}

void ant::on_enemy(
  gos::state::ant & enemy) noexcept {
  _state.events.enemy = true;
}

void ant::on_attacked(gos::state::ant & enemy) noexcept {
  _state.events.attacked = true;
  GOS__LOG_DEBUG("ant.on_attacked", "enemy: " <<
                 "t:"  << enemy.team_id() << " " <<
                 "id:" << enemy.id());
  _state.damage += enemy.strength();
}

void ant::on_collision() noexcept {
  _state.events.collision = true;
}



void ant::attack() noexcept {
  if (num_carrying() > 0) {
    GOS__LOG_DEBUG("ant.attack", "failed: cannot attack while carrying");
    return;
  }
  if (dir().dx == 0 && dir().dy == 0) {
    GOS__LOG_DEBUG("ant.attack", "failed: no direction for attack");
    return;
  }
  position enemy_pos { pos().x + dir().dx,
                       pos().y + dir().dy };
  if (!this->game_state().grid_state().contains_position(enemy_pos)) {
    GOS__LOG_DEBUG("ant.attack",
                   "failed: invalid enemy position " <<
                   "(" << enemy_pos.x << "," << enemy_pos.y << ")");
    return;
  }
  ant_id enemy_id = this->game_state().grid_state()[enemy_pos].ant();
  if (enemy_id.id == -1 || enemy_id.team_id == -1 ||
      enemy_id.team_id == team_id()) {
    GOS__LOG_DEBUG("ant.attack", "failed: invalid enemy id " <<
                   "id:" << enemy_id.id << " team:" << enemy_id.team_id);
    return;
  }
  gos::state::ant & enemy = this->game_state().population_state()
                                              .teams()[enemy_id.team_id]
                                              .ants()[enemy_id.id];
  GOS__LOG_DEBUG("ant.attack",
                 "ant "   << id() << ".t" << team_id() << " attacks "
                 "enemy " << enemy_id.id << ".t" << enemy_id.team_id);
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
  // Apply cell change from last round:
  _state.tick_count       = this->game_state().round_count();
  _state.rand             = gos::random();
  _state.damage           = 0;
  _state.events.enemy     = false;
  _state.events.food      = false;
  _state.events.collision = false;
  _state.events.attacked  = false;

  ++_state.nticks_not_fed;
  if (_state.strength > max_strength() / 2 &&
      ((_state.nticks_not_fed + 1) % 100) == 0) {
    --_state.strength;
  }

  if (_state.action == ant_state::ant_action::do_move) {
    // Updates position and triggers grid cell events:
    move();
  } else if (_state.action == ant_state::ant_action::do_backtrace) {
    backtrace();
    move();
  }

  const auto & current_cell = cell();
  if (current_cell.type() == cell_type::plain ||
      current_cell.type() == cell_type::food) {
    const resource_cell_state * current_cell_state =
      reinterpret_cast<const resource_cell_state *>(current_cell.state());
    if (current_cell_state->amount_left() > 0) {
      _state.events.food = true;
    }
  }
}

void ant::update_action() noexcept {
  if (!is_alive()) { return; }
  // Apply actions from current round:
  if (_state.action == ant_state::ant_action::do_eat) {
    eat();
  }
  else if (_state.action == ant_state::ant_action::do_harvest) {
    harvest();
  }
  else if (_state.action == ant_state::ant_action::do_attack) {
    attack();
  }
  // Scan for enemies:
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
        _state.enemy_dir = { x, y };
        return on_enemy(this->game_state().population_state()
                                          .teams()[enemy_id.team_id]
                                          .ants()[enemy_id.id]);
      }
    }
  }
  // No enemy in range
  _state.events.enemy = false;
  _state.enemy_dir    = { 0, 0 };
}

void ant::update_reaction() noexcept {
  if (!is_alive()) { return; }
  // Apply results of actions from current round:
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
  // Request next state:
  _state = gos::update_ant(_state);
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
}

void ant::harvest() noexcept {
  if (!is_alive()) { return; }
  if (_state.num_carrying >= strength()) {
    // carry capacity reached
    return;
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
}

void ant::backtrace() noexcept {
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
}

gos::state::cell & ant::cell() noexcept {
  return this->game_state().grid_state()[pos()];
}

const gos::state::cell & ant::cell() const noexcept {
  return this->game_state().grid_state()[pos()];
}

std::ostream & operator<<(
  std::ostream                          & os,
  const gos::state::ant_state::ant_mode & m)
{
  std::ostringstream ss;
  ss << "mode(";
  switch (m) {
    case ant_state::ant_mode::waiting:    ss << "waiting";     break;
    case ant_state::ant_mode::scouting:   ss << "scouting";    break;
    case ant_state::ant_mode::detour:     ss << "detour";      break;
    case ant_state::ant_mode::eating:     ss << "eating";      break;
    case ant_state::ant_mode::harvesting: ss << "harvesting";  break;
    case ant_state::ant_mode::tracing:    ss << "tracing";     break;
    case ant_state::ant_mode::dead:       ss << "dead";        break;
    default: ss << "???"; break;
  }
  ss << ")";
  return operator<<(os, ss.str());
}

std::ostream & operator<<(
  std::ostream                            & os,
  const gos::state::ant_state::ant_action & a)
{
  std::ostringstream ss;
  ss << "action(";
  switch (a) {
    case ant_state::ant_action::do_idle:      ss << "idle";      break;
    case ant_state::ant_action::do_eat:       ss << "eat";       break;
    case ant_state::ant_action::do_drop:      ss << "drop";      break;
    case ant_state::ant_action::do_move:      ss << "move";      break;
    case ant_state::ant_action::do_attack:    ss << "attack";    break;
    case ant_state::ant_action::do_harvest:   ss << "harvest";   break;
    case ant_state::ant_action::do_backtrace: ss << "backtrace"; break;
    default: ss << "???"; break;
  }
  ss << ")";
  return operator<<(os, ss.str());
}

std::ostream & operator<<(
  std::ostream                              & os,
  const gos::state::ant_state::state_events & e)
{
  std::ostringstream ss;
  ss << "events { ";
  if (e.enemy)     { ss << "enemy ";     }
  if (e.food)      { ss << "food ";      }
  if (e.collision) { ss << "collision "; }
  if (e.attacked)  { ss << "attacked ";  }
  ss << "}";
  return operator<<(os, ss.str());
}

std::ostream & operator<<(
  std::ostream          & os,
  const gos::state::ant & a)
{
  std::ostringstream ss;
  ss << "ant { "
     << " id:"   << a.id() 
                 << ".t" << a.team_id() << " "
     << a.mode() << " "
     << a.state().action << " "
     << a.state().events;
  if (a.state().events.enemy) {
     ss << " edir:"  << "(" 
                     << a.state().enemy_dir.dx << ","
                     << a.state().enemy_dir.dy
                     << ")";
  }
  ss << " str:"  << a.strength()
     << " dir:"  << "(" 
                 << a.dir().dx << ","
                 << a.dir().dy
                 << ")"
     << " car:"  << a.num_carrying()
     << " dmg:"  << a.damage()
     << " ldc:"  << a.state().last_dir_change
     << " tic:"  << a.state().tick_count
     << " rnd:"  << a.state().rand
     << " }";
  return operator<<(os, ss.str());
}

} // namespace state
} // namespace gos
