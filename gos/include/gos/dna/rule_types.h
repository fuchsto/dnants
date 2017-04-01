#ifndef GOS__DNA__RULE_TYPES_H__INCLUDED
#define GOS__DNA__RULE_TYPES_H__INCLUDED

#include <string>

#include <boost/spirit/include/lex_lexertl.hpp>

#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_core.hpp>

#include <boost/bind.hpp>
#include <boost/ref.hpp>


namespace gos {
namespace dna {

namespace lex = boost::spirit::lex;

template <typename Lexer>
struct state_tokens : lex::lexer<Lexer>
{
  size_t c = 0;
  size_t w = 0;
  size_t l = 0;

  boost::spirit::lex::token_def<> word, eol, any;

  state_tokens()
  : word("[^ \t\n]+")
  , eol("\n")
  , any(".")
  {
    using boost::spirit::lex::_start;
    using boost::spirit::lex::_end;
    using boost::phoenix::ref;

    // associate tokens with the lexer
    this->self
        =   word  [++ref(w), ref(c) += distance(_start, _end)]
        |   eol   [++ref(c), ++ref(l)]
        |   any   [++ref(c)]
        ;
  }
};


#if 0
enum class settable_tag : int {
  direction = 0,
  mode
};

class setter {

};

class state_rule {
  std::string _state_tag;

 public:
  state_rule(const std::string & state_tag)
  : _state_tag(g)
  { }

  void add_setter(settable_tag);
};
#endif

} // namespace dna
} // namespace gos

#endif // _DNA__RULE_TYPES_H__INCLUDED
