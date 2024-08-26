#ifndef GOS__STATE__ANT_H__INCLUDED
#define GOS__STATE__ANT_H__INCLUDED

#include <gos/state/ant_state.h>
#include <gos/state/game_state.h>

#include <gos/types.h>
#include <gos/client.h>

#include <gos/util/timestamp.h>

#include <vector>
#include <iostream>
#include <algorithm>


namespace gos {
namespace state {

class ant;
class game_state;
class cell;

class resource_cell_state;
class food_cell_state;
class spawn_cell_state;

class ant_team {
  using game_state = gos::state::game_state;

  friend class ant;

  int                     _team_id;
  size_t                  _team_size;
  gos::client             _client;
  game_state            * _game_state;
  std::vector<ant>        _ants;
  std::vector<position>   _spawn_points;
  int                     _num_food = 0;
 public:
  ant_team() = delete;

  ant_team(
    int              team_id,
    int              init_team_size,
    const position & spawn_point,
    game_state     & gs)
  : _team_id(team_id)
  , _team_size(init_team_size)
  , _client(team_id, gs.settings().team_codes[team_id])
  , _game_state(&gs)
  , _num_food(0) {
    _spawn_points.push_back(spawn_point);
  }

  ant_team(const ant_team & other)           = delete;
  ant_team & operator=(const ant_team & rhs) = delete;
  ant_team(ant_team && other)                = default;
  ant_team & operator=(ant_team && rhs)      = default;

  void update();

  inline const gos::client      & client() const { return _client; }  

  inline int                      id()     const { return _team_id; }
  inline std::vector<ant>       & ants()         { return _ants; }
  inline const std::vector<ant> & ants()   const { return _ants; }

  int num_ants() const noexcept;

  inline void store_food(int nfood) noexcept {
    _num_food += nfood;
  }

  inline int num_food() const noexcept {
    return _num_food;
  }

  inline const std::vector<position> & spawn_points() const {
    return _spawn_points;
  }

  ant & add_ant_at(position && pos);
  ant & add_ant_at(const position & pos);
  void  spawn_ants();
};



class ant {
 private:
  ant_team   * _team  = nullptr;
  ant_state    _state;

 public:
  static const int max_strength() { return 10; }

 public:
  ant() = delete;

  ant(ant_team       & team,
      int              id,
      const position & pos)
  : _team(&team)
  {
    _state.id       = id;
    _state.team_id  = team.id();
    _state.pos      = pos;
    _state.strength = 5;
    set_dir({ (( id + 7) % 3) - 1,
              (( id + 5) % 3) - 1 });
  }

  ant(const ant & other) = delete;
  ant(ant && other)      = default;

  ant & operator=(const ant & rhs) = delete;
  ant & operator=(ant && rhs)      = default;

 private:
  inline void set_pos(const position & p) noexcept {
    _state.pos = p;
  }

 public:
  const ant_state & state() const noexcept {
    return _state;
  }

  void on_home_cell(gos::state::cell_state & home_cell) noexcept;
  void on_food_cell(gos::state::cell_state & food_cell) noexcept;
  void on_enemy(gos::state::ant & enemy)    noexcept;
  void on_attacked(gos::state::ant & enemy) noexcept;
  void on_collision() noexcept;

  void update_init()     noexcept;
  void update_position() noexcept;
  void update_action()   noexcept;
  void update_reaction() noexcept;

  void die() noexcept;

  inline bool is_alive() const noexcept {
    return mode() != ant_mode::dead;
  }

  inline size_t rand() const noexcept {
    return _state.rand;
  }

  inline const position & pos() const noexcept {
    return _state.pos;
  }

  inline const direction & dir() const noexcept {
    return _state.dir;
  }

  inline gos::orientation orientation() const noexcept {
    return gos::dir2or(_state.dir);
  }

  inline int id() const noexcept {
    return _state.id;
  }

  inline int team_id() const noexcept {
    return _team->id();
  }

  const gos::state::cell & cell() const noexcept;

  gos::state::cell & cell() noexcept;

  inline ant_mode mode() const noexcept {
    return _state.mode;
  }

  inline int strength() const noexcept {
    return _state.strength;
  }

  inline int num_carrying() const noexcept {
    return _state.num_carrying;
  }

  inline int damage() const noexcept {
    return _state.damage;
  }

  inline void set_dir(direction && d) noexcept {
    if (_state.dir == d) { return; }
    _state.last_dir_change = game_state().round_count();
    _state.dir = std::move(d);
  }

  inline void set_dir(const direction & d) noexcept {
    if (dir() == d) { return; }
    _state.last_dir_change = game_state().round_count();
    _state.dir = d;
  }

  inline void turn(int turn_dir) noexcept {
    // for example, turn_dir +4 or -4 is reverse direction:
    int ort_idx = or2int(this->orientation());
    ort_idx += turn_dir;
    if (ort_idx < 0) { ort_idx  = 7 + ort_idx; }
    if (ort_idx > 7) { ort_idx -= ort_idx;     }
    auto ort = int2or(ort_idx);
    set_dir(or2dir(ort));
  }

  inline int num_no_dir_change() const noexcept {
    return this->game_state().round_count() - _state.last_dir_change;
  }

  inline void switch_mode(ant_mode m) noexcept {
    if (!is_alive()) { return; }
    _state.mode = m;
  }

 private:
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

 private:
  // actions:
  void attack()     noexcept;
  void eat()        noexcept;
  void harvest()    noexcept;
  void move()       noexcept;

};

std::ostream & operator<<(
  std::ostream               & os,
  const gos::state::ant_mode & m);

std::ostream & operator<<(
  std::ostream                 & os,
  const gos::state::ant_action & a);

std::ostream & operator<<(
  std::ostream                              & os,
  const gos::state::ant_state::state_events & e);

std::ostream & operator<<(
  std::ostream          & os,
  const gos::state::ant & a);

} // namespace state
} // namespace gos

#endif // GOS__STATE__ANT_H__INCLUDED
