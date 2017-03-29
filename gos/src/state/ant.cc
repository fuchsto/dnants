
#include <gos/state/ant.h>
#include <gos/state/game_state.h>
#include <gos/state/grid.h>
#include <gos/state/cell.h>

#include <gos/types.h>


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
      add_ant_at(spawn_pos);
    }
  }
}

void ant::update() noexcept {
  auto rc = _game_state.round_count();
  _rand   = static_cast<int>(gos::timestamp_ns());
  ++_nticks_not_fed;
  switch (_mode) {
    case ant::mode::eating:
      _nticks_not_fed = 0;
      ++_strength;
      break;
    case ant::mode::scouting:
      if (rc % 8 == 0) {
        int dx = ((_rand ) % 3) - 1;
        int dy = ((_rand + rc) % 3) - 1;
        set_direction(direction { dx, dy });
      }
      move();
      break;
    default:
      break;
  }
  if (_strength > 0 && _nticks_not_fed > 100) {
    --_strength;
  }
}

void ant::move() noexcept {
  int px = _pos.x + _dir.dx;
  int py = _pos.y + _dir.dy;
  if (_can_move_to(px, py)) {
    _game_state.grid_state()[{ _pos.x, _pos.y }].leave(*this);
    _pos.x = px;
    _pos.y = px;
    _game_state.grid_state()[{ _pos.x, _pos.y }].enter(*this);
  } else {
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

bool ant::_can_move_to(int px, int py) const noexcept {
  return ( px > 0 &&
           py > 0 &&
           px < _game_state.grid_state().extents().w &&
           py < _game_state.grid_state().extents().h &&
           !_game_state.grid_state()[{ px, py }].is_taken() );
}

} // namespace state
} // namespace gos