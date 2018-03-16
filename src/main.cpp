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
    std::string db_file("db/" + dictionary + ".db");
    std::string dictionary_file(dictionary + "-words.txt");

    // Create a BK db
    auto tree = makeBKTree(dictionary.begin(), dictionary.end(), levenshtein_distance);

    // try finding an iterator that goes through a txt file

    std::string line;
    std::ifstream myfile(dictionary_file);
    if (myfile.is_open()) {
        while (getline(myfile,line)) {
            // dbw.insert(line);
            break;
        }
        myfile.close();
    } else {
        std::cout << "Unable to open input file" << std::endl;
        return 1;
    }
    // dbw.close();

    tree.printDotToStdout();

    return 0;
}
