#ifndef GOS__STATE__POPULATION_H__INCLUDED
#define GOS__STATE__POPULATION_H__INCLUDED

#include <gos/state/ant.h>

#include <vector>


namespace gos {
namespace state {

class population {
  std::vector<ant_team> _teams;
 public:
  population(const std::vector<ant_team> & teams)
  : _teams(teams)
  { }

  const std::vector<ant_team> & teams() const noexcept {
    return _teams;
  }

  std::vector<ant_team> & teams() noexcept {
    return _teams;
  }
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__POPULATION_H__INCLUDED
