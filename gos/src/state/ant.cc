
#include <gos/state/ant.h>

#include <gos/state/ant_state.h>
#include <gos/state/game_state.h>
#include <gos/state/grid.h>
#include <gos/state/cell.h>
#include <gos/state/population.h>

#include <gos/types.h>
#include <gos/client.h>

#include <gos/util/random.h>
#include <gos/util/logging.h>

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
    auto & base_state = base_cell.state();
    if (base_state.num_food() >= 4) {
      _team_size++;
      base_state.take_food(4);
    }
    if (!(base_cell.is_taken())) {
      if (_ants.size() >= _team_size) { return; }
      add_ant_at(spawn_pos);
    }
  }
}



void ant::on_home_cell(gos::state::cell_state & cell) noexcept {
  GOS__LOG_DEBUG("ant.on_home_cell",
                 "dist:(" << _state.dist.x << "," << _state.dist.y << ")");
  if (_state.dist.x != 0 || _state.dist.y != 0) {
    return;
  }
  GOS__LOG_DEBUG("ant.on_home_cell", "delivered " << num_carrying());
  cell.drop_food(num_carrying());
  _state.num_carrying = 0;
  switch_mode(ant_mode::scouting);
}

void ant::on_food_cell(gos::state::cell_state & cell) noexcept {
  if (_state.dist.x == 0 && _state.dist.y == 0) {
    _state.events.food = false;
  } else {
    _state.events.food = true;
  }
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



void ant::die() noexcept {
  GOS__LOG_DEBUG("ant.die", "ant died: " <<
                 "t:"   << team_id() << " " <<
                 "id:"  << id() << " " <<
                 "at (" << pos().x << "," << pos().y << ")");
  _state.mode = ant_mode::dead;
  this->game_state().grid_state()[_state.pos]
                    .leave(*this, this->game_state());
}

void ant::update_init() noexcept {
  _state.tick_count       = this->game_state().round_count();
  _state.rand             = gos::random();
  _state.damage           = 0;
  _state.events.enemy     = false;
  _state.events.food      = false;
  _state.events.attacked  = false;
}

void ant::update_position() noexcept {
  if (!is_alive()) { return; }
  // Apply cell change from last round:

  ++_state.nticks_not_fed;
  if (_state.strength > max_strength() / 2 &&
      ((_state.nticks_not_fed + 1) % 100) == 0) {
    --_state.strength;
  }

  if (_state.action == ant_action::do_move) {
    // Updates position and triggers grid cell events:
    move();
  }
}

void ant::update_action() noexcept {
  if (!is_alive()) { return; }

  const auto & current_cell = cell();

  _state.events.food = false;
  if (current_cell.type() != cell_type::spawn_point &&
      current_cell.state().num_food() > 0) {
    _state.events.food = true;
  }
  // Scan for enemies:
  _state.events.enemy = false;
  _state.enemy_dir    = { 0, 0 };
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
        on_enemy(this->game_state().population_state()
                                   .teams()[enemy_id.team_id]
                                   .ants()[enemy_id.id]);
      }
    }
  }

  // Request next state:
  _state = this->team().client().callback(
             _state,
             this->game_state().grid_state());

  // Apply actions from current round:
  if (_state.action == ant_action::do_eat) {
    eat();
  }
  else if (_state.action == ant_action::do_harvest) {
    harvest();
  }
  else if (_state.action == ant_action::do_attack) {
    attack();
  }
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
      pos_cell.state().drop_food(num_carrying());
      _state.num_carrying = 0;
    }
  }
}

void ant::eat() noexcept {
  int consumed = 0;
  gos::state::cell & pos_cell = this->game_state().grid_state()[pos()];
  consumed = pos_cell.state().take_food();
  if (consumed > 0) {
    _state.nticks_not_fed = 0;
    _state.strength       = std::min(max_strength(),
                                     strength() + consumed);
  }
}

void ant::harvest() noexcept {
  if (_state.num_carrying >= strength()) {
    // carry capacity reached
    return;
  }
  int consumed = 0;
  gos::state::cell & pos_cell = this->game_state().grid_state()[pos()];
  consumed = pos_cell.state().take_food();
  if (consumed > 0) {
    _state.num_carrying += consumed;
  }
}

void ant::move() noexcept {
  if (!is_alive()) { return; }
  if (dir().dx == 0 && dir().dy == 0) {
    return;
  }
  position pos_next { pos().x + dir().dx,
                      pos().y + dir().dy };
  if (this->game_state().grid_state().allows_move_to(pos_next)) {
    _state.events.collision = false;
    _state.dist.x += dir().dx;
    _state.dist.y += dir().dy;
    this->game_state().grid_state()[_state.pos]
                      .leave(*this, this->game_state());
    this->game_state().grid_state()[pos_next]
                      .cell::enter(*this, this->game_state());
    set_pos(pos_next);
  } else {
    on_collision();
  }
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

gos::state::cell & ant::cell() noexcept {
  return this->game_state().grid_state()[pos()];
}

const gos::state::cell & ant::cell() const noexcept {
  return this->game_state().grid_state()[pos()];
}

std::ostream & operator<<(
  std::ostream                          & os,
  const gos::state::ant_mode & m)
{
  std::ostringstream ss;
  ss << "mode(";
  switch (m) {
    case ant_mode::waiting:     ss << "waiting";     break;
    case ant_mode::scouting:    ss << "scouting";    break;
    case ant_mode::eating:      ss << "eating";      break;
    case ant_mode::harvesting:  ss << "harvesting";  break;
    case ant_mode::backtracing: ss << "backtracing"; break;
    case ant_mode::dead:        ss << "dead";        break;
    default: ss << "???"; break;
  }
  ss << ")";
  return operator<<(os, ss.str());
}

std::ostream & operator<<(
  std::ostream                            & os,
  const gos::state::ant_action & a)
{
  std::ostringstream ss;
  ss << "action(";
  switch (a) {
    case ant_action::do_idle:      ss << "idle";      break;
    case ant_action::do_eat:       ss << "eat";       break;
    case ant_action::do_drop:      ss << "drop";      break;
    case ant_action::do_move:      ss << "move";      break;
    case ant_action::do_attack:    ss << "attack";    break;
    case ant_action::do_harvest:   ss << "harvest";   break;
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
     << " dis:"  << "(" 
                 << a.state().dist.x << ","
                 << a.state().dist.y
                 << ")"
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
