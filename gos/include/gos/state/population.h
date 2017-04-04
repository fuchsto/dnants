#ifndef GOS__STATE__POPULATION_H__INCLUDED
#define GOS__STATE__POPULATION_H__INCLUDED

#include <gos/state/ant.h>

#include <vector>


namespace gos {
namespace state {

class population {
  std::vector<ant_team> _teams;
 public:

  population(std::vector<ant_team> && teams)
  : _teams(std::move(teams))
  { }

  const std::vector<ant_team> & teams() const noexcept {
    return _teams;
  }

  std::vector<ant_team> & teams() noexcept {
    return _teams;
  }

  void update() {
    for (auto & t : _teams) {
      t.update();
    }

    for (auto & t : _teams) {
      for (auto & a : t.ants()) {
        a.update_position();
      }
    }
    for (auto & t : _teams) {
      for (auto & a : t.ants()) {
        a.update_action();
      }
    }
    for (auto & t : _teams) {
      for (auto & a : t.ants()) {
        a.update_reaction();
      }
    }
  }
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__POPULATION_H__INCLUDED
