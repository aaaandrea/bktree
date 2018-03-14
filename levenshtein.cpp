// from https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C++

#include <iostream>

unsigned int levenshtein_distance(const std::string& s1, const std::string& s2)
{
	const std::size_t len1 = s1.size(), len2 = s2.size();
	std::vector<unsigned int> col(len2+1), prevCol(len2+1);

	for (unsigned int i = 0; i < prevCol.size(); i++)
		prevCol[i] = i;
	for (unsigned int i = 0; i < len1; i++) {
		col[0] = i+1;
		for (unsigned int j = 0; j < len2; j++)
                        // note that std::min({arg1, arg2, arg3}) works only in C++11,
                        // for C++98 use std::min(std::min(arg1, arg2), arg3)
			col[j+1] = std::min({ prevCol[1 + j] + 1, col[j] + 1, prevCol[j] + (s1[i]==s2[j] ? 0 : 1) });
		col.swap(prevCol);
	}
	return prevCol[len2];
}


// or

// int levenshtein_distance(const std::string &s1, const std::string &s2)
// {
// 	// To change the type this function manipulates and returns, change
// 	// the return type and the types of the two variables below.
// 	int s1len = s1.size();
// 	int s2len = s2.size();
//
// 	auto column_start = (decltype(s1len))1;
//
// 	auto column = new decltype(s1len)[s1len + 1];
// 	std::iota(column + column_start - 1, column + s1len + 1, column_start - 1);
//
// 	for (auto x = column_start; x <= s2len; x++) {
// 		column[0] = x;
// 		auto last_diagonal = x - column_start;
// 		for (auto y = column_start; y <= s1len; y++) {
// 			auto old_diagonal = column[y];
// 			auto possibilities = {
// 				column[y] + 1,
// 				column[y - 1] + 1,
// 				last_diagonal + (s1[y - 1] == s2[x - 1]? 0 : 1)
// 			};
// 			column[y] = std::min(possibilities);
// 			last_diagonal = old_diagonal;
// 		}
// 	}
// 	auto result = column[s1len];
// 	delete[] column;
// 	return result;
// }
