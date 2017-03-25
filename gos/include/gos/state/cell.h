#ifndef GOS__STATE__CELL_H__INCLUDED
#define GOS__STATE__CELL_H__INCLUDED

#include <gos/state/ant.h>

namespace gos {
namespace state {

enum cell_type : int {
  plain = 0,
  grass,
  water,
  material,
  food
};

/**
 * State of a single cell in the grid.
 *
 */
class cell {
  bool      _taken     = false;
  cell_type _cell_type = cell_type::plain;

 public:
  constexpr cell(cell_type ct)
  : _cell_type(ct)
  { }

  cell_type type() const { return _cell_type; }

  virtual void on_enter(const ant &) { };
  virtual void on_exit(const ant &)  { };

  void enter(ant & a) { _taken = true;  this->on_enter(a); }
  void leave(ant & a) { _taken = false; this->on_exit(a); }

  virtual bool is_obstacle() const noexcept {
    return false;
  }
  constexpr bool taken() const noexcept {
    return _taken;
  }
};


class plain_cell : public cell {
 public:
  plain_cell() : cell(cell_type::plain) { }
};

class grass_cell : public cell {
 public:
  grass_cell() : cell(cell_type::grass) { }
};

class resource_cell : public cell {
  int _amount = 1;
 public:
  resource_cell() = delete;
  constexpr resource_cell(cell_type ct)          : cell(ct), _amount(1)   { }
  constexpr resource_cell(cell_type ct, int cap) : cell(ct), _amount(cap) { }

  virtual void on_enter(const ant & a) {
    if (_amount > 0) { --_amount; }
  }

  virtual void on_exit(const ant & a)  { }

  constexpr int amount_left() const noexcept {
    return _amount;
  }
  constexpr bool depleted() const noexcept {
    return amount_left() == 0;
  }

  int consume(int strength) noexcept {
    int consumed = std::min<int>(strength, _amount);
    _amount -= consumed;
    return consumed;
  }
};

class water_cell : public resource_cell {
 public:
  water_cell() : resource_cell(cell_type::water) { }
};

class food_cell : public resource_cell {
 public:
  food_cell() : resource_cell(cell_type::food) { }
};

class material_cell : public resource_cell {
 public:
  material_cell() : resource_cell(cell_type::material) { }
};


} // namespace state
} // namespace gos

#endif // GOS__STATE__CELL_H__INCLUDED
