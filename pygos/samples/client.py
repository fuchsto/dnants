
from pygos import(
    direction,
    ant_mode,
    ant_action as action,
    ant_state as state,
    cell_state,
    neighbor_grid)


harvest_min_strength = 5
attack_min_strength  = 5


def handle_enemy(s,g):
    global attack_min_strength
    if s.strength >= attack_min_strength:
        s.set_dir(s.enemy_dir.x, s.enemy_dir.y)
        s.attack()
    else:
        s.set_dir(-s.enemy_dir.x, -s.enemy_dir.y)
        s.move()

def random_turn(s,g):
    if s.dir.x == 0 and s.dir.y == 0:
        s.set_dir(0,1)
    nturn = 1
    if s.tick_count % 4 < 2:
        nturn *= -1
    s.turn_dir(1)


def follow_food(s,g):
    food_dir = direction()
    food_dir.x = 0
    food_dir.y = 0
    for x in [ -1,0,1 ]:
        for y in [ -1,0,1 ]:
            if (x != 0 or y != 0) and g.at(x,y).num_food() > 0:
                s.set_dir(x,y)
                return True
    return False


def follow_trace(s,g):
    best_trace_dir = direction()
    best_trace_dir.x = 0
    best_trace_dir.y = 0
    best_trace_val = 0
    scnd_trace_val = 0
    scnd_trace_dir = best_trace_dir
    for x in [ -1,0,1 ]:
        for y in [ -1,0,1 ]:
            fwd_trace = g.out_trace(x,y)
            bwd_trace = g.in_trace(x,y)
            if fwd_trace > 0 and bwd_trace > 0:
                trace_val = (((fwd_trace + bwd_trace) / 2)**2 -
                             (fwd_trace - bwd_trace)**2 )
                if trace_val > best_trace_val and (
                        x != -s.dir.x and y != -s.dir.y):
                    scnd_trace_val = best_trace_val
                    scnd_trace_dir = best_trace_dir
                    best_trace_val = trace_val
                    best_trace_dir.x = x
                    best_trace_dir.y = y
                    print("follow_trace({},{}) : f:{} b:{}"
                            .format(best_trace_dir.x, best_trace_dir.y,
                                    fwd_trace, bwd_trace))
    if scnd_trace_dir.x != 0 or scnd_trace_dir.y != 0:
        s.set_dir(scnd_trace_dir.x, scnd_trace_dir.y)
        print("follow_trace -> dir({},{})"
                .format(scnd_trace_dir.x, scnd_trace_dir.y))
        return True
    else:
        return False



def init_mode(s,g):
    s.set_mode(ant_mode.scouting)
    if not follow_trace(s,g):
        s.set_dir(0,1)
        nturn = 1
        if (s.tick_count - s.last_dir_change == 0):
            nturn -= 2
        s.turn_dir(nturn)
    s.move()


def scout_mode(s,g):
    global harvest_min_strength
    # Prioritize attack and defense:
    if s.events.enemy:
        handle_enemy(s,g)
    else:
        if s.events.food:
            # Food found:
            if (s.strength < harvest_min_strength):
                s.eat();
                s.set_mode(ant_mode.eating)
            else:
                s.harvest()
                s.set_mode(ant_mode.harvesting)
        else:
            # No food:
            if s.events.collision:
                random_turn(s,g)
                s.move()
            elif follow_food(s,g):
                s.move()
            elif follow_trace(s,g):
                s.turn_dir(4)
                s.move()
            elif (s.tick_count - s.last_dir_change > 4):
                random_turn(s,g)
                s.move()

def eat_mode(s,g):
    global harvest_min_strength
    if s.events.enemy:
        handle_enemy(s,g)
    else:
        # cell_food = g.at(0,0).num_food()
        # print("events.food:{} cell.num_food:{}"
        #         .format(s.events.food, cell_food))
        if s.events.food:
            # Food at current cell:
            if s.strength < harvest_min_strength:
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
    # cell_food = g.at(0,0).num_food()
    # print("events.food:{} cell.num_food:{}"
    #         .format(s.events.food, cell_food))
    if s.events.food:
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
            s.turn_dir(4)
            s.move()

def backtrace_mode(s,g):
    if s.num_carrying == 0:
        s.set_mode(ant_mode.scouting)
        s.move()
    else:
        if s.events.collision:
            random_turn(s,g)
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


def update_state(s,g):
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

