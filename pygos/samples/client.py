
import pygos

def update_state(current):
    current.set_mode(pygos.ant_state.ant_mode.scouting)
    current.set_dir(pygos.direction(1,1))
    current.move()
    return current

