#ifndef GOS__VIEW__UTIL_H__INCLUDED
#define GOS__VIEW__UTIL_H__INCLUDED

#include <SDL.h>


namespace gos {
namespace view {

static int next_time;

static Uint32 timeLeft(void) {
  Uint32 now = SDL_GetTicks();
  return ( next_time <= now
           ? 0
           : next_time - now );
}

static void 
cap_framerate(int tick_interval = 20) {
  SDL_Delay(timeLeft());
  next_time += tick_interval;
}

} // namespace view
} // namespace gos

#endif // GOS__VIEW__UTIL_H__INCLUDED
