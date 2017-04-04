#ifndef GOS__VIEW__GRID_VIEW_H__INCLUDED
#define GOS__VIEW__GRID_VIEW_H__INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <stdexcept>

#include <gos/view/utils.h>


namespace gos {
namespace view {

class grid_view {

  const gos::state::grid       & _grid;
  const gos::state::population & _population;

 public:
  grid_view(
    const gos::state::grid       & grid,
    const gos::state::population & population)
  : _grid(grid)
  , _population(population)
  {
  }

};

} // namespace gos
} // namespace view

#endif // GOS__VIEW__GRID_VIEW_H__INCLUDED
