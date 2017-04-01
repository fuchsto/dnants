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

    // update positions:
    // -- team 0, even ant ids
    for (int tid = 0; tid < _teams.size(); tid += 2) {
      for (int aid = 0; aid < _teams[tid].ants().size(); aid += 2) {
        _teams[tid].ants()[aid].update_position();
      }
    }
    // -- team 1, all ants
    for (int tid = 1; tid < _teams.size(); tid += 2) {
      for (int aid = 0; aid < _teams[tid].ants().size(); ++aid) {
        _teams[tid].ants()[aid].update_position();
      }
    }
    // -- team 0, odd ant ids
    for (int tid = 0; tid < _teams.size(); tid += 2) {
      for (int aid = 1; aid < _teams[tid].ants().size(); aid += 2) {
        _teams[tid].ants()[aid].update_position();
      }
    }

    // update actions: 
    // -- team 0, even ant ids
    for (int tid = 0; tid < _teams.size(); tid += 2) {
      for (int aid = 0; aid < _teams[tid].ants().size(); aid += 2) {
        _teams[tid].ants()[aid].update_action();
      }
    }
    // -- team 1, all ants
    for (int tid = 1; tid < _teams.size(); tid += 2) {
      for (int aid = 0; aid < _teams[tid].ants().size(); ++aid) {
        _teams[tid].ants()[aid].update_action();
      }
    }
    // -- team 0, odd ant ids
    for (int tid = 0; tid < _teams.size(); tid += 2) {
      for (int aid = 1; aid < _teams[tid].ants().size(); aid += 2) {
        _teams[tid].ants()[aid].update_action();
      }
    }

    // update reactions: 
    // -- team 0, even ant ids
    for (int tid = 0; tid < _teams.size(); tid += 2) {
      for (int aid = 0; aid < _teams[tid].ants().size(); aid += 2) {
        _teams[tid].ants()[aid].update_reaction();
      }
    }
    // -- team 1, all ants
    for (int tid = 1; tid < _teams.size(); tid += 2) {
      for (int aid = 0; aid < _teams[tid].ants().size(); ++aid) {
        _teams[tid].ants()[aid].update_reaction();
      }
    }
    // -- team 0, odd ant ids
    for (int tid = 0; tid < _teams.size(); tid += 2) {
      for (int aid = 1; aid < _teams[tid].ants().size(); aid += 2) {
        _teams[tid].ants()[aid].update_reaction();
      }
    }
  }
};

} // namespace state
} // namespace gos

#endif // GOS__STATE__POPULATION_H__INCLUDED
