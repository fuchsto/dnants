
#include <gos/default_rules.h>

#include <gos/state/ant_state.h>
#include <gos/types.h>


namespace gos {

gos::state::ant_state on_enemy(
  gos::state::ant_state & state)
{
}

gos::state::ant_state update_ant(
  const gos::state::ant_state & current)
{
  using gos::state::ant_state;

  gos::state::ant_state next = current;

  switch (current.mode) {
    case ant_state::ant_mode::scouting:
      if (current.events.collision) {
        if (true || next.rand % 6 <= 2) { next.set_turn(-1); }
        else                            { next.set_turn(1);  }
        next.move();
      } else if (current.events.food) {
        if (current.strength < 7) {
          next.eat();
          next.set_mode(ant_state::ant_mode::eating);
        } else {
          next.harvest();
          next.set_mode(ant_state::ant_mode::harvesting);
        }
      } else if (current.tick_count - current.last_dir_change > 4) {
        next.set_turn(
          ((next.rand + next.tick_count) % 3) - 1);
        next.move();
      }
      break;
    case ant_state::ant_mode::eating:
      if (current.events.enemy) {
        next.set_dir(current.enemy_dir);
        next.attack();
      } else {
        if (current.strength >= 7) {
          next.harvest();
          next.set_mode(ant_state::ant_mode::harvesting);
        } else {
          next.eat();
        }
      }
      break;
    case ant_state::ant_mode::harvesting:
      next.harvest();
      break;
    case ant_state::ant_mode::tracing:
      next.backtrace();
      break;
    default:
      break;
  }

  return next;
}

} // namespace gos

