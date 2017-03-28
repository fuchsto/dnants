#ifndef GOS__TYPES_H__INCLUDED
#define GOS__TYPES_H__INCLUDED

#include <SDL.h>
#include <array>


namespace gos {

struct point {
  int x;
  int y;
};

// A position in the grid.
struct position {
  int x;
  int y;
};

// For example, an ant with movement direction { 3, -2 } would move 3 cells
// west and 2 cells south every 5 ticks.
struct direction {
  int dx;
  int dy;
};

struct extents {
  int w;
  int h;
};

struct rgba {
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;
};

} // namespace gos

#endif // GOS__TYPES_H__INCLUDED
