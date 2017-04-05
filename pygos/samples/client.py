
from pygos import direction, ant_state as state



def update_state(cur):
    if cur.mode == state.ant_mode.scouting:
        if cur.events.enemy:
            cur.set_dir(cur.enemy_dir.dx,
                        cur.enemy_dir.dy)
            cur.action = state.ant_action.do_attack
        elif cur.events.collision:
            nturn = 1
            if (cur.tick_count - cur.last_dir_change == 0):
                nturn = 2
            cur.turn_dir(nturn)
        elif cur.events.food:
            if (cur.strength < 5):
                cur.action = state.ant_action.do_eat
                cur.mode   = state.ant_mode.eating
            else:
                cur.action = state.ant_action.do_harvest
                cur.mode   = state.ant_mode.harvesting
        elif cur.tick_count - cur.last_dir_change > 4:
            cur.turn_dir(((cur.rand + cur.tick_count) % 3) - 1)
            cur.action = state.ant_action.do_move
    elif cur.mode == state.ant_mode.eating:
        if cur.events.enemy:
            cur.set_dir(cur.enemy_dir.dx,
                        cur.enemy_dir.dy)
            cur.action = state.ant_action.do_attack
        elif cur.events.food:
            if (cur.strength >= 5):
                cur.action = state.ant_action.do_harvest
                cur.mode   = state.ant_mode.harvesting
            else:
                cur.action = state.ant_action.do_eat
        else:
            cur.mode   = state.ant_mode.scouting
            cur.action = state.ant_action.do_move
    elif cur.mode == state.ant_mode.harvesting:
        if cur.events.food:
            if cur.num_carrying < cur.strength:
                cur.action = state.ant_action.do_harvest
            else:
                cur.mode   = state.ant_mode.tracing
                cur.action = state.ant_action.do_backtrace
        else:
            if cur.num_carrying > 0:
                cur.mode   = state.ant_mode.tracing
                cur.action = state.ant_action.do_backtrace
            else:
                cur.mode   = state.ant_mode.scouting
                cur.action = state.ant_action.do_move
    elif cur.mode == state.ant_mode.tracing:
        if cur.num_carrying > 0:
            cur.mode   = state.ant_mode.tracing
            cur.action = state.ant_action.do_backtrace
        else:
            cur.mode   = state.ant_mode.scouting
            cur.action = state.ant_action.do_move

    return cur

