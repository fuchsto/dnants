
from pygos import(
    direction,
    ant_mode,
    ant_action as action,
    ant_state as state,
    cell_state,
    neighbor_grid)

def random_turn(s,g):
    if s.dir.x == 0 and s.dir.y == 0:
        s.set_dir(0,1)
    nturn = 1
    if (s.tick_count - (s.id * 3)) % 4 < 2:
        nturn += 1
    if (s.tick_count + s.id) % 8 < 6:
        nturn *= -1
    s.turn_dir(nturn)


def update_state(s,g):
    food_dir = s.dir
    for y in [ -1, 0, 1 ]:
        for x in [ -1, 0, 1 ]:
            if g.at(x, y).num_food() > 0:
                food_dir.x = x
                food_dir.y = y

    if s.mode == ant_mode.scouting:
        if s.events.enemy:
            s.set_dir(s.enemy_dir.x,
                      s.enemy_dir.y)
            s.attack()
        elif s.events.collision:
            s.turn_dir(1)
            s.move()
        elif s.events.food:
            if (s.strength < 5):
                s.eat()
                s.set_mode(ant_mode.eating)
            else:
                s.harvest()
                s.set_mode(ant_mode.harvesting)
        elif food_dir.x != 0 and food_dir.y != 0:
            s.set_dir(food_dir.x, food_dir.y)
            s.move()
        elif s.tick_count - s.last_dir_change > 7:
            s.turn_dir(((s.rand + s.tick_count) % 3) - 1)
            s.move()
    elif s.mode == ant_mode.eating:
        if s.events.enemy:
            s.set_dir(s.enemy_dir.x,
                      s.enemy_dir.y)
            s.attack()
        elif s.events.food:
            if (s.strength >= 5):
                s.harvest()
                s.set_mode(ant_mode.harvesting)
            else:
                s.eat()
        else:
            s.set_mode(ant_mode.scouting)
            s.move()
    elif s.mode == ant_mode.harvesting:
        if s.events.food:
            if s.num_carrying < s.strength:
                s.harvest()
            else:
                s.set_mode(ant_mode.backtracing)
                s.move()
        else:
            if s.num_carrying > 0:
                s.set_mode(ant_mode.backtracing)
                s.move()
            else:
                s.set_mode(ant_mode.scouting)
                s.move()
    elif s.mode == ant_mode.backtracing:
        if s.num_carrying == 0:
            s.set_mode(ant_mode.scouting)
            random_turn(s,g)
            s.move()
        else:
            if s.events.collision:
                s.turn_dir(1)
                s.move()
            else:
                if s.dist.x < 0:
                    s.set_dir( 1, s.dir.y)
                elif s.dist.x > 0:
                    s.set_dir(-1, s.dir.y)
                else:
                    s.set_dir( 0, s.dir.y)
                if s.dist.y < 0:
                    s.set_dir(s.dir.x,  1)
                elif s.dist.y > 0:
                    s.set_dir(s.dir.x, -1)
                else:
                    s.set_dir(s.dir.x,  0)
                s.move()

    return s

