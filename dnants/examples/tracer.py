
from pygos import(
    direction,
    ant_mode,
    ant_action as action,
    ant_state as state,
    cell_state,
    neighbor_grid)

#   ant.mode
#        { ant_mode.waiting,     - warten
#          ant_mode.scouting,    - Zucker suchen
#          ant_mode.eating,      - essen
#          ant_mode.harvesting,  - ernten
#          ant_mode.homing       - zurueck zur Basis
#        }
#
#   ant.set_mode(ant_mode)       - setze Modus in naechster Runde
#
#   ant.dir:         - zum Beispie: x:0,y:1 = unten, x:-1,y:-1 = links oben
#        .x
#        .y
#
#   ant.set_dir(x,y) - Setze Richtung der Ameise
#   ant.turn(d)      - Drehe Richtung der Ameise, z.B. d=-2 => 90 Grad links,
#                      d=4 => umkehren, d=1 => 45 Grad rechts
#   
#   ant.strength     - Staerke der Ameise (1-10)
#   ant.damage       - in letzter Runde erlittener Schaden
#   ant.num_carrying - Einheiten Zucker, die die Ameise transportiert
#   
#   ant.events:
#        .enemy      - Feind in Nachbarzelle
#        .food       - Zucker in aktueller Zelle
#        .attacked   - wurde angegriffen
#        .collision  - letzte Bewegung wurde durch Hindernis blockiert
#
#   ant.dist:        - Entfernung (Pfadintegral) von Basis
#        .x
#        .y
#   
#   ant.idle()       - tue nichts
#   ant.attack()     - greife Zelle in aktueller Richtung an
#   ant.move()       - bewege zu Zelle in aktueller Richtung
#   ant.harvest()    - ernte eine Einheit Zucker
#   ant.drop(n)      - lasse n Einheiten Zucker in der aktuellen Zelle fallen
#   ant.eat()        - iss eine Einheit Zucker
#   


harvest_min_strength = 5
attack_min_strength  = 5


def handle_enemy(ant,g):
    global attack_min_strength
    if ant.strength >= attack_min_strength:
        ant.set_dir(ant.enemy_dir.x, ant.enemy_dir.y)
        ant.attack()
    else:
        ant.set_dir(-ant.enemy_dir.x, -ant.enemy_dir.y)
        ant.move()

def random_turn(ant,g):
    if ant.dir.x == 0 and ant.dir.y == 0:
        ant.set_dir(0,1)
    nturn = 1
    if (ant.tick_count - (ant.id * 3)) % 4 < 2:
        nturn += 1
    if (ant.tick_count + ant.id) % 8 < 6:
        nturn *= -1
    # ant.turn_dir(1)
    ant.turn_dir(nturn)


def follow_food(ant,g):
    food_dir = direction()
    food_dir.x = 0
    food_dir.y = 0
    for x in [ -1,0,1 ]:
        for y in [ -1,0,1 ]:
            if (x != 0 or y != 0) and g.at(x,y).num_food() > 0:
                ant.set_dir(x,y)
                return True
    return False


def follow_trace(ant,g):
    best_trace_dir = direction()
    best_trace_dir.x = 0
    best_trace_dir.y = 0
    best_trace_val = 0
    scnd_trace_val = 0
    scnd_trace_dir = best_trace_dir
    for x in [ -1,0,1 ]:
        for y in [ -1,0,1 ]:
            if x != 0 or y != 0:
                fwd_trace = g.out_trace(x,y)
                bwd_trace = g.in_trace(x,y)
                if fwd_trace > 0 and bwd_trace > 0:
                    trace_val = abs((2 * abs(fwd_trace + bwd_trace)) -
                                    (8 * abs(fwd_trace - bwd_trace)))
                    # print("ant {} tracing({},{}) fwd({}) bwd({}) -> {}"
                    #         .format(ant.id, x, y, fwd_trace, bwd_trace,
                    #                 trace_val))
                    if trace_val > best_trace_val and (
                            x != -ant.dir.x or y != -ant.dir.y):
                        scnd_trace_val = best_trace_val
                        scnd_trace_dir = best_trace_dir
                        best_trace_val = trace_val
                        best_trace_dir.x = x
                        best_trace_dir.y = y
    if best_trace_dir.x != 0 or best_trace_dir.y != 0:
        ant.set_dir(best_trace_dir.x, best_trace_dir.y)
        return True
    else:
        return False



