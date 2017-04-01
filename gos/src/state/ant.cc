
#include <gos/state/ant.h>
#include <gos/state/game_state.h>
#include <gos/state/grid.h>
#include <gos/state/cell.h>

#include <gos/types.h>
#include <gos/util/random.h>


namespace gos {
namespace state {

void ant_team::update_positions() {
  if (_game_state.round_count() % 20) {
    spawn_ants();
  }
  for (auto & a : _ants) {
    if (a.is_alive()) {
      a.update_position();
    }
  }
}

void ant_team::update_reactions() {
  for (auto & a : _ants) {
    if (a.is_alive()) {
      a.update_reaction();
    }
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

void ant::on_food_cell(gos::state::food_cell_state & food_cell)
{
  if (_strength < max_strength()) {
    _strength += food_cell.consume();
  }
}

void ant::attack(gos::state::ant & enemy) noexcept {
  GOS__LOG_DEBUG("ant.attack", "enemy: " <<
                 "team:" << enemy.team().id() << " " <<
                 "id:"   << enemy.id());
  _mode = mode::fighting;
  enemy.attacked_by(*this);
}

void ant::attacked_by(gos::state::ant & enemy) noexcept {
  GOS__LOG_DEBUG("ant.attacked_by", "enemy: " <<
                 "team:" << enemy.team().id() << " " <<
                 "id:"   << enemy.id());
  _mode = mode::fighting;
  if (_strength <= enemy.strength()) {
    if (_strength > 1) {
      --_strength;
    } else {
      die();
    }
  }
}

void ant::die() noexcept {
  _mode = mode::dead;
  _game_state->grid_state()[_pos]
             .leave(*this, *_game_state);
}

void ant::update_position() noexcept {
  auto rc = _game_state->round_count();
  _rand   = gos::random();
  ++_nticks_not_fed;
  switch (_mode) {
    case ant::mode::eating:
      _nticks_not_fed = 0;
      ++_strength;
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
  if (_strength > 1 && (_nticks_not_fed % 100) == 0) {
    --_strength;
  }
}

void ant::update_reaction() noexcept {
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
      if (enemy != nullptr && enemy->team().id() != team().id()) {
        break;
        _dir  = direction { x, y };
      } else {
        enemy = nullptr;
      }
    }
  }
  if (enemy != nullptr && enemy->team().id() != team().id()) {
    GOS__LOG_DEBUG("ant.update", "ant " <<
                   "t:"  << team().id() << " " << "id:"   << id() << " " <<
                   "at " << pos().x << "," << pos().y << " " <<
                   "attacks enemy " <<
                   "at " << enemy->pos().x << "," << enemy->pos().y);
    attack(*enemy);
    if (is_alive() && !enemy->is_alive()) {
      _mode = mode::scouting;
    }
  } else {
    _mode = mode::scouting;
  }
}

void ant::move() noexcept {
  int px = _pos.x + _dir.dx;
  int py = _pos.y + _dir.dy;
  if (_game_state->grid_state().allows_move_to({ px, py })) {
    _blocked = false;
    _game_state->grid_state()[_pos]
                .leave(*this, *_game_state);
    _pos.x = px;
    _pos.y = py;
    _game_state->grid_state()[{ px, py }]
                .enter(*this, *_game_state);
  } else {
    _blocked = true;
    // collision, move failed:
    if (_rand % 2) { _dir.dx *= -1; }
    else           { _dir.dy *= -1; }
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
