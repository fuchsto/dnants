
#include <gos/dna/rule_types.h>


int main()
{
  std::cout << "state configuration test parser\n";

  using boost::spirit::ascii::space;
  using boost::spirit::utree;
  typedef std::string::const_iterator iterator_type;
  typedef gos::dna::state_grammar<iterator_type> state_grammar;

  state_grammar sg;

  std::string str;
  while (std::getline(std::cin, str)) {
    if (str.empty()) { break; }

    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end  = str.end();
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
  }
  return 0;
}
