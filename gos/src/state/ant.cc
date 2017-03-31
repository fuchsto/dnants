
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
  for (auto & a : _ants) {
    a.update();
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

void ant::update() noexcept {
  auto rc = _game_state.round_count();
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
  if (_strength > 1 && _nticks_not_fed > 100) {
    --_strength;
  }
}

void ant::move() noexcept {
  int px = _pos.x + _dir.dx;
  int py = _pos.y + _dir.dy;
  if (_game_state.grid_state().allows_move_to({ px, py })) {
    _blocked = false;
    _game_state.grid_state()[{ _pos.x, _pos.y }].leave(*this);
    _pos.x = px;
    _pos.y = py;
    _game_state.grid_state()[{ _pos.x, _pos.y }].enter(*this);
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
