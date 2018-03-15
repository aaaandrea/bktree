#include <iostream>
#include <numeric>
#include "../src/main.cpp"

bool build_bktree()
{
    std::vector<std::string> dictionary = {"beep", "boop", "bing", "bong", "beet", "jingle", "jangle", "bee", "best", "peep", "belt"};

    // try finding an iterator that goes through a txt file

    auto tree = makeBKTree(dictionary.begin(), dictionary.end(), levenshtein_distance);
    tree.printDotToStdout();

    return true;
}

void load_tree()
{

}

void lookup()
{
    std::vector<std::string> dictionary = {"beep", "boop", "bing", "bong", "beet", "jingle", "jangle", "bee", "best", "peep", "belt"};
    std::string value = "beep"

    // try finding an iterator that goes through a txt file
    auto tree = makeBKTree(dictionary.begin(), dictionary.end(), levenshtein_distance);
    tree.nearest("beep", 2, "log");

    return true;
}

int main()
{
    build_bktree();
    load_tree();
    lookup();
    return 0;
}
