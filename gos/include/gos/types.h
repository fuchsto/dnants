#ifndef GOS__TYPES_H__INCLUDED
#define GOS__TYPES_H__INCLUDED

#include <array>

// A position in the grid.
typedef std::array<int, 2> position;

// For example, an ant with movement direction { 3, -2 } would move 3 cells
// south and 2 cells west every 5 ticks.
typedef std::array<int, 2> direction;

#endif // GOS__TYPES_H__INCLUDED
