# DNAnts

A simple ant colony simulation to teach and learn state machines and agent-based models in a cellular automaton.

<img src="doc/screenshot.png" alt="Screenshot of a match in DNAnts" width="272" height="265" />

# Dependencies

Experimenting with new behavior strategies does not require recompiling the application.
Behavior of ants is implemented as state generator function in Python files that are specified at runtime.
For this, DNAnts requires an extension to pybind11 which is not merged to the official pybind11 repository, yet.

The implementation is very reliable, however.
Clone branch `embedded` from

- https://github.com/fuchsto/pybind11.git (known good state)
- https://github.com/dean0x7d/pybind11.git (official pull request)

Specify the installation path of pybind11 using the CMake option `-DPYBIND11_BASE` when building DNAnts.
