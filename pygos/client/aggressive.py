
from pygos import(
    direction,
    ant_mode,
    ant_action as action,
    ant_state as state,
    cell_state,
    neighbor_grid)


    


def handle_enemy(s,g):
    if s.strength >= 3:
        s.set_dir(s.enemy_dir.dx, s.enemy_dir.dy)
        s.attack()

def handle_collision(s,g):
    nturn = 1
    if s.tick_count % 4 < 2:
        nturn *= -1
    s.turn_dir(nturn)
    if g.at(s.dir.dx, s.dir.dy).is_obstacle():
        if nturn < 0:
            s.turn_dir(-1)
        else:
            s.turn_dir(1)



def init_mode(s,g):
    s.set_mode(ant_mode.scouting)
    nturn = 1
    if (s.tick_count - s.last_dir_change == 0):
        nturn -= 2
    s.turn_dir(nturn)
    s.move()


def scout_mode(s,g):
    # Prioritize attack and defense:
    if s.events.enemy:
        handle_enemy(s,g)
    else:
        if s.events.food:
            # Food found:
            if (s.strength < 7):
                s.eat();
                s.set_mode(ant_mode.eating)
            else:
                s.harvest()
                s.set_mode(ant_mode.harvesting)
        else:
            # No food:
            if s.events.collision:
                nturn = 1
                if (s.tick_count - s.last_dir_change == 0):
                    nturn -= 2
                s.turn_dir(nturn)
                s.move()
            elif s.tick_count - s.last_dir_change > 7:
                s.turn_dir(((s.rand + s.tick_count) % 3) - 1)
                s.move()

def eat_mode(s,g):
    if s.events.enemy:
        handle_enemy(s,g)
    else:
        cell_food = g.at(0,0).num_food()
        print("events.food:{} cell.num_food:{}"
                .format(s.events.food, cell_food))
        if cell_food > 0:
            # Food at current cell:
            if s.strength < 7:
                # Eat until strength threshold:
                s.eat()
            else:
                # Start harvesting:
                s.harvest()
                s.set_mode(ant_mode.harvesting)
        else:
            # No more food:
            if s.num_carrying > 0:
                # Carry to base:
                s.set_mode(ant_mode.backtracing)
                s.turn_dir(1 - (s.rand % 2) * 2)
                s.move()
            else:
                # Continue search:
                s.set_mode(ant_mode.scouting)
                s.turn_dir(1 - (s.rand % 2) * 2)
                s.move()

def harvest_mode(s,g):
    if s.events.enemy:
        handle_enemy(s,g)
    else:
        cell_food = g.at(0,0).num_food()
        print("events.food:{} cell.num_food:{}"
                .format(s.events.food, cell_food))
        if cell_food > 0:
            # Food at current cell:
            if s.num_carrying < s.strength:
                # Carry capacity available:
                s.harvest()
            else:
                # Carry capacity reached:
                s.set_mode(ant_mode.backtracing)
                s.turn_dir(3)
                s.move()
        else:
            # No more food:
            if s.num_carrying > 0:
                # Carry to base:
                s.set_mode(ant_mode.backtracing)
                s.turn_dir(1 - (s.rand % 2) * 2)
                s.move()
            else:
                # Continue search:
                s.set_mode(ant_mode.scouting)
                s.turn_dir(1 - (s.rand % 2) * 2)
                s.move()

def backtrace_mode(s,g):
    if s.num_carrying == 0:
        s.set_mode(ant_mode.scouting)
    elif s.tick_count - s.last_dir_change > 7:
        s.turn_dir(((s.rand + s.tick_count) % 3) - 1)
    s.move()


def update_state(s,g):
    if s.events.collision:
        handle_collision(s,g)

    if s.mode == ant_mode.scouting:
        scout_mode(s,g)
    elif s.mode == ant_mode.eating:
        eat_mode(s,g)
    elif s.mode == ant_mode.harvesting:
        harvest_mode(s,g)
    elif s.mode == ant_mode.backtracing:
        backtrace_mode(s,g)
    else:
        init_mode(s,g)
    return s

