#ifndef GOS__STATE__ANT_H__INCLUDED
#define GOS__STATE__ANT_H__INCLUDED

#include <gos/types.h>
#include <gos/util/timestamp.h>

#include <gos/state/game_state.h>

#include <vector>


namespace gos {
namespace state {

class ant;
class game_state;
class food_cell_state;

struct ant_id {
  int team_id;
  int id;
};

class ant_team {
  using game_state = gos::state::game_state;

  friend class ant;

  int                     _team_id;
  size_t                  _team_size;
  game_state            * _game_state;
  std::vector<ant>        _ants;
  std::vector<position>   _spawn_points;
 public:
  ant_team() = delete;

  ant_team(
    int              team_id,
    int              init_team_size,
    const position & spawn_point,
    game_state     & gs)
  : _team_id(team_id)
  , _team_size(init_team_size)
  , _game_state(&gs) {
    _spawn_points.push_back(spawn_point);
  }

  ant_team(const ant_team & other)           = delete;
  ant_team & operator=(const ant_team & rhs) = delete;
  ant_team(ant_team && other)                = default;
  ant_team & operator=(ant_team && rhs)      = default;

  void update();

  inline int                      id()   const { return _team_id; }
  inline std::vector<ant>       & ants()       { return _ants; }
  inline const std::vector<ant> & ants() const { return _ants; }

  inline const std::vector<position> & spawn_points() const {
    return _spawn_points;
  }

  ant & add_ant_at(position && pos);
  ant & add_ant_at(const position & pos);
  void  spawn_ants();
};



class ant {
 public:
  enum mode : int {
    waiting    = 0, // do nothing
    collision,      // collision, move failed
    scouting,       // move without following a pheromone trace
    fighting,       // fighting enemy in adjacent cell
    tracing,        // follow the closes pheromone trace
    eating,         // eat and gain strength
    dead            // ant has died
  };
  // Ants can detect pheromone traces and distinguish friendly (own team)
  // from enemy pheromones.
 private:
  ant_team   * _team;
  int          _id;
  // When an ant is in a cell next to ants of another team, it dies if its
  // strength is less than the cumulative strength of the enemy ants in
  // adjacent cells (von-Neumann neighborhood, diagonals are considered
  // neighbor cells).
  int          _strength       = 5;
  int          _attack_str     = 0;
  // If an ant carries food or material, its strength available for fights
  // is reduced by the carried weight.
  int          _carry_weight   = 0;
  int          _nticks_not_fed = 0;
  position     _pos;
  direction    _dir;
  mode         _mode           = ant::mode::scouting;
  size_t       _rand           = 0;

 public:
  static const int max_strength() { return 10; }

 public:
  ant() = delete;
  ant(ant_team  & team,
      int         id,
      position && pos)
  : _team(&team)
  , _id(id)
  , _pos(std::move(pos))
  { }

  ant(ant_team       & team,
      int              id,
      const position & pos)
  : _team(&team)
  , _id(id)
  , _pos(pos)
  { }

  ant(const ant & other) = delete;
  ant(ant && other)      = default;

  ant & operator=(const ant & rhs) = delete;
  ant & operator=(ant && rhs)      = default;

  void on_food_cell(gos::state::food_cell_state & food_cell);
  void on_collision();

  void update_position() noexcept;
  void update_action()   noexcept;
  void update_reaction() noexcept;

  void attack(gos::state::ant & enemy) noexcept;

  void attacked_by(gos::state::ant & enemy) noexcept;

  void die() noexcept;

  inline bool is_alive() const noexcept {
    return _mode != mode::dead;
  }

  inline bool is_blocked() const noexcept {
    return _mode == mode::collision;
  }

  inline const position & pos() const noexcept {
    return _pos;
  }

  inline const direction & dir() const noexcept {
    return _dir;
  }

  inline gos::orientation orientation() const noexcept {
    return gos::dir2or(_dir);
  }

  inline int id() const noexcept {
    return _id;
  }

  inline int team_id() const noexcept {
    return _team->id();
  }

  inline const gos::state::game_state & game_state() const noexcept {
    return *(_team->_game_state);
  }

  inline gos::state::game_state & game_state() noexcept {
    return *(_team->_game_state);
  }

  inline const ant_team & team() const noexcept {
    return *_team;
  }

  inline ant_team & team() noexcept {
    return *_team;
  }

  inline int strength() const noexcept {
    return _strength;
  }

  inline void set_direction(direction && dir) noexcept {
    _dir = std::move(dir);
  }

  inline void set_direction(const direction & dir) noexcept {
    _dir = dir;
  }

  inline void switch_mode(ant::mode m) noexcept {
    if (_mode != ant::mode::dead) {
      _mode = m;
    }
  }

 private:
  void eat()  noexcept;
  void move() noexcept;

};

} // namespace state
} // namespace gos

#endif // GOS__STATE__ANT_H__INCLUDED
