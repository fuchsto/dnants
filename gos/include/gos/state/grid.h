#ifndef GOS__STATE__GRID_H__INCLUDED
#define GOS__STATE__GRID_H__INCLUDED

#include <gos/state/cell.h>


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
  { }

  const cell & at(int row, int col) const {
    return _cells[row][col];
  }

  cell & at(int row, int col) {
    return _cells[row][col];
  }


};

} // namespace state
} // namespace gos

#endif // GOS__STATE__GRID_H__INCLUDED
