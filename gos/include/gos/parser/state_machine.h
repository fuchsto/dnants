#ifndef GOS__DNA__RULE_TYPES_H__INCLUDED
#define GOS__DNA__RULE_TYPES_H__INCLUDED

#define BOOST_SPIRIT_DEBUG

#include <string>

#include <boost/spirit/include/lex_lexertl.hpp>

#include <boost/spirit/include/support_utree.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <iostream>


namespace gos {
namespace dna {

struct state_command {
};

struct state_event_handler {
  std::vector<state_command>       _commands;
};

struct state_definition {
  std::string                      _state_id;
  std::vector<state_command>       _commands;
  std::vector<state_event_handler> _evt_handlers;

  state_definition(const std::string & state_id)
  : _state_id(state_id)
  { }

  const std::string & id() const {
    return _state_id;
  }
};

struct state_definitions {
  std::vector<state_definition> _states;

  state_definitions() { }

	void add_state_def(const std::string & state_id) {
    _states.push_back(
      state_definition(state_id));
	}

  bool has_state(const std::string & state_id) const {
    return std::find_if(_states.begin(), _states.end(),
                        [&](const state_definition & s) {
                            return s.id() == state_id; })
           != _states.end();
  }

  state_definition & operator[](const std::string & state_id) {
    return *std::find_if(_states.begin(), _states.end(),
                         [&](const state_definition & s) {
                             return s.id() == state_id; });
  }
};


namespace qi     = boost::spirit::qi;
namespace ascii  = boost::spirit::ascii;
namespace spirit = boost::spirit;

template <typename Iterator>
struct state_grammar
: boost::spirit::qi::grammar<
    Iterator,
    boost::spirit::qi::space_type,
    boost::spirit::utree()>
{
  state_grammar() : state_grammar::base_type(program)
  {
    using namespace qi::labels;
    using qi::eol;
    using qi::eoi;
    using qi::alpha;
    using qi::graph;
    using qi::lit;
    using qi::lexeme;
    using qi::int_;
    using ascii::char_;

    identifier
      = +alpha;

    string_literal
      = lit('"') >> +~char_('"') >> lit('"');

    int_value
      = +int_;

    program = +(state_definition); 
                       
    state_definition
      = ( (lit("state") >> lit(":") >> identifier)
          [ std::cout << "state tag: " << qi::_1 << "\n" ]
        ) >> rules_block;

    state_identifier
      = lit("@") >> string_literal;

    attribute
      = lit("strength") | lit("carrying") | lit("dir") | lit("damage");

    action
      = ( lit("move") | lit("turn") | lit("set_dir") |
          lit("eat")  | lit("take") | lit("backtrace") ) >> lit("!")
        [ std::cout << "action: " << qi::_1 << "\n" ];

    event
      = lit("food")     | lit("collision") | lit("enemy") |
        lit("attacked") | lit("home");
            
    on_event_block
      = on_event_clause >> rules_block; 

    // on(event)
    on_event_clause
      = lit("on") >> lit("(") >> event >> lit(")");

    // { +expression }
    rules_block
      = lit("{") >> +(rule_expression) >> lit("}");

    // if (...) { } or command
    rule_expression
      = conditional_block | command_clause;

    // switch(@state_id)
    state_switch_clause
      = lit("switch") >> lit("(")
                      >> (state_identifier | lit("prev"))
                      >> lit(")");

    // on(event) or action! or switch(@state)
    command_clause
      = on_event_block | action_expression | state_switch_clause;

    // move! or turn!(3)
    action_expression
      // should be expression_term instead of value
      = action >> -(lit("(") >> int_value >> lit(")"));

    conditional_block
      = if_clause | if_else_clause | every_clause;

		every_clause
			= lit("every") >> lit("(") >> int_value >> lit(")")
                     >> rules_block;

    // identifier <= 123
    condition_clause
      // should be expression_term instead of value
      = identifier >> compare_op >> int_value;

    compare_op
      = (lit("<=") | lit("<") | lit(">=") | lit(">") |
         lit("==") | lit("!="));

    // if (...) { ... }
    if_clause
      = lit("if") >> lit("(") >> condition_clause >> lit(")")
                  >> rules_block;

    // else { ... }
    else_clause
      = lit("else") >> rules_block;

    // if (...) { ... } else { ... }
    if_else_clause
      = if_clause >> else_clause;

		BOOST_SPIRIT_DEBUG_NODE(program);
		BOOST_SPIRIT_DEBUG_NODE(state_definition);
		BOOST_SPIRIT_DEBUG_NODE(state_identifier);
		BOOST_SPIRIT_DEBUG_NODE(identifier);
		BOOST_SPIRIT_DEBUG_NODE(action);
		BOOST_SPIRIT_DEBUG_NODE(rule_expression);
		BOOST_SPIRIT_DEBUG_NODE(rules_block);
		BOOST_SPIRIT_DEBUG_NODE(command_clause);
  } // state_grammar()
  
  boost::spirit::qi::rule<
    Iterator,
    qi::space_type,
    spirit::utree()>
      program,
      state_definition, state_identifier,
      attribute, action, event, on_event_block,
      on_event_clause, rules_block, rule_expression,
			if_clause, else_clause, if_else_clause, every_clause,
      conditional_block, condition_clause, command_clause, compare_op,
      action_expression, 
      state_switch_clause,
      int_value, 
      number, op, identifier, string_literal;
};


#if 0
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
#endif

} // namespace dna
} // namespace gos

#endif // _DNA__RULE_TYPES_H__INCLUDED
