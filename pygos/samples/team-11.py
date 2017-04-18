
from pygos import(
    direction,
    ant_mode,
    ant_action as action,
    ant_state as state,
    cell_state,
    neighbor_grid)

#   ant.mode
#        {
#          ant_mode.waiting,     - warten
#          ant_mode.scouting,    - Zucker suchen
#          ant_mode.eating,      - essen
#          ant_mode.harvesting,  - ernten
#          ant_mode.homing       - zurueck zur Basis
#        }
#
#   ant.set_mode(ant_mode)       - setze Modus fuer naechste Runde
#
#   ant.dir:            - zum Beispie: x:0,y:1 = unten, 
#        .x               x:-1,y:-1 = links oben
#        .y
#
#   ant.set_dir(x,y)    - Setze Richtung der Ameise
#   ant.turn(d)         - Drehe Richtung der Ameise, z.B.:
#                         d=-2 => 90 Grad links,
#                         d=4 => umkehren,
#                         d=1 => 45 Grad rechts
#   
#   ant.strength        - Staerke der Ameise (1-10)
#   ant.damage          - in letzter Runde erlittener Schaden
#   ant.num_carrying    - Einheiten Zucker, die die Ameise transportiert
#   ant.tick_count      - Rundenzaehler
#   
#   ant.events:         - Ereignisse:
#        .enemy           - Feind in Nachbarzelle
#        .food            - Zucker in aktueller Zelle
#        .attacked        - wurde angegriffen
#        .collision       - letzte Bewegung wurde durch Hindernis blockiert
#
#   ant.dist:           - Entfernung (Pfadintegral) von Basis
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
#   grid.out_trace(x,y) - Intensitaet von ausgehender Spur in
#                         Richtung (x,y) aus aktueller Zelle
#   grid.in_trace(x,y)  - Intensitaet von eingehender Spur aus
#                         Richtung (x,y) in aktueller Zelle
#   grid.at(x,y)        - Zustand von Nachbarzelle abfragen
#     -> cell
#
#   cell.is_taken()     - Zelle von anderer Ameise besetzt
#   cell.is_obstacle()  - Zelle von Hindernis blockiert
#   cell.ant()          - Falls Zelle besetzt, gibt Id der Ameise in der
#                         Zelle zurueck, oder ungueltige (-1, -1) Id falls
#                         nicht besetzt
#     -> ant_id:
#          .team_id     - Eindeutige Nummer des Teams der Ameise
#                         Also: wenn ant.team_id != cell.ant().team_id,
#                         dann ist in Zelle `cell` eine gegnerische
#                         Ameise
#          .id          - Eindeutige Nummer der Ameise innerhalb ihres
#                         Teams
#   cell.num_food()

def random_turn(ant,grid):
    if ant.dir.x == 0 and ant.dir.y == 0:
        ant.set_dir(0,1)
    nturn = 1
    if (ant.tick_count - (ant.id * 3)) % 4 < 2:
        nturn += 1
    if (ant.tick_count + ant.id) % 8 < 6:
        nturn *= -1
    ant.turn_dir(nturn)

def search_adj_food(ant,grid):
    for y in [ -1, 0, 1 ]:
        for x in [ -1, 0, 1 ]:
            if grid.at(x, y).num_food() > 0 and (x != 0 or y != 0):
                ant.set_dir(x,y)
                return True
    return False

def update_state(ant,grid):
    if ant.mode == ant_mode.scouting:
        if ant.events.collision:
            ant.turn_dir(-1)
            ant.move()
        elif ant.events.enemy:
            if ant.strength > 7:
                ant.set_dir(ant.enemy_dir.x, ant.enemy_dir.y)
                ant.attack()
            else:
                ant.set_dir(-ant.enemy_dir.x, -ant.enemy_dir.y)
                ant.move()
        elif ant.events.food:
            ant.set_mode(ant_mode.eating)
            ant.eat()
        elif not search_adj_food(ant,grid):
            if ant.id < 4 and ant.tick_count < 7 + ant.id*20:
                ant.set_dir((-1 + 2*(ant.id % 2)), (-1 + 2*(ant.id % 2)))
                if ant.id < 2:
                    ant.set_dir((-1 + 2*(ant.id % 2)), -(-1 + 2*(ant.id % 2)))
            else:
                if ant.tick_count % 6 == 0:
                    ant.turn_dir(1)
                elif ant.tick_count % 3 == 0:
                    ant.turn_dir(-1)
            ant.move()
    elif ant.mode == ant_mode.eating:
        if ant.strength < 10:
            ant.eat()
        else:
            ant.set_mode(ant_mode.harvesting)
            ant.harvest()
    elif ant.mode == ant_mode.harvesting:
        if ant.strength > ant.num_carrying and ant.events.food:
            ant.harvest()
        else:
            ant.set_mode(ant_mode.homing)
            ant.turn_dir(4)
            ant.move()
    elif ant.mode == ant_mode.homing:
        xCor = ant.dist.x
        xCorBe = abs(ant.dist.x)
        yCor = ant.dist.y
        yCorBe = abs(ant.dist.y)
        if xCorBe == 0:
            xCorBe = 1
        if yCorBe == 0:
            yCorBe = 1
        ant.set_dir(-(xCor/xCorBe), -(yCor/yCorBe))
        if ant.events.collision:
            ant.turn_dir(ant.tick_count % 8)
        ant.move()
    return ant

