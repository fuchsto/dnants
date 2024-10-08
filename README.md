# DNAnts

A simple ant colony simulation to teach and learn state machines and agent-based models in a cellular automaton.

<img src="doc/screenshot.png" alt="Screenshot of a match in DNAnts" width="272" height="265" />

## Running DNAnts

1. Place ant behavior state machine implementations next to the `dnants` binary.
2. Run `dnants` with one to four teams by adding command argument `-t <behavior>` for each team.

For example:

```txt
# ls -l
dnants
simple.py
greedy.py
tracer.py
# dnants -t simple -t greedy -t tracer -w 50 -h 50
```

# Dependencies

- SDL2 with development headers (e.g. `libSDL2-dev`)
- Python with development headers (e.g. `libpython-dev`), both Python 2.7 or Python 3 is fine

Experimenting with new behavior strategies does not require recompiling the application.
Behavior of ants is implemented as state generator function in Python files that are specified at runtime.
For this, we use an embedded Python interpreter provided by the excellent `pybind11` library.

Clone `pybind11` from

- https://github.com/pybind11/pybind11.git

Specify the installation path of pybind11 using the CMake option `-DPYBIND11_BASE` when building DNAnts.
