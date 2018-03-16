#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "bktree.cpp"
#include "levenshtein.cpp"


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

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "usage: build <word-list>" << std::endl;
        return 1;
    }

    std::string dictionary(argv[1]);
    std::string dictionary_file(dictionary + "-words.txt");

    // Create a BK db
    // try finding an iterator that goes through a txt file

    auto tree = makeBKTree<std::string>(levenshtein_distance);

    std::string line;
    std::ifstream myfile(dictionary_file);
    if (myfile.is_open()) {
        while (getline(myfile,line)) {
            tree.insert(line);
        }
        tree.sort();
        myfile.close();
    } else {
        std::cout << "Unable to open input file" << std::endl;
        return 1;
    }

    std::vector<std::string> matches;

    tree.nearest("beep", 2, std::back_inserter(matches));

    std::cout << matches.size() << std::endl;

    for (auto item:matches) {
        std::cout << item << std::endl;
    }

    tree.printDotToStdout();

    return 0;
}
