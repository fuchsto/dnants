
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
#   ant.turn(d)      - Drehe Richtung der Ameise, z.B.:
#                      d=-2 => 90 Grad links,
#                      d=4 => umkehren,
#                      d=1 => 45 Grad rechts
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
#   ant.idle()          - tue nichts
#   ant.attack()        - greife Zelle in aktueller Richtung an
#   ant.move()          - bewege zu Zelle in aktueller Richtung
#   ant.harvest()       - ernte eine Einheit Zucker
#   ant.drop(n)         - lasse n Zucker in der aktuellen Zelle fallen
#   ant.eat()           - iss eine Einheit Zucker
#
#   grid.out_trace(x,y) - Intensitaet von ausgehender Spur in Richtung (x,y)
#   grid.in_trace(x,y)  - Intensitaet von eingehender Spur aus Richtung (x,y)
#   grid.at(x,y)        - Zustand von Nachbarzelle abfragen
#     -> cell
#
#   cell.is_taken()
#   cell.is_obstacle()
#   cell.ant()
#   cell.num_food()

def random_turn(ant,g):
    if ant.dir.x == 0 and ant.dir.y == 0:
        ant.set_dir(0,1)
    nturn = 1
    if (ant.tick_count - (ant.id * 3)) % 4 < 2:
        nturn += 1
    if (ant.tick_count + ant.id) % 8 < 6:
        nturn *= -1
    ant.turn_dir(nturn)


def update_state(ant,g):
    food_dir = ant.dir
    for y in [ -1, 0, 1 ]:
        for x in [ -1, 0, 1 ]:
            if g.at(x, y).num_food() > 0:
                food_dir.x = x
                food_dir.y = y

    if ant.mode == ant_mode.scouting:
        if ant.events.enemy:
            ant.set_dir(ant.enemy_dir.x,
                      ant.enemy_dir.y)
            ant.attack()
        elif ant.events.collision:
            ant.turn_dir(1)
            ant.move()
        elif ant.events.food:
            if (ant.strength < 5):
                ant.eat()
                ant.set_mode(ant_mode.eating)
            else:
                ant.harvest()
                ant.set_mode(ant_mode.harvesting)
        elif food_dir.x != 0 and food_dir.y != 0:
            ant.set_dir(food_dir.x, food_dir.y)
            ant.move()
        elif ant.tick_count - ant.last_dir_change > 7:
            ant.turn_dir(((ant.rand + ant.tick_count) % 3) - 1)
            ant.move()
    elif ant.mode == ant_mode.eating:
        if ant.events.enemy:
            ant.set_dir(ant.enemy_dir.x,
                      ant.enemy_dir.y)
            ant.attack()
        elif ant.events.food:
            if (ant.strength >= 5):
                ant.harvest()
                ant.set_mode(ant_mode.harvesting)
            else:
                ant.eat()
        else:
            ant.set_mode(ant_mode.scouting)
            ant.move()
    elif ant.mode == ant_mode.harvesting:
        if ant.events.food:
            if ant.num_carrying < ant.strength:
                ant.harvest()
            else:
                ant.set_mode(ant_mode.homing)
                ant.move()
        else:
            if ant.num_carrying > 0:
                ant.set_mode(ant_mode.homing)
                ant.move()
            else:
                ant.set_mode(ant_mode.scouting)
                ant.move()
    elif ant.mode == ant_mode.homing:
        if ant.events.collision:
            ant.turn_dir(1)
            ant.move()
        elif ant.num_carrying == 0:
            ant.set_mode(ant_mode.scouting)
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
    return ant

