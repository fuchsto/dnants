#ifndef GOS__TYPES_H__INCLUDED
#define GOS__TYPES_H__INCLUDED

#include <SDL.h>
#include <array>


namespace gos {

// A position in the grid.
typedef std::array<int, 2> position;

// For example, an ant with movement direction { 3, -2 } would move 3 cells
// south and 2 cells west every 5 ticks.
typedef std::array<int, 2> direction;

struct extents {
  int w;
  int h;
};

struct point {
  int x;
  int y;
};

struct rgba {
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;
};

} // namespace gos

#endif // GOS__TYPES_H__INCLUDED
