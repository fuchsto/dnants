
from pygos import direction, ant_state as state


def update_state(cur):
    if cur.tick_count - cur.last_dir_change > 4:
        cur.turn_dir(((cur.rand + cur.tick_count) % 3) - 1)
        cur.action = state.ant_action.do_move
    return cur

