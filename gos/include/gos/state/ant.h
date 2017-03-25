#ifndef GOS__STATE__ANT_H__INCLUDED
#define GOS__STATE__ANT_H__INCLUDED

#include <gos/types.h>


namespace gos {
namespace state {

class ant {
 public:
  enum mode : int {
    waiting  = 0, // do nothing
    scouting,     // move without following a pheromone trace
    tracing,      // follow the closes pheromone trace
    eating        // eat and gain strength
  };
  // Ants can detect pheromone traces and distinguish friendly (own team)
  // from enemy pheromones.
 private:
  // When an ant is in a cell next to ants of another team, it dies if its
  // strength is less than the cumulative strength of the enemy ants in
  // adjacent cells (von-Neumann neighborhood, diagonals are considered
  // neighbor cells).
  int       _strength       = 10;
  // If an ant carries food or material, its strength available for fights
  // is reduced by the carried weight.
  int       _carry_weight   = 0;
  int       _nticks_not_fed = 0;
  position  _pos;
  direction _dir;
  mode      _mode           = ant::mode::waiting;
 public:
  ant(const position & pos) : _pos(pos) { }
  ant(position && pos)      : _pos(std::move(pos)) { }

  const position & pos() const noexcept {
    return _pos;
  }

  void move_to(const position & pos) noexcept {
    _pos = pos;
  }

  void move_to(position && pos) noexcept {
    _pos = std::move(pos);
  }

};

} // namespace state
} // namespace gos

#endif // GOS__STATE__ANT_H__INCLUDED