def init_mode(ant,g):
    ant.set_mode(ant_mode.scouting)
    if not follow_trace(ant,g):
        random_turn(ant,g)
        nturn = 1
        if (ant.tick_count - ant.last_dir_change == 0):
            nturn -= 2
        ant.turn_dir(nturn)
    ant.move()


def scout_mode(ant,g):
    global harvest_min_strength
    # Prioritize attack and defense:
    if ant.events.enemy:
        handle_enemy(ant,g)
    else:
        if ant.events.food:
            # Food found:
            if (ant.strength < harvest_min_strength):
                ant.eat();
                ant.set_mode(ant_mode.eating)
            else:
                ant.harvest()
                ant.set_mode(ant_mode.harvesting)
        else:
            # No food:
            if ant.events.collision:
                random_turn(ant,g)
                ant.move()
            elif follow_food(ant,g):
                ant.move()
            elif follow_trace(ant,g):
                ant.move()
            elif (ant.tick_count - ant.last_dir_change > 7):
                random_turn(ant,g)
                ant.move()

def eat_mode(ant,g):
    global harvest_min_strength
    if ant.events.enemy:
        handle_enemy(ant,g)
    else:
        if ant.events.food:
            # Food at current cell:
            if ant.strength < harvest_min_strength:
                # Eat until strength threshold:
                ant.eat()
            else:
                # Start harvesting:
                ant.harvest()
                ant.set_mode(ant_mode.harvesting)
        else:
            # No more food:
            if ant.num_carrying > 0:
                # Carry to base:
                ant.set_mode(ant_mode.homing)
                ant.turn_dir(4)
                ant.move()
            else:
                # Continue search:
                ant.set_mode(ant_mode.scouting)
                random_turn(ant,g)
                ant.move()

def harvest_mode(ant,g):
    if ant.events.food:
        # Food at current cell:
        if ant.num_carrying < ant.strength:
            # Carry capacity available:
            ant.harvest()
        else:
            # Carry capacity reached:
            ant.set_mode(ant_mode.homing)
            ant.turn_dir(3)
            ant.move()
    else:
        # No more food:
        if ant.num_carrying > 0:
            # Carry to base:
            ant.set_mode(ant_mode.homing)
            random_turn(ant,g)
            ant.move()
        else:
            # Continue search:
            ant.set_mode(ant_mode.scouting)
            random_turn(ant,g)
            ant.move()

def backtrace_mode(ant,g):
    if ant.num_carrying == 0:
        ant.set_mode(ant_mode.scouting)
        follow_trace(ant,g)
        ant.move()
    else:
        if ant.events.collision:
            random_turn(ant,g)
            ant.move()
        else:
            if ant.dist.x < 0:
                ant.set_dir( 1, ant.dir.y)
            elif ant.dist.x > 0:
                ant.set_dir(-1, ant.dir.y)
            else:
                ant.set_dir( 0, ant.dir.y)
            if ant.dist.y < 0:
                ant.set_dir(ant.dir.x,  1)
            elif ant.dist.y > 0:
                ant.set_dir(ant.dir.x, -1)
            else:
                ant.set_dir(ant.dir.x,  0)
            ant.move()


def update_state(ant,g):
    if ant.mode == ant_mode.scouting:
        scout_mode(ant,g)
    elif ant.mode == ant_mode.eating:
        eat_mode(ant,g)
    elif ant.mode == ant_mode.harvesting:
        harvest_mode(ant,g)
    elif ant.mode == ant_mode.homing:
        backtrace_mode(ant,g)
    else:
        init_mode(ant,g)
    return ant

