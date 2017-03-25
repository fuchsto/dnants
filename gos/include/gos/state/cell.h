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
  bool _taken    = false;
  bool _obstacle = false;

 public:
  constexpr cell(bool obstacle = false)
  : _obstacle(obstacle)
  { }

  virtual void on_enter(const ant &) { };
  virtual void on_exit(const ant &)  { };

  void enter(ant & a) { _taken = true;  this->on_enter(a); }
  void leave(ant & a) { _taken = false; this->on_exit(a); }

  virtual constexpr bool is_obstable() const noexcept {
    return _obstacle;
  }
  constexpr bool taken() const noexcept {
    return _taken;
  }
};


class plain_cell : public cell {
};

class grass_cell : public cell {
 public:
  grass_cell() : cell(true) { }
};

class resource_cell : public cell {
  int _amount = 1;
 public:
  constexpr resource_cell()        : _amount(1)   { }
  constexpr resource_cell(int cap) : _amount(cap) { }

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
  water_cell() : resource_cell(true) { }
};

class food_cell : public resource_cell {
};

class material_cell : public resource_cell {
};


} // namespace state
} // namespace gos

#endif // GOS__STATE__CELL_H__INCLUDED
