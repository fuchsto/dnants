
from pygos import direction, ant_mode, ant_action, ant_state as state, cell_state, neighbor_grid



def update_state(s,g):
    food_dir = s.dir
    for y in [ -1, 0, 1 ]:
        for x in [ -1, 0, 1 ]:
            if g.at(x, y).num_food() > 0:
                food_dir.dx = x
                food_dir.dy = y

    if s.mode == ant_mode.scouting:
        if s.events.enemy:
            s.set_dir(s.enemy_dir.dx,
                      s.enemy_dir.dy)
            s.action = ant_action.do_attack
        elif s.events.collision:
            nturn = 1
            if (s.tick_count - s.last_dir_change == 0):
                nturn = 2
            s.turn_dir(nturn)
        elif s.events.food:
            if (s.strength < 5):
                s.action = ant_action.do_eat
                s.set_mode(ant_mode.eating)
            else:
                s.action = ant_action.do_harvest
                s.set_mode(ant_mode.harvesting)
        elif food_dir != s.dir:
            s.set_dir(food_dir.dx)
            s.set_mode(ant_mode.eating)
            s.move()
        elif s.tick_count - s.last_dir_change > 4:
            s.turn_dir(((s.rand + s.tick_count) % 3) - 1)
            s.action = ant_action.do_move
    elif s.mode == ant_mode.eating:
        if s.events.enemy:
            s.set_dir(s.enemy_dir.dx,
                        s.enemy_dir.dy)
            s.action = ant_action.do_attack
        elif s.events.food:
            if (s.strength >= 5):
                s.action = ant_action.do_harvest
                s.set_mode(ant_mode.harvesting)
            else:
                s.action = ant_action.do_eat
        else:
            s.set_mode(ant_mode.scouting)
            s.action = ant_action.do_move
    elif s.mode == ant_mode.harvesting:
        if s.events.food:
            if s.num_carrying < s.strength:
                s.action = ant_action.do_harvest
            else:
                s.set_mode(ant_mode.tracing)
                s.action = ant_action.do_backtrace
        else:
            if s.num_carrying > 0:
                s.set_mode(ant_mode.tracing)
                s.action = ant_action.do_backtrace
            else:
                s.set_mode(ant_mode.scouting)
                s.action = ant_action.do_move
    elif s.mode == ant_mode.tracing:
        if s.num_carrying > 0:
            s.set_mode(ant_mode.tracing)
            s.action = ant_action.do_backtrace
        else:
            s.set_mode(ant_mode.scouting)
            s.action = ant_action.do_move

    return s

