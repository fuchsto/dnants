#ifndef GOS__STATE__ANT_H__INCLUDED
#define GOS__STATE__ANT_H__INCLUDED

#include <gos/types.h>
#include <gos/state/game_state.h>
#include <gos/util/timestamp.h>

#include <vector>


namespace gos {
namespace state {

class ant;
class game_state;

class ant_team {
  using game_state = gos::state::game_state;

  friend class ant;

  int                   _team_id;
  int                   _team_size;
  game_state &          _game_state;
  std::vector<ant>      _ants;
  std::vector<position> _spawn_points;
 public:
  ant_team() = delete;

  ant_team(
    int              team_id,
    int              init_team_size,
    const position & spawn_point,
    game_state     & gs)
  : _team_id(team_id)
  , _team_size(init_team_size)
  , _game_state(gs) {
    _spawn_points.push_back(spawn_point);
  }

  void update();

  inline int                      id()   const { return _team_id; }
  inline std::vector<ant>       & ants()       { return _ants; }
  inline const std::vector<ant> & ants() const { return _ants; }

  ant & add_ant_at(position && pos);
  ant & add_ant_at(const position & pos);
  void  spawn_ants();
};



class ant {
 public:
  enum mode : int {
    waiting  = 0, // do nothing
    blocked,      // collision, move failed
    scouting,     // move without following a pheromone trace
    tracing,      // follow the closes pheromone trace
    eating        // eat and gain strength
  };
  // Ants can detect pheromone traces and distinguish friendly (own team)
  // from enemy pheromones.
 private:
  game_state & _game_state;
  ant_team   & _team;
  int          _id;
  // When an ant is in a cell next to ants of another team, it dies if its
  // strength is less than the cumulative strength of the enemy ants in
  // adjacent cells (von-Neumann neighborhood, diagonals are considered
  // neighbor cells).
  int          _strength       = 10;
  // If an ant carries food or material, its strength available for fights
  // is reduced by the carried weight.
  int          _carry_weight   = 0;
  int          _nticks_not_fed = 0;
  position     _pos;
  direction    _dir;
  mode         _mode           = ant::mode::scouting;
  int          _rand           = 0;

 public:
  static const int max_strength() { return 20; }

 public:
  ant() = delete;
  ant(ant_team  & team,
      int         id,
      position && pos)
  : _game_state(team._game_state)
  , _team(team)
  , _id(id)
  , _pos(std::move(pos)) {
    move_to(_pos);
  }

  ant(ant_team       & team,
      int              id,
      const position & pos)
  : _game_state(team._game_state)
  , _team(team)
  , _id(id)
  , _pos(pos) {
    move_to(_pos);
  }

  void update() noexcept;

  const ant_team & team() const noexcept {
    return _team;
  }

  int id() const noexcept {
    return _id;
  }

  int strength() const noexcept {
    return _strength;
  }

  const position & pos() const noexcept {
    return _pos;
  }

  void set_direction(direction && dir) noexcept {
    _dir = std::move(dir);
  }

  void set_direction(const direction & dir) noexcept {
    _dir = dir;
  }

  void move() noexcept;

  void move_to(const position & pos) noexcept {
    _pos = pos;
  }

  void move_to(position && pos) noexcept {
    _pos = std::move(pos);
  }
 
 private:
  bool _can_move_to(int px, int py) const noexcept;

};

} // namespace state
} // namespace gos

#endif // GOS__STATE__ANT_H__INCLUDED
