## BK Tree
### Background
The BK Tree a graph tree structure, with nodes and edges, based on metric space. The space is calculated using Levenshtein distance from the root, which indicates both the distance of change from the root, as well as determining the edge where the result will be found. Levenshtein distance is calculated as the difference from the root word, to the target. For example `book => boot` has a Levenshtein distance of 1, because the only letter that is different is k => t. The same would also go for `book => boo`, whose whitespace means a Levenshtein distance of 1 as well.

Nodes in a BK Tree consist of a single root item and arbitrary subtrees with distance labels on its edges. In constructing a BK Tree you take an arbitrary item in the database and set that as the root node. Inserting happens one at a time by traversing the tree, and following edges labeled `distance(node, item)`. If there is no such edge inset a new child. Otherwise recurse down the tree.

### Technology
There were a few implementations I checked out before deciding:
- JavaScript npm package: https://www.npmjs.com/package/bktree
- C++ BK Tree:
    - https://github.com/TeamHG-Memex/bk-string - has errors
    - https://gist.github.com/daniel-j-h/8418cd89789c3fe611a8362161d86a6a
- “C++ performant” python-module: https://github.com/fake-name/IntraArchiveDeduplicator/tree/master/deduplicator
- Rust BK Tree: https://github.com/eugene-bulkin/rust-bk-tree

The [handiwork](https://gist.github.com/daniel-j-h/8418cd89789c3fe611a8362161d86a6a) for building a BK Tree is in C++. It was fairly straightforward, so I would not expect our implementation to be particularly challenging. C++ also promised to be a bit faster than the JS package, and I don't know Rust.

### Structure size
I took the approximate size of a node, and multiplied that by the number of nodes. For the US index the size was 12.8524 Megabytes. The European index was 9.68394 Megabytes.

### Sample
`query.cpp` builds a BK Tree and loads it with the records of a given file.

The query eval:
- Does an exact-match lookup with a Levenshtein Distance of 0.
- Attempts an exact-match lookup for words that are not in the structure.
- Does a fuzzy search for words with a Levenshtein Distance of 1.
- Does a lookup for words in the structure with a Levenshtein Distance of 2.
- measures the time it takes for each of these lookups.

### Performance
- Build time: 1.8 seconds
- Load time: ~4 seconds
- On-disk structure: There is no on-storage disk
- Structure size: ~12MB
- Query time:
```
$ ./query.o us-address
node size: 2.28882e-05 Megabytes
number of nodes: 561528
tree size: 12.8524 Megabytes
starting exact lookup...
exact lookup took 0.494023 seconds (2.0242e+06 lookups/sec, 0.000494023ms/lookup)
starting exact-failure lookup...
exact-failure lookup took 0.496443 seconds (2.01433e+06 lookups/sec, 0.000496443ms/lookup)
starting edit-distance-1 lookup...
edit-distance-1 lookup took 0.872617 seconds (1.14598e+06 lookups/sec, 0.000872617ms/lookup)
starting edit-distance-1+levenshtein-filter lookup...
edit-distance-1+levenshtein-filter lookup took 0.85977 seconds (1.1631e+06 lookups/sec, 0.00085977ms/lookup)
starting edit-distance-2 lookup...
edit-distance-2 lookup took 15.904 seconds (62877.2 lookups/sec, 0.015904ms/lookup)
starting edit-distance-2+levenshtein-filter lookup...
edit-distance-2+levenshtein-filter lookup took 14.083 seconds (71007.4 lookups/sec, 0.014083ms/lookup)

$ ./query.o europen-place
node size: 2.28882e-05 Megabytes
number of nodes: 423098
tree size: 9.68394 Megabytes
starting exact lookup...
exact lookup took 0.583696 seconds (1.71322e+06 lookups/sec, 0.000583696ms/lookup)
starting exact-failure lookup...
exact-failure lookup took 0.592624 seconds (1.68741e+06 lookups/sec, 0.000592624ms/lookup)
starting edit-distance-1 lookup...
edit-distance-1 lookup took 1.01624 seconds (984018 lookups/sec, 0.00101624ms/lookup)
starting edit-distance-1+levenshtein-filter lookup...
edit-distance-1+levenshtein-filter lookup took 1.04148 seconds (960173 lookups/sec, 0.00104148ms/lookup)
starting edit-distance-2 lookup...
edit-distance-2 lookup took 5.53284 seconds (180739 lookups/sec, 0.00553284ms/lookup)
starting edit-distance-2+levenshtein-filter lookup...
edit-distance-2+levenshtein-filter lookup took 5.34246 seconds (187180 lookups/sec, 0.00534246ms/lookup)
```

### Features
- - Stores non-Latin text?
    - It does **not** store non-Latin text
- Can query for mispellings at arbitrary edit distances.
    - Yes. It builds the tree, and later is able to query up to x edit distance.
- Can you get multiple results at a time?
    - how do you determine which ones come back? is it “give me the X closest matches” or “give me all matches within edit distance Y” or something else?
- can you get back variations even if the exact match is in the structure?
    - Not currently
- is it possible to map particular entries in the structure to an ID we could use to do lookups in some other data structure?
    - Not currently
- can you do prefix searches with it? (e.g., “give me all the things that start with X”)
    - Not currently
- can you do queries that are simultaneously fuzzy search and prefix search (e.g., “give me all the things that start with something within edit distance Y of X”)
    - Not currently
- what metric does it use for measuring distance between strings? (Levenshtein, Damerau-Levenshtein, something else?) If something other than Levenshtein, does it look like it would still be suitable for our purposes?
    - Levenshtein

### Ease
There is ample documentation for using BK Trees. I didn't know anything about BK Trees, and very little C++, prior to this, and it works.

### Thoughts
BK Trees [cannot handle floating point values as metric](https://gist.github.com/eiiches/2016232#gistcomment-223344), though I do use a `double` in setting Levenshtein Distance.
