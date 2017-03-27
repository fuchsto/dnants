#ifndef GOS__STATE__GRID_H__INCLUDED
#define GOS__STATE__GRID_H__INCLUDED

#include <gos/types.h>
#include <gos/state/cell.h>
#include <gos/util/logging.h>

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

  int nrows() const { return _rows; }
  int ncols() const { return _cols; }

  const cell & operator[](const position & pos) const {
    return at(pos[0], pos[1]);
  }

  cell & operator[](const position & pos) {
    return at(pos[0], pos[1]);
  }

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
