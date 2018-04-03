// the query.cpp script loads a structure and queries it both for exact and inexact matches, and times each.
// In particular:
//  it does an exact-match lookup (~equivalent to "edit distance 0")
//  it tries an exact-match lookup of words not in the structure
//  it does a fuzzy lookup of words of edit distance one from words in the dictionary, using a Levenshtein distance
#include "levenshtein.cpp"
#include "bktree.cpp"

#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "usage: build <word-list>" << std::endl;
        return 1;
    }
    std::string word_list(argv[1]);
    std::string line;

    // read the edit-distance-1 typos into a vector of (correct, incorrect) pairs
    std::string typo1_file(word_list + "-typos-d1.txt");
    std::vector<std::pair<std::string, std::string> > typo1;
    std::ifstream t1f(typo1_file);
    while (getline(t1f, line)) {
        size_t comma = line.find(",");
        typo1.push_back(std::make_pair<std::string, std::string>(line.substr(0, comma), line.substr(comma + 1, line.length())));
    }
    t1f.close();

    // read the edit-distance-2 typos into a vector of (correct, incorrect) pairs
    std::string typo2_file(word_list + "-typos-d2.txt");
    std::vector<std::pair<std::string, std::string> > typo2;
    std::ifstream t2f(typo2_file);
    while (getline(t2f, line)) {
        size_t comma = line.find(",");
        typo2.push_back(std::make_pair<std::string, std::string>(line.substr(0, comma), line.substr(comma + 1, line.length())));
    }
    t2f.close();

    // Open the database for reading. ******************
    if (argc < 2) {
        std::cout << "usage: build <word-list>" << std::endl;
        return 1;
    }

    std::string dictionary(argv[1]);
    std::string dictionary_file(dictionary + "-words.txt");

    std::vector<std::string> matches, filtered_matches;
    std::chrono::high_resolution_clock::time_point start, end;
    std::chrono::duration<double> seconds;
    // Create a BK db
    // try finding an iterator that goes through a txt file
    std::cout << "starting BK Tree build..." << std::endl;
    start  = std::chrono::high_resolution_clock::now();

    auto tree = makeBKTree<std::string>(levenshtein_distance);

    end  = std::chrono::high_resolution_clock::now();
    seconds = end - start;
    std::cout << "BK Tree build time was " <<
        seconds.count() << " seconds (" <<
        (1000000./seconds.count()) << " lookups/sec, " <<
        (seconds.count() / 1000) << "ms/lookup)" << std::endl;

    std::cout << "starting BK Tree load..." << std::endl;
    start  = std::chrono::high_resolution_clock::now();

    double number_of_nodes = 0;
    double node_size = 0;
    std::string newline;
    std::ifstream myfile(dictionary_file);
    std::string non_latin_char;

    if (myfile.is_open()) {
        while (getline(myfile,newline)) {
            if (newline == "京") {
                non_latin_char = sizeof(newline);
            }
            tree.insert(newline);
            node_size = sizeof(newline);
            number_of_nodes += 1;
        }
        tree.sort();
        myfile.close();
    } else {
        std::cout << "Unable to open input file" << std::endl;
        return 1;
    }

    end  = std::chrono::high_resolution_clock::now();
    seconds = end - start;
    std::cout << "BK Tree load time was " <<
        seconds.count() << " seconds (" <<
        (1000000./seconds.count()) << " lookups/sec, " <<
        (seconds.count() / 1000) << "ms/lookup)" << std::endl;

    // *************************************************************************

    // log structure size
    // std::cout << "京 size: " << non_latin_char << "Megabytes \n";
    node_size /= 1048576;
    std::cout << "node size: " << node_size << " Megabytes \n";
    std::cout << "number of nodes: " << number_of_nodes << "\n";

    double tree_size = node_size * number_of_nodes;
    std::cout << "tree size: " << tree_size << " Megabytes \n";

    // *************************************************************************

    // test the correctly spelled words from one of the typo files (doesn't matter which)
    // with a 0 edit threshold

    std::cout << "starting exact lookup..." << std::endl;
    start  = std::chrono::high_resolution_clock::now();

    for (auto it = typo1.begin(); it != typo1.end(); ++it) {
        matches.clear();
        tree.nearest(std::get<0>(*it), 0, std::back_inserter(matches));
    }

    end  = std::chrono::high_resolution_clock::now();
    seconds = end - start;
    std::cout << "exact lookup took " <<
        seconds.count() << " seconds (" <<
        (1000000./seconds.count()) << " lookups/sec, " <<
        (seconds.count() / 1000) << "ms/lookup)" << std::endl;

    // *************************************************************************

    // test incorrectly spelled words from one of the typo files (doesn't matter which)
    // with a 0 edit threshold -- these will return no results, so this is a test of the
    // speed of failures

    std::cout << "starting exact-failure lookup..." << std::endl;
    start  = std::chrono::high_resolution_clock::now();

    for (auto it = typo1.begin(); it != typo1.end(); ++it) {
        matches.clear();
        tree.nearest(std::get<1>(*it), 0, std::back_inserter(matches));
    }

    end  = std::chrono::high_resolution_clock::now();
    seconds = end - start;
    std::cout << "exact-failure lookup took " <<
        seconds.count() << " seconds (" <<
        (1000000./seconds.count()) << " lookups/sec, " <<
        (seconds.count() / 1000) << "ms/lookup)" << std::endl;

    // *************************************************************************

    // test the edit-distance-1 typos with a permissive threshold

    std::cout << "starting edit-distance-1 lookup..." << std::endl;
    start  = std::chrono::high_resolution_clock::now();

    for (auto it = typo1.begin(); it != typo1.end(); ++it) {
        matches.clear();
        tree.nearest(std::get<1>(*it), 1, std::back_inserter(matches));
    }

    end  = std::chrono::high_resolution_clock::now();
    seconds = end - start;
    std::cout << "edit-distance-1 lookup took " <<
        seconds.count() << " seconds (" <<
        (1000000./seconds.count()) << " lookups/sec, " <<
        (seconds.count() / 1000) << "ms/lookup)" << std::endl;

    // *************************************************************************

    // test the edit-distance-1 plus levenshtein filter typos with a permissive threshold

    std::cout << "starting edit-distance-1+levenshtein-filter lookup..." << std::endl;
    start  = std::chrono::high_resolution_clock::now();

    for (auto it = typo1.begin(); it != typo1.end(); ++it) {
        matches.clear();
        filtered_matches.clear();
        tree.nearest(std::get<1>(*it), 1, std::back_inserter(matches));
        for (auto const& m: matches) {
            if (levenshtein_distance(m, std::get<1>(*it)) <= 1) {
                filtered_matches.push_back(m);
            }
        }
    }

    end  = std::chrono::high_resolution_clock::now();
    seconds = end - start;
    std::cout << "edit-distance-1+levenshtein-filter lookup took " <<
        seconds.count() << " seconds (" <<
        (1000000./seconds.count()) << " lookups/sec, " <<
        (seconds.count() / 1000) << "ms/lookup)" << std::endl;

    // *************************************************************************

    // test the edit-distance-2 typos with a permissive threshold

    std::cout << "starting edit-distance-2 lookup..." << std::endl;
    start  = std::chrono::high_resolution_clock::now();

    for (auto it = typo2.begin(); it != typo2.end(); ++it) {
        matches.clear();
        tree.nearest(std::get<1>(*it), 2, std::back_inserter(matches));
    }

    end  = std::chrono::high_resolution_clock::now();
    seconds = end - start;
    std::cout << "edit-distance-2 lookup took " <<
        seconds.count() << " seconds (" <<
        (1000000./seconds.count()) << " lookups/sec, " <<
        (seconds.count() / 1000) << "ms/lookup)" << std::endl;

    // *************************************************************************

    // test the edit-distance-2 plus levenshtein filter typos with a permissive threshold

    std::cout << "starting edit-distance-2+levenshtein-filter lookup..." << std::endl;
    start  = std::chrono::high_resolution_clock::now();

    for (auto it = typo1.begin(); it != typo1.end(); ++it) {
        matches.clear();
        filtered_matches.clear();
        tree.nearest(std::get<1>(*it), 2, std::back_inserter(matches));
        for (auto const& m: matches) {
            if (levenshtein_distance(m, std::get<1>(*it)) <= 2) {
                filtered_matches.push_back(m);
            }
        }
    }

    end  = std::chrono::high_resolution_clock::now();
    seconds = end - start;
    std::cout << "edit-distance-2+levenshtein-filter lookup took " <<
        seconds.count() << " seconds (" <<
        (1000000./seconds.count()) << " lookups/sec, " <<
        (seconds.count() / 1000) << "ms/lookup)" << std::endl;

    return 0;
}
