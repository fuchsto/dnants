#ifndef GOS__STATE__ANT_H__INCLUDED
#define GOS__STATE__ANT_H__INCLUDED

#include <gos/types.h>

#include <vector>


namespace gos {
namespace state {

class ant;
class game_state;

class ant_team {
  using game_state = gos::state::game_state;

  friend class ant;

  int              _team_id;
  game_state &     _game_state;
  std::vector<ant> _ants;
 public:
  ant_team() = delete;

  ant_team(int team_id, game_state & gs)
  : _team_id(team_id)
  , _game_state(gs)
  { }

  int                      id()   const { return _team_id; }
  std::vector<ant>       & ants()       { return _ants; }
  const std::vector<ant> & ants() const { return _ants; }
};



class ant {
 public:
  enum mode : int {
    waiting  = 0, // do nothing
    scouting,     // move without following a pheromone trace
    tracing,      // follow the closes pheromone trace
    eating        // eat and gain strength
  };
  // Ants can detect pheromone traces and distinguish friendly (own team)
  // from enemy pheromones.
 private:
  ant_team   & _team;
  game_state & _game_state;
  // When an ant is in a cell next to ants of another team, it dies if its
  // strength is less than the cumulative strength of the enemy ants in
  // adjacent cells (von-Neumann neighborhood, diagonals are considered
  // neighbor cells).
  int       _strength       = 10;
  // If an ant carries food or material, its strength available for fights
  // is reduced by the carried weight.
  int       _carry_weight   = 0;
  int       _nticks_not_fed = 0;
  position  _pos;
  direction _dir;
  mode      _mode           = ant::mode::waiting;
 public:
  ant() = delete;
  ant(ant_team  & team,
      position && pos)
  : _team(team)
  , _game_state(team._game_state)
  , _pos(std::move(pos))
  { }

  void tick() {
    ++_nticks_not_fed;
    switch (_mode) {
      case ant::mode::eating:
        _nticks_not_fed = 0;
        break;
      default:
        break;
    }
  }

  const position & pos() const noexcept {
    return _pos;
  }

  void set_direction(direction && dir) noexcept {
    _dir = std::move(dir);
  }

  void move() noexcept {
    move_to(position { _pos.x + _dir.dx,
                       _pos.y + _dir.dy });
  }

  void move_to(position && pos) noexcept {
    _pos = std::move(pos);
  }

};

} // namespace state
} // namespace gos

#endif // GOS__STATE__ANT_H__INCLUDED
