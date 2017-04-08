#ifndef GOS__STATE__CELL_STATE_H__INCLUDED
#define GOS__STATE__CELL_STATE_H__INCLUDED

#include <gos/state/ant_state.h>
#include <gos/types.h>


namespace gos {
namespace state {

class cell;

enum cell_type : int {
  plain = 0,
  none,
  barrier,
  spawn_point,
  grass,
  water,
  food
};

class cell_state {
  friend cell;

 public:
  typedef int trace;

  typedef std::array<trace, 8> traces;

 private:
  cell_type               _type       = cell_type::plain;
  int                     _amount     = 0;
  bool                    _taken      = false;
  gos::state::ant_id      _ant_id     { -1, -1 };
  std::array<traces, 4>   _traces_in  {{ }};
  std::array<traces, 4>   _traces_out {{ }};

 public:
  cell_state(cell_type ct, int nfood = 0)
  : _type(ct)
  , _amount(nfood)
  { }

  cell_state()                                   = delete;
  cell_state(const cell_state & other)           = default;
  cell_state(cell_state && other)                = default;
  cell_state & operator=(const cell_state & rhs) = default;
  cell_state & operator=(cell_state && rhs)      = default;

  cell_type type() const noexcept {
    return _type;
  }

  bool is_obstacle() const noexcept {
    return ( _type == cell_type::barrier );
  }

  int take_food(int ntake = 1) noexcept {
    int consumed = std::min<int>(1, _amount);
    _amount -= consumed;
    return consumed;
  }

  void drop_food(int amount) noexcept {
    _amount += amount;
  }

  inline int num_food() const noexcept {
    return _amount;
  }

  bool is_taken() const noexcept {
    return _taken;
  }

  const traces & in_traces(int team_id) const noexcept {
    return _traces_in[team_id];
  }

  const traces & out_traces(int team_id) const noexcept {
    return _traces_out[team_id];
  }

  gos::state::ant_id ant() const {
    return (_taken ? _ant_id : gos::state::ant_id { -1, -1 });
  }
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__CELL_STATE_H__INCLUDED
