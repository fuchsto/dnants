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
   return ( pos.x >= 0 &&
            pos.y >= 0 &&
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

  void update() {
    for (int y = 0; y < _extents.h; ++y) {
      for (int x = 0; x < _extents.w; ++x) {
        _cells[y][x].update();
      }
    }
  }
};

class neighbor_grid {
  typedef neighbor_grid self_t;

  const grid & _grid;
  position     _pos;
  int          _team_id;
  int          _tick;
  cell_state   _nil_cell;

 public:
  neighbor_grid()                               = delete;
  neighbor_grid(const self_t & other)           = default;
  neighbor_grid(self_t && other)                = default;
  neighbor_grid & operator=(const self_t & rhs) = default;
  neighbor_grid & operator=(self_t && rhs)      = default;
  neighbor_grid(
    const grid     & g,
    const position & pos,
    int              team_id,
    int              tick)
  : _grid(g)
  , _pos(pos)
  , _team_id(team_id)
  , _tick(tick)
  , _nil_cell(cell_type::barrier)
  { }

  inline const cell_state & state_at(int x, int y) const {
    auto nb_pos = neighbor(x,y);
    GOS__LOG_DEBUG("neighbor_grid", "state_at" <<
                   "(" << nb_pos.x <<
                   "," << nb_pos.y << ")");
    return ( _grid.contains_position(nb_pos)
             ? _grid[nb_pos].state()
             : _nil_cell );
  }

  int in_trace(int dx, int dy) const noexcept {
    GOS__LOG_DEBUG("neighbor_grid",
                   "team:" << _team_id << " " <<
                   "tick:" << _tick << " " <<
                   "in_trace(" << dx << "," << dy << ") " <<
                   "oidx:" << gos::dir2int({ dx, dy }));
    if (dx == 0 && dy == 0) {
      return 0;
    }
    const auto & cs = state_at(0,0);
    GOS__LOG_DEBUG("neighbor_grid", "got cell state ...");
    if (cs.type() == cell_type::barrier) {
      return 0;
    }
    const auto & tc = cs.in_traces(_team_id);
    GOS__LOG_DEBUG("neighbor_grid", "got traces ...");
    return 0; // tc[gos::dir2int({ dx, dy })] - _tick;
  }

  int out_trace(int dx, int dy) const noexcept {
    GOS__LOG_DEBUG("neighbor_grid",
                   "team:" << _team_id << " " <<
                   "tick:" << _tick << " " <<
                   "out_trace(" << dx << "," << dy << ") " <<
                   "oidx:" << gos::dir2int({ dx, dy }));
    if (dx == 0 && dy == 0) {
      return 0;
    }
    const auto & cs = state_at(0,0);
    GOS__LOG_DEBUG("neighbor_grid", "got cell state ...");
    if (cs.type() == cell_type::barrier) {
      return 0;
    }
    const auto & tc = cs.out_traces(_team_id);
    GOS__LOG_DEBUG("neighbor_grid", "got traces ...");
    return 0; // tc[gos::dir2int({ dx, dy })] - _tick;
  }

 private:
  position neighbor(int x, int y) const noexcept {
    return position { _pos.x + std::max(-1, std::min(x, 1)),
                      _pos.y + std::max(-1, std::min(y, 1)) };
  }
};


} // namespace state
} // namespace gos

#endif // GOS__STATE__GRID_H__INCLUDED
