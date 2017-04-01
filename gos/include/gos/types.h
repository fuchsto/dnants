#ifndef GOS__TYPES_H__INCLUDED
#define GOS__TYPES_H__INCLUDED

#include <SDL.h>
#include <array>


namespace gos {

enum class orientation : unsigned char {
  none       = 0x00,
  north      = 0x01,
  south      = 0x02,
  east       = 0x20,
  west       = 0x10,
  northeast  = north | east,
  southeast  = south | east,
  southwest  = south | west,
  northwest  = north | west
};

struct point {
  int x;
  int y;

  bool operator==(const point & rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
  }
  bool operator!=(const point & rhs) const noexcept {
    return x != rhs.x || y != rhs.y;
  }
};

// A position in the grid.
struct position {
  int x;
  int y;

  bool operator==(const position & rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
  }
  bool operator!=(const position & rhs) const noexcept {
    return x != rhs.x || y != rhs.y;
  }
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

static orientation int2or(
  int o_idx)
{
  switch(o_idx) {
    case  0: return orientation::north;
    case  1: return orientation::northeast;
    case  2: return orientation::east;
    case  3: return orientation::southeast;
    case  4: return orientation::south;
    case  5: return orientation::southwest;
    case  6: return orientation::west;
    case  7: return orientation::northwest;
    default: return orientation::none;
  };
}

static int or2int(
  const orientation & ort)
{
  switch(ort) {
    case orientation::north:     return 0;
    case orientation::northeast: return 1;
    case orientation::east:      return 2;
    case orientation::southeast: return 3;
    case orientation::south:     return 4;
    case orientation::southwest: return 5;
    case orientation::west:      return 6;
    case orientation::northwest: return 7;
    default: return -1;
  };
}

static constexpr orientation dir2or(
  const direction & dir)
{
  return static_cast<orientation>(
    static_cast<unsigned char>(0x00)
    | (dir.dx > 0
       ? static_cast<unsigned char>(orientation::east)
       : dir.dx < 0
         ? static_cast<unsigned char>(orientation::west)
         : static_cast<unsigned char>(0x00))
    | (dir.dy > 0
       ? static_cast<unsigned char>(orientation::south)
       : dir.dy < 0
         ? static_cast<unsigned char>(orientation::north)
         : static_cast<unsigned char>(0x00))
    );
}

static constexpr direction or2dir(
  const orientation & ort)
{
  return direction {
    // x
    ( (static_cast<unsigned char>(ort) &
       static_cast<unsigned char>(orientation::west))
      ? -1
      : (static_cast<unsigned char>(ort) &
         static_cast<unsigned char>(orientation::east))
        ? 1
        : 0
    ),
    // y
    ( (static_cast<unsigned char>(ort) &
       static_cast<unsigned char>(orientation::north))
      ? -1
      : (static_cast<unsigned char>(ort) &
         static_cast<unsigned char>(orientation::south))
        ? 1
        : 0
    )
  };
}

} // namespace gos

#endif // GOS__TYPES_H__INCLUDED
