## BK
### Background
It is a graph tree structure, with nodes and edges, based on metric space. The space is calculated using Levenshtein distance from the root, which indicates both the distance of change from the root, as well as determining the edge where the result will be found. Levenshtein distance is calculated as the difference from the root word, to the target. For example book => boot has a Levenshtein distance of 1, because the only letter that is different is k => t. The same would also go for book => boo, whose whitespace means a Levenshtein distance of 1 as well.

### Technology
The [handiwork](https://gist.github.com/daniel-j-h/8418cd89789c3fe611a8362161d86a6a) is in C++. It was fairly straightforward, so I would not expect our implemetnation to be particularly challenging.

### Structure size
???

### Sample
`query.cpp` builds a BK Tree and loads it with the records of a given file.

The query eval:
- Does an exact-match lookup with a Levenshtein Distance of 0.
- Attempts an exact-match lookup for words that are not in the structure.
- Does a fuzzy search for words with a Levenshtein Distance of 1.
- Does a lookup for words in the structure with a Levenshtein Distance of 2.
- measures the time it takes for each of these lookups.

### Performance
- Build time:
- Load time:
- On-disk structure: No.
- Structure size: ~12MB
- Memory usage:
- Query time:
```
$ ./query.o us-address
tree size: 12.8524 Megabytes
starting exact lookup...
exact lookup took 0.611254 seconds (1.63598e+06 lookups/sec, 0.000611254ms/lookup)
starting exact-failure lookup...
exact-failure lookup took 0.639069 seconds (1.56478e+06 lookups/sec, 0.000639069ms/lookup)
starting edit-distance-1 lookup...
edit-distance-1 lookup took 1.10268 seconds (906885 lookups/sec, 0.00110268ms/lookup)
starting edit-distance-1+levenshtein-filter lookup...
edit-distance-1+levenshtein-filter lookup took 1.07771 seconds (927894 lookups/sec, 0.00107771ms/lookup)
starting edit-distance-2 lookup...
edit-distance-2 lookup took 24.2244 seconds (41280.7 lookups/sec, 0.0242244ms/lookup)
starting edit-distance-2+levenshtein-filter lookup...
edit-distance-2+levenshtein-filter lookup took 20.1692 seconds (49580.6 lookups/sec, 0.0201692ms/lookup)

$ ./query.o europen-place
tree size: 9.68394 Megabytes
starting exact lookup...
exact lookup took 0.744637 seconds (1.34294e+06 lookups/sec, 0.000744637ms/lookup)
starting exact-failure lookup...
exact-failure lookup took 0.730214 seconds (1.36946e+06 lookups/sec, 0.000730214ms/lookup)
starting edit-distance-1 lookup...
edit-distance-1 lookup took 1.38088 seconds (724177 lookups/sec, 0.00138088ms/lookup)
starting edit-distance-1+levenshtein-filter lookup...
edit-distance-1+levenshtein-filter lookup took 1.40078 seconds (713889 lookups/sec, 0.00140078ms/lookup)
starting edit-distance-2 lookup...
edit-distance-2 lookup took 7.68356 seconds (130148 lookups/sec, 0.00768356ms/lookup)
starting edit-distance-2+levenshtein-filter lookup...
edit-distance-2+levenshtein-filter lookup took 8.49118 seconds (117769 lookups/sec, 0.00849118ms/lookup)
```

### Features
- - Stores non-Latin text?
    - - Internal representation is byte-based || character-based
        eg. is the edit distance between “北” and “京”, each of which is one character represented by three bytes, 1 or 3?
- Can query for mispellings at arbitrary edit distances.
- - Can you get multiple results at a time?
    - how do you determine which ones come back? is it “give me the X closest matches” or “give me all matches within edit distance Y” or something else?
- can you get back variations even if the exact match is in the structure?
    -
- is it possible to map particular entries in the structure to an ID we could use to do lookups in some other data structure?
    -
- can you do prefix searches with it? (e.g., “give me all the things that start with X”)
    -
- can you do queries that are simultaneously fuzzy search and prefix search (e.g., “give me all the things that start with something within edit distance Y of X”)
    -
- what metric does it use for measuring distance between strings? (Levenshtein, Damerau-Levenshtein, something else?) If something other than Levenshtein, does it look like it would still be suitable for our purposes?
    -

### Ease
There is ample documentation for using BK Trees. I didn't know anything about BK Trees, and very little C++, prior to this, and it works.

### Thoughts
BK Trees [cannot handle floating point values as metric](https://gist.github.com/eiiches/2016232#gistcomment-223344), though I do use a `double` in setting Levenshtein Distance.
