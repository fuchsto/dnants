
from pygos import direction, ant_state as state



def update_state(cur):
    if cur.mode == state.ant_mode.scouting:
        if cur.events.enemy:
            cur.set_dir(cur.enemy_dir.dx,
                        cur.enemy_dir.dy)
            cur.attack()
        elif cur.events.collision:
            nturn = 1
            if (cur.tick_count - cur.last_dir_change == 0):
                nturn = 2
            cur.turn_dir(nturn)
        elif cur.events.food:
            if (cur.strength < 5):
                cur.eat()
                cur.set_mode(state.ant_mode.eating)
            else:
                cur.harvest()
                cur.set_mode(state.ant_mode.harvesting)
        elif cur.tick_count - cur.last_dir_change > 4:
            cur.turn_dir(((cur.rand + cur.tick_count) % 3) - 1)
            cur.move()
    elif cur.mode == state.ant_mode.eating:
        if cur.events.enemy:
            cur.set_dir(cur.enemy_dir.dx,
                        cur.enemy_dir.dy)
            cur.attack()
        elif cur.events.food:
            if (cur.strength >= 5):
                cur.harvest()
                cur.set_mode(state.ant_mode.harvesting)
            else:
                cur.eat()
        else:
            cur.set_mode(state.ant_mode.scouting)
            cur.move()
    elif cur.mode == state.ant_mode.harvesting:
        if cur.events.food:
            if cur.num_carrying < cur.strength:
                cur.harvest()
            else:
                cur.set_mode(state.ant_mode.tracing)
                cur.backtrace()
        else:
            if cur.num_carrying > 0:
                cur.set_mode(state.ant_mode.tracing)
                cur.backtrace()
            else:
                cur.set_mode(state.ant_mode.scouting)
                cur.move()
    elif cur.mode == state.ant_mode.tracing:
        if cur.num_carrying > 0:
            cur.set_mode(state.ant_mode.tracing)
            cur.backtrace()
        else:
            cur.set_mode(state.ant_mode.scouting)
            cur.move()

    cur.set_mode(state.ant_mode.scouting)
    return cur

