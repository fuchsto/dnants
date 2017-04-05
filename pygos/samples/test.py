
import pygos

# help(pygos)

state = pygos.ant_state()
state.set_mode(pygos.ant_state.ant_mode.eating)
state.eat()

print state.mode
print state.strength
print state.id
print state.dir
print state.events.enemy

#  if 'set_next' in locals() and 'current' in locals():
#      set_next(current)
#  else:
#      print("invalid locals")

