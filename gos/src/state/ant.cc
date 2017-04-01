
#include <gos/state/ant.h>
#include <gos/state/game_state.h>
#include <gos/state/grid.h>
#include <gos/state/cell.h>

#include <gos/types.h>
#include <gos/util/random.h>


namespace gos {
namespace state {

void ant_team::update() {
  if (_game_state.round_count() % 20) {
    spawn_ants();
  }
}

ant & ant_team::add_ant_at(position && pos) {
  _ants.push_back(ant(*this, _ants.size(), std::move(pos)));
  return _ants.back();
}

ant & ant_team::add_ant_at(const position & pos) {
  _ants.push_back(ant(*this, _ants.size(), pos));
  return _ants.back();
}

void ant_team::spawn_ants() {
  for (auto & spawn_pos : _spawn_points) {
    if (!(_game_state.grid_state()[spawn_pos].is_taken())) {
      if (_ants.size() >= _team_size) { return; }
      add_ant_at(spawn_pos);
    }
  }
}



void ant::on_food_cell(gos::state::food_cell_state & food_cell) {
  if (_strength < max_strength() &&
      food_cell.amount_left() > 0) {
    switch_mode(mode::eating);
  } else {
    switch_mode(mode::scouting);
  }
}

void ant::on_collision() {
  if (_rand % 2) { _dir.dx *= -1; }
  else           { _dir.dy *= -1; }
  switch_mode(mode::scouting);
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
  switch_mode(mode::fighting);
  _attack_str += enemy.strength();
}

void ant::die() noexcept {
  GOS__LOG_DEBUG("ant.die", "ant died: " <<
                 "t:"   << team_id() << " " <<
                 "id:"  << id() << " " <<
                 "at (" << pos().x << "," << pos().y << ")");
  _mode = mode::dead;
  _game_state->grid_state()[_pos]
             .leave(*this, *_game_state);
}

void ant::update_position() noexcept {
  if (!is_alive()) { return; }
  auto rc     = _game_state->round_count();
  _rand       = gos::random();
  _attack_str = 0;
  ++_nticks_not_fed;
  switch (_mode) {
    case ant::mode::eating:
      eat();
      break;
    case ant::mode::scouting:
      if (_rand % 8 == 0) {
        int dx = ((_rand + rc / 2) % 3) - 1;
        int dy = ((_rand + rc * 7) % 3) - 1;
        set_direction(direction { dx, dy });
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
  if (!is_alive()) { return; }
  gos::state::ant * enemy = nullptr;
  for (int y = -1; enemy == nullptr && y <= 1; ++y) {
    for (int x = -1; enemy == nullptr && x <= 1; ++x) {
      position adj_pos { _pos.x + x,
                         _pos.y + y };
      if ((x == 0 && y == 0) ||
          !_game_state->grid_state().contains_position(adj_pos)) {
        continue;
      }
      enemy = _game_state->grid_state()[adj_pos].ant();
      if (enemy != nullptr &&
          enemy->team_id() != team_id() &&
          enemy->is_alive()) {
        _dir  = direction { x, y };
        break;
      } else {
        enemy = nullptr;
      }
    }
  }
  if (enemy != nullptr) {
    GOS__LOG_DEBUG(
        "ant.reaction", "ant " <<
        "t:"   << team_id() << " " <<
        "id:"  << id() << " " <<
        "at (" << pos().x << "," << pos().y << ") " <<
        "attacks enemy " <<
        "t:"   << enemy->team_id() << " " <<
        "id:"  << enemy->id() << " " <<
        "at (" << enemy->pos().x << "," << enemy->pos().y << ")");
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
  gos::state::cell & pos_cell =_game_state->grid_state()[_pos];
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

void ant::move() noexcept {
  if (!is_alive()) { return; }
  position pos_next { _pos.x + _dir.dx,
                      _pos.y + _dir.dy };
  if (pos_next == _pos) {
    return;
  }
  if (_game_state->grid_state().allows_move_to(pos_next)) {
    _game_state->grid_state()[_pos]
                .leave(*this, *_game_state);
    _game_state->grid_state()[pos_next]
                .enter(*this, *_game_state);
    _pos = pos_next;
  } else {
    switch_mode(mode::collision);
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

} // namespace state
} // namespace gos
