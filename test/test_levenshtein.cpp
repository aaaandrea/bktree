#include <iostream>

#include "../src/levenshtein.cpp"

int find_levenshtein()
{
    // calculate lev btwwn 2 strings
    std::string string1 = "beep";
    std::string string2 = "boop";

    int result = levenshtein_distance(string1, string2);
    std::cout << result << "\n";

    return 0;
}

int main()
{
    find_levenshtein();
    return 0;
}
