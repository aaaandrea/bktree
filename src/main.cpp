#include "bktree.cpp"
#include "levenshtein.cpp"

#include <iostream>
#include <vector>

// need to provide these in order to allow printDotToStdout to print string instead of int
template <> struct StdoutRep<std::string>
{
  StdoutRep(std::string v) : value(v) {}

  friend std::ostream& operator<<(std::ostream& out, StdoutRep rep) {
    return out << rep.value;
  }

  const std::string value;
};

template <> struct StdoutRep<int>
{
  StdoutRep(int v) : value(v) {}

  friend std::ostream& operator<<(std::ostream& out, StdoutRep rep) {
    return out << rep.value;
  }

  const int value;
};

int main()
{
    // calculate lev btwwn 2 strings
    std::string string1 = "beep";
    std::string string2 = "boop";

    int result = levenshtein_distance(string1, string2);
    std::cout << result << "\n";

    std::vector<std::string> dictionary = {"beep", "boop", "glop", "gravy", "beet"};

    // try finding an iterator that goes through a txt file

    auto tree = makeBKTree(dictionary.begin(), dictionary.end(), levenshtein_distance);
    tree.printDotToStdout();

    return 0;
}
