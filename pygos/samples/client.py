
import pygos

def update_state(current):
    current.set_mode(ant_state.ant_mode.scouting)
    current.set_dir(direction(1,1))
    current.move()
    return current

