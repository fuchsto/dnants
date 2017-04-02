
#include <gos/dna/rule_types.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


int main(int argc, const char * argv[])
{
  std::cout << "State definitions test parser\n";

  using boost::spirit::qi::space;
  using boost::spirit::qi::blank;
  using boost::spirit::utree;
  typedef std::string::const_iterator iterator_type;
  typedef gos::dna::state_grammar<iterator_type> state_grammar;

  state_grammar sg;

  if (argc < 2) {
    std::cerr << "no file given\n";
    return -1;
  }

  std::ifstream ifs(argv[1]);
  std::stringstream ss;
  ss << ifs.rdbuf();

  std::string input = ss.str();

  std::replace(input.begin(), input.end(), '\n', ' ');

  std::cout << "-------------------------\n"
            << "Parsing input: " << '\n'
            << input << '\n';

  std::string::const_iterator iter = input.begin();
  std::string::const_iterator end  = input.end();
  utree ut;
  bool r = phrase_parse(iter, end, sg, space, ut);

  if (r && iter == end) {
    std::cout << "-------------------------\n"
              << "Parsing succeeded: " << '\n'
              << ut << "\n"
              << "-------------------------\n";
  } else {
    std::string rest(iter, end);
    std::cout << "-------------------------\n"
              << "Parsing failed\n"
              << "stopped at: \": " << rest << "\"\n"
              << "-------------------------\n";
  }
  return 0;
}
