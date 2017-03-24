#ifndef GOS__RULE_ENGINE_H__INCLUDED
#define GOS__RULE_ENGINE_H__INCLUDED

namespace gos {

class rule_engine
{
 public:
  rule_engine();

  game_state & next_state(game_state & prev_state);
};

} // namespace

#endif // GOS__RULE_ENGINE_H__INCLUDED
