#ifndef GOS__RULE_ENGINE_H__INCLUDED
#define GOS__RULE_ENGINE_H__INCLUDED

#include <gos/state/game_state.h>


namespace gos {

class rule_engine
{
 public:
  rule_engine();

  state::game_state & next_state(state::game_state & prev_state);
};

} // namespace

#endif // GOS__RULE_ENGINE_H__INCLUDED
