#ifndef GOS__STATE__CELL_H__INCLUDED
#define GOS__STATE__CELL_H__INCLUDED

#include <gos/state/ant.h>

#include <memory>


namespace gos {
namespace state {

enum cell_type : int {
  plain = 0,
  grass,
  water,
  material,
  food
};

class cell;

class cell_state {
  cell & _cell;
  bool   _taken = false;

 public:
  cell_state(cell & c)
  : _cell(c)
  { }

  virtual ~cell_state() { }

  virtual void on_enter(ant &);
  virtual void on_exit(ant &);

  bool is_obstacle() const noexcept {
    return is_taken();
  }

  bool is_taken() const noexcept {
    return _taken;
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

  virtual void on_enter(ant &);
  virtual void on_exit(ant &);

  int amount_left() const noexcept {
    return _amount;
  }
  bool is_depleted() const noexcept {
    return amount_left() == 0;
  }

  int consume(int strength) noexcept {
    int consumed = std::min<int>(strength, _amount);
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
  food_cell_state(cell & c, int amount = 1)
  : resource_cell_state(c, amount)
  { }
};

/**
 * Single cell in the grid.
 *
 */
class cell {
  cell_type                   _type = cell_type::plain;
  std::shared_ptr<cell_state> _state;

 public:
  cell(cell_type ct, std::shared_ptr<cell_state> cs)
  : _type(ct)
  , _state(cs)
  { }

  cell(cell_type ct = cell_type::plain)
  : _type(ct)
  {
    switch (ct) {
      case cell_type::grass:
        _state = std::make_shared<grass_cell_state>(*this);
        break;
      case cell_type::water:
        _state = std::make_shared<water_cell_state>(*this);
        break;
      case cell_type::food:
        _state = std::make_shared<food_cell_state>(*this);
        break;
      case cell_type::plain:
      default: // fall-through
        _state = std::make_shared<plain_cell_state>(*this);
        break;
    }
  }

  bool               is_taken() const { return _state->is_taken(); }

  cell_type          type()     const { return _type;         }
  cell_state       & state()          { return *_state.get(); }
  const cell_state & state()    const { return *_state.get(); }

  void enter(ant & a) { _state->on_enter(a); }
  void leave(ant & a) { _state->on_exit(a);  }
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__CELL_H__INCLUDED
