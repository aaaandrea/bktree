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
    std::vector<std::string> dictionary = {"beep", "boop", "bing", "bong", "beet", "jingle", "jangle", "bee", "best", "peep", "belt"};

    // try finding an iterator that goes through a txt file

    auto tree = makeBKTree(dictionary.begin(), dictionary.end(), levenshtein_distance);
    tree.printDotToStdout();

    return 0;
}
