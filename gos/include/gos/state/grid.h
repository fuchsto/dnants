#ifndef GOS__STATE__GRID_H__INCLUDED
#define GOS__STATE__GRID_H__INCLUDED

#include <gos/state/cell.h>

#include <gos/util/logging.h>
#include <gos/types.h>

#include <vector>


namespace gos {
namespace state {

/**
 * Overall state of the grid, the playing field apart from population
 * such as obstacles, water, food and building material.
 *
 */
class grid {
  typedef grid self_t;

  gos::extents _extents;

  std::vector< std::vector<cell> > _cells;

 public:

  grid(gos::extents ext)
  : _extents(ext)
  {
    GOS__LOG_DEBUG("state::grid",
                   "initialize(" << _extents.w << "," << _extents.h << ")");
    _cells.resize(_extents.h);
    for (int r = 0; r < _extents.h; ++r) {
      _cells[r].resize(_extents.w);
    }
    GOS__LOG_DEBUG("state::grid", "initialize >");
  }

  inline int nrows() const { return _extents.h; }
  inline int ncols() const { return _extents.w; }

  inline const gos::extents & extents() const {
    return _extents;
  }

  inline bool contains_position(const gos::position & pos) const {
   return ( pos.x > 0 &&
            pos.y > 0 &&
            pos.x < _extents.w &&
            pos.y < _extents.h );
  }

  inline bool allows_move_to(const gos::position & pos) const {
    return contains_position(pos) &&
           !at(pos.x, pos.y).is_taken() &&
           !at(pos.x, pos.y).is_obstacle();
  }

  inline void set_cell_type(
    const gos::position   & pos,
    gos::state::cell_type   type) noexcept {
    _cells[pos.y][pos.x] = gos::state::cell(type);
  }

  inline const cell & operator[](const gos::position & pos) const {
    return at(pos.x, pos.y);
  }

  inline cell & operator[](const gos::position & pos) {
    return at(pos.x, pos.y);
  }

  inline const cell & at(int x, int y) const {
    return _cells[y][x];
  }

  inline cell & at(int x, int y) {
    return _cells[y][x];
  }

};

} // namespace state
} // namespace gos

#endif // GOS__STATE__GRID_H__INCLUDED
