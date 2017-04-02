#ifndef GOS__DNA__RULE_TYPES_H__INCLUDED
#define GOS__DNA__RULE_TYPES_H__INCLUDED

#include <string>

#include <boost/spirit/include/lex_lexertl.hpp>

#include <boost/spirit/include/support_utree.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>

#include <boost/bind.hpp>
#include <boost/ref.hpp>


namespace gos {
namespace dna {

struct dna_context {
  std::vector<std::string> _state_tags;
};


namespace qi     = boost::spirit::qi;
namespace ascii  = boost::spirit::ascii;
namespace spirit = boost::spirit;

template <typename Iterator>
struct state_grammar
: boost::spirit::qi::grammar<
    Iterator,
    boost::spirit::ascii::space_type,
    boost::spirit::utree()>
{
  // state:
  constexpr static const char * STATE     = "state";
  constexpr static const char * SWITCH    = "switch";
  constexpr static const char * PREV      = "prev";
  // flow control:
  constexpr static const char * IF        = "if";
  constexpr static const char * ELSE      = "else";
  constexpr static const char * EVERY     = "every";
  constexpr static const char * NOT       = "not";
  // stack values:
  constexpr static const char * STORE     = "store";
  constexpr static const char * LOAD      = "load";
  constexpr static const char * COUNT     = "count";
  // functions:
  constexpr static const char * RANDOM    = "random";
  // events:
  constexpr static const char * ON        = "on";
  constexpr static const char * FOOD      = "food";
  constexpr static const char * ENEMY     = "enemy";
  constexpr static const char * COLLISION = "collision";
  constexpr static const char * HOME      = "home";
  constexpr static const char * ATTACKED  = "attacked";
  // attributes:
  constexpr static const char * STRENGTH  = "strength";
  constexpr static const char * CARRYING  = "num_carrying";
  constexpr static const char * DAMAGE    = "damage";
  constexpr static const char * DIRECTION = "direction";
  // actions:
  constexpr static const char * MOVE      = "move";
  constexpr static const char * SET_DIR   = "set_dir";
  constexpr static const char * TURN      = "turn";
  constexpr static const char * EAT       = "eat";
  constexpr static const char * TAKE      = "take";
  constexpr static const char * DROP      = "drop";
  constexpr static const char * BACKTRACE = "backtrace";

  constexpr static const char   AT         = '@';
  constexpr static const char   EXCL       = '!';
  constexpr static const char   STAR       = '*';
  constexpr static const char   COMMA      = ',';
  constexpr static const char   LPAREN     = '(';
  constexpr static const char   RPAREN     = ')'; 
  constexpr static const char   LBRACE     = '{';
  constexpr static const char   RBRACE     = '}'; 
  constexpr static const char   SEMICOLON  = ';';
  constexpr static const char   COLON      = ':';
  constexpr static const char * LAND       = "&&";
  constexpr static const char * LOR        = "||";
  constexpr static const char   DOT        = '.';        
  constexpr static const char   ASSIGN     = '=';
  constexpr static const char   LT         = '<';
  constexpr static const char   GT         = '>';
  constexpr static const char * LE         = "<=";
  constexpr static const char * GE         = ">=";
  constexpr static const char * EQUAL      = "==";
  constexpr static const char * NE         = "!=";    

  state_grammar() : state_grammar::base_type(program)
  {
    using namespace qi::labels;
    using qi::eol;
    using qi::graph;
    using qi::lit;
    using qi::lexeme;
    using qi::int_;
    using ascii::char_;

    identifier
      = +graph;

    string_literal
      = lit('"') >> +~char_('"') >> '"';


    program = +(state_definition); 
                       
    state_definition
      = STATE << state_identifier
              << rules_clause;

    state_identifier
      = AT << identifier;

    attribute
      = lit(STRENGTH) | lit(CARRYING) | lit(DIRECTION) | lit(DAMAGE);

    action
      = lit(MOVE) | lit(TURN) | lit(SET_DIR) |
        lit(EAT) | lit(TAKE) | lit(BACKTRACE);

    event
      = lit(FOOD) | lit(COLLISION) | lit(ENEMY) |
        lit(ATTACKED) | lit(HOME);
            
    on_event_rules_clause
      = on_event_clause >> rules_clause; 

    on_event_clause
      = lit(ON) >> event;

    rules_clause
      = lit(LBRACE) >> +(rule_expression) >> lit(RBRACE);

    rule_expression
      = +(conditional_block | command_clause);

    state_switch_clause
      = lit(SWITCH) << (state_identifier | lit(PREV));

    command_clause
      = on_event_rules_clause | action_clause | state_switch_clause;

    conditional_block
      = if_clause | if_else_clause;

    condition_clause
      = identifier >> compare_op >> value;

    compare_op
      = lit(LE) | lit(LT) | lit(GE) | lit(GT) | lit(EQUAL) | lit(NE);

    if_clause
      = lit(IF) >> lit(LPAREN) >> condition_clause >> lit(RPAREN)
                >> rules_clause;

    else_clause
      = lit(ELSE) >> rules_clause;

    if_else_clause
      = if_clause >> else_clause;

  } // state_grammar()
  
  boost::spirit::qi::rule<
    Iterator,
    ascii::space_type,
    spirit::utree()>
      program,
      state_definition, state_identifier,
      attribute, action, event, on_event_rules_clause,
      on_event_clause, rules_clause, rule_expression,
      conditional_block, if_clause, else_clause, if_else_clause,
      condition_clause, command_clause, compare_op,
      action_clause, state_switch_clause,
      value, 
   // value_list, string_list, number_list,
      number, op, identifier, string_literal;

#if 0
  preprocessor() : preprocessor::base_type(body)
  {
    using namespace qi::labels;
    using qi::eol;
    using qi::graph;
    using qi::lit;
    using qi::lexeme;
    using qi::int_;
    using ascii::char_;

    vartype = +graph;
    varname = +(graph - char_("[;"));
    vardim  = '[' >> int_  >> "]";

    state_tag =
        "state" >> lit(':') >> // +~char_(')') >> ')' >>
        eol;

    field =
        vartype >>
        varname >>
        -vardim >>
        ';' >> 
        eol;

    body =
        strucname  >>
        '(' >> eol >>
        *field >>
        ')' >> -eol;

    BOOST_SPIRIT_DEBUG_NODES((body)(field)(statetag))
  }

  qi::rule<Iterator, struct_body(),  Skipper> body;
  qi::rule<Iterator, struct_field(), Skipper> field;
  qi::rule<Iterator, std::string(),  Skipper> strucname;
  qi::rule<Iterator, int(),          Skipper> vardim;
  // lexemes
  qi::rule<Iterator, std::string()> vartype, varname;
#endif
};

#if 0
template<typename Iterator, typename Skipper>
bool parse(
  Iterator      & first,
  Iterator        end,
  Skipper const & skipper,
  struct_body   & mystruct)
{
  preprocessor<Iterator, Skipper> g;
  return qi::phrase_parse(first, end, g, skipper, mystruct);
}
#endif


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
