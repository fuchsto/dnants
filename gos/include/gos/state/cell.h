#ifndef GOS__STATE__CELL_H__INCLUDED
#define GOS__STATE__CELL_H__INCLUDED

#include <gos/state/ant.h>

#include <memory>


namespace gos {
namespace state {

enum cell_type : int {
  plain = 0,
  barrier,
  spawn_point,
  grass,
  water,
  material,
  food
};


class cell;

class cell_state {

 public:
  typedef std::array<int, 8> traces;

 private:
  cell                  & _cell;
  bool                    _taken  = false;
  gos::state::ant       * _ant    = nullptr;
  std::array<traces, 4>   _traces = {{ }};

 public:
  cell_state(cell & c)
  : _cell(c)
  , _ant(nullptr)
  { }

  virtual ~cell_state() { }

  virtual void on_enter(ant &, const gos::state::game_state &);
  virtual void on_exit(ant &, const gos::state::game_state &);

  virtual bool is_obstacle() const noexcept {
    return false;
  }

  bool is_taken() const noexcept {
    return _taken;
  }

  const traces & get_traces(int team_id) const noexcept {
    return _traces[team_id];
  }

  gos::state::ant * ant() {
    return (_taken ? _ant : nullptr);
  }

  void add_trace(
    int              team_id,
    gos::orientation ort,
    int              round_count) noexcept {
    _traces[team_id][or2int(ort)] = round_count;
  }
};

class resource_cell_state : public cell_state {
  int _amount = 1;
 public:
  resource_cell_state(cell & c, int amount = 1)
  : cell_state(c)
  , _amount(amount)
  { }

  virtual ~resource_cell_state() { }

  virtual void on_enter(gos::state::ant &, const gos::state::game_state &);
  virtual void on_exit(gos::state::ant &, const gos::state::game_state &);

  int amount_left() const noexcept {
    return _amount;
  }
  bool is_depleted() const noexcept {
    return amount_left() == 0;
  }

  int consume() noexcept {
    int consumed = std::min<int>(1, _amount);
    _amount -= consumed;
    return consumed;
  }
};

class plain_cell_state : public cell_state {
 public:
  plain_cell_state(cell & c)
  : cell_state(c)
  { }
};

class barrier_cell_state : public cell_state {
 public:
  barrier_cell_state(cell & c)
  : cell_state(c)
  { }

  virtual bool is_obstacle() const noexcept {
    return true;
  }
};

class spawn_cell_state : public cell_state {
 public:
  spawn_cell_state(cell & c)
  : cell_state(c)
  { }

  virtual bool is_obstacle() const noexcept {
    return true;
  }
};

class grass_cell_state : public cell_state {
 public:
  grass_cell_state(cell & c)
  : cell_state(c)
  { }
};

class water_cell_state : public resource_cell_state {
 public:
  water_cell_state(cell & c, int amount = 1)
  : resource_cell_state(c, amount)
  { }
};

class food_cell_state : public resource_cell_state {
 public:
  static constexpr int max_amount() { return 10; }

  food_cell_state(cell & c, int amount)
  : resource_cell_state(c, std::max(max_amount(), amount))
  { }

  food_cell_state(cell & c)
  : resource_cell_state(c, max_amount())
  { }

  virtual void on_enter(gos::state::ant &, const gos::state::game_state &);
};

/**
 * Single cell in the grid.
 *
 */
class cell {
  cell_type                     _type = cell_type::plain;
  std::shared_ptr<cell_state>   _state;

  friend cell_state;

 public:
  cell(
    cell_type                     ct,
    std::shared_ptr<cell_state>   cs)
  : _type(ct)
  , _state(cs)
  { }

  cell(
    cell_type                     ct = cell_type::plain)
  : _type(ct)
  {
    switch (ct) {
      case cell_type::grass:
        _state = std::make_shared<grass_cell_state>(*this);
        break;
      case cell_type::spawn_point:
        _state = std::make_shared<spawn_cell_state>(*this);
        break;
      case cell_type::material:
        _state = std::make_shared<plain_cell_state>(*this);
        break;
      case cell_type::water:
        _state = std::make_shared<water_cell_state>(*this);
        break;
      case cell_type::food:
        _state = std::make_shared<food_cell_state>(*this);
        break;
      case cell_type::barrier:
        _state = std::make_shared<barrier_cell_state>(*this);
        break;
      case cell_type::plain:
      default: // fall-through
        _state = std::make_shared<plain_cell_state>(*this);
        break;
    }
  }

  bool               is_taken()    const { return _state->is_taken();    }
  bool               is_obstacle() const { return _state->is_obstacle(); }

  gos::state::ant  * ant()            { return _state->ant(); }

  cell_type          type()     const { return _type;        }
  cell_state       * state()          { return _state.get(); }
  const cell_state * state()    const { return _state.get(); }

  void enter(gos::state::ant & a, const gos::state::game_state & gs) {
    _state->on_enter(a, gs);
  }
  void leave(gos::state::ant & a, const gos::state::game_state & gs) {
    _state->on_exit(a, gs);
  }
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__CELL_H__INCLUDED
