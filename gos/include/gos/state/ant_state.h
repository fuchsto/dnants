#ifndef GOS__STATE__ANT_STATE_H__INCLUDED
#define GOS__STATE__ANT_STATE_H__INCLUDED

#include <gos/types.h>


namespace gos {
namespace state {

struct ant_id {
  int team_id;
  int id;
};

struct ant_state {
 public:
  enum ant_mode : int {
    waiting    = 0, // do nothing
    detour,         // collision, move failed
    scouting,       // move without following a pheromone trace
    tracing,        // follow the closes pheromone trace
    eating,         // eat and gain strength
    harvesting,     // eat and gain strength
    dead            // ant has died
  };

  enum ant_action : int {
    do_idle    = 0,
    do_move,
    do_backtrace,
    do_eat,
    do_harvest,
    do_drop,
    do_attack,
    do_turn
  };

  struct state_events {
    bool collision = false;
    bool attacked  = false;
    bool food      = false;
    bool enemy     = false;
  };

  // Read-only:
  int          id;
  int          team_id;
  position     pos;
  direction    dir             { 0, 0 };
  size_t       rand            = 0;
  size_t       last_dir_change = 0;
  int          strength        = 0;
  int          damage          = 0;
  int          num_carrying    = 0;
  int          nticks_not_fed  = 0;
  int          tick_count      = 0;
  state_events events;
  direction    enemy_dir       { 0, 0 };
  ant_action   action          = ant_action::do_idle;
  ant_mode     mode            = ant_mode::scouting;

  // Commands:
  inline void set_mode(ant_mode m) noexcept {
    if (mode == ant_mode::dead) { return; }
    mode = m;
  }

  inline void set_action(ant_action a) noexcept {
  }

  inline void set_dir(int x, int y) noexcept {
    if (dir.dx == x && dir.dy == y) { return; }
    last_dir_change = tick_count;
    dir.dx = x;
    dir.dy = y;
  }

  inline void turn_dir(int turn) noexcept {
    // for example, turn_dir +4 or -4 is reverse direction:
    int ort_idx = or2int(gos::dir2or(dir));
    ort_idx += turn;
    if (ort_idx < 0) { ort_idx  = 7 + ort_idx; }
    if (ort_idx > 7) { ort_idx -= ort_idx;     }
    auto turn_dir = or2dir(int2or(ort_idx));
    set_dir(turn_dir.dx, turn_dir.dy);
  }

  // Actions:
  inline void move()       noexcept {
                             action = ant_action::do_move; }
  inline void attack()     noexcept {
                             if (num_carrying > 0) { return; }
                             action = ant_action::do_attack; }
  inline void eat()        noexcept {
                             action = ant_action::do_eat; }
  inline void harvest()    noexcept {
                             action = ant_action::do_harvest; }
  inline void drop()       noexcept {
                             action = ant_action::do_drop; }
  inline void backtrace()  noexcept {
                             action = ant_action::do_backtrace; }
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__ANT_STATE_H__INCLUDED
