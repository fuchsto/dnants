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
    idle       = 0,
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
  position     pos;
  size_t       rand            = 0;
  size_t       last_dir_change = 0;
  int          strength        = 0;
  int          damage          = 0;
  int          num_carrying    = 0;
  int          nticks_not_fed  = 0;
  ant_event    event           = ant_event::none;
  ant_action   action          = ant_action::idle;

  // Read-write:
  direction    dir;
  ant_mode     mode            = ant_mode::scouting;

  // actions:
  inline void attack(direction d) noexcept {
                                    dir    = d;
                                    action = ant_action::do_attack; }
  inline void eat()               noexcept {
                                    action = ant_action::do_eat; }
  inline void harvest()           noexcept {
                                    action = ant_action::do_harvest; }
  inline void drop()              noexcept {
                                    action = ant_action::do_drop; }
  inline void trace_back()        noexcept {
                                    action = ant_action::do_backtrace; }
  inline void move()              noexcept {
                                    action = ant_action::do_move; }
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__ANT_STATE_H__INCLUDED
