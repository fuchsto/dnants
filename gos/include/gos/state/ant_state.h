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
    fighting,       // fighting enemy in adjacent cell
    tracing,        // follow the closes pheromone trace
    eating,         // eat and gain strength
    harvesting,     // eat and gain strength
    dead            // ant has died
  };

  enum ant_event : int {
    none       = 0,
    food,
    enemy,
    collision,
    attacked
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

  // Read-only:
  int          id;
  int          team_id;
  position     pos;
  direction    dir;
  size_t       rand            = 0;
  size_t       last_dir_change = 0;
  int          strength        = 0;
  int          damage          = 0;
  int          num_carrying    = 0;
  int          nticks_not_fed  = 0;
  int          tick_count      = 0;
  ant_event    event           = ant_event::none;
  ant_action   action          = ant_action::do_idle;
  ant_mode     mode            = ant_mode::scouting;

  // Commands:
  inline void set_mode(ant_mode m) noexcept {
    if (mode != ant_mode::dead &&
        // fighting while carrying is not allowed:
        (num_carrying == 0 || m != ant_mode::fighting)) {
      mode = m;
    }
  }

  inline void set_dir(direction d) noexcept {
    if (dir == d) { return; }
    last_dir_change = tick_count;
    dir             = d;
  }

  inline void set_turn(int turn_dir) noexcept {
    // for example, turn_dir +4 or -4 is reverse direction:
    int ort_idx = or2int(orientation());
    ort_idx += turn_dir;
    if (ort_idx < 0) { ort_idx  = 7 + ort_idx; }
    if (ort_idx > 7) { ort_idx -= ort_idx;     }
    auto ort = int2or(ort_idx);
    set_dir(or2dir(ort));
  }

  // Actions:
  inline void move()       noexcept {
                             action = ant_action::do_move; }
  inline void attack()     noexcept {
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
