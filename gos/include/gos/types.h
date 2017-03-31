#ifndef GOS__TYPES_H__INCLUDED
#define GOS__TYPES_H__INCLUDED

#include <SDL.h>
#include <array>


namespace gos {

enum class orientation : int {
  none = 0,
  north,
  northeast,
  east,
  southeast,
  south,
  southwest,
  west,
  northwest
};

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

static constexpr orientation dir2or(
  const direction & dir)
{
  return (
    dir.dx == 0
    ? dir.dy == 0
      ? gos::orientation::none
      : dir.dy > 0
        ? gos::orientation::south
        : gos::orientation::north
    : dir.dx < 0
      ? dir.dy == 0
        ? gos::orientation::west
        : dir.dy < 0
          ? gos::orientation::northwest
          : gos::orientation::southwest
      : dir.dy == 0
        ? gos::orientation::east
        : dir.dy < 0
          ? gos::orientation::northeast
          : gos::orientation::southeast
  );
}

} // namespace gos

#endif // GOS__TYPES_H__INCLUDED
