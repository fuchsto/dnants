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

  int _rows;
  int _cols;

  std::vector< std::vector<cell> > _cells;

 public:

  grid(int rows, int cols)
  : _rows(rows)
  , _cols(cols)
  {
    GOS__LOG_DEBUG("state::grid",
                   "initialize(" << _rows << "," << _cols << ")");
    _cells.resize(_rows);
    for (int r = 0; r < _rows; ++r) {
      _cells[r].resize(_cols);
    }
    GOS__LOG_DEBUG("state::grid", "initialize >");
  }

  inline int nrows() const { return _rows; }
  inline int ncols() const { return _cols; }

  inline gos::extents extents() const {
    return gos::extents { _cols, _rows };
  }

  inline bool contains_position(const gos::position & pos) const {
   return ( pos.x > 0 &&
            pos.y > 0 &&
            pos.x < extents().w &&
            pos.y < extents().h );
  }

  inline bool allows_move_to(const gos::position & pos) const {
    return contains_position(pos) &&
           !at(pos.x, pos.y).is_taken() &&
           !at(pos.x, pos.y).is_obstacle();
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
