## BK Tree

BK implementation taken from [here](https://daniel-j-h.github.io/post/nearest-neighbors-in-metric-spaces/).
Using a basic Levenshtein Distance calculation.

### TODO
- [ ] test
    - [ ] test tree build
    - [ ] test import/export
    - [ ] test queries
        - [ ] test load all unique words from current us-address index
        - [ ] test lookup exact word match
        - [ ] test lookup mispelled word
- [ ] save to disk && loading it again
    - [ ] if built in:
        - [ ] loads unique words into structure and saves to disk
        - [ ] as a separate program, loads the structure and does the same lookups
    - [ ] if not built in:
        - [ ] weigh difficulty of saving structure to disk, vs not.
    - [ ] put up sample code for both variants
- [ ] major questions
    - [ ] how long does it take to build the structure?
    - [ ] how long does it take to save to disk?
    - [ ] how long does it take to load to disk?
    - [ ] can it operate directly on an on-disk structure, via memory mapping or similar?
        - [ ] or can it be red from disk into a buffer that can be operated directly?
        - [ ] or is a complex/slow/memory-intensive deserialization step necessary before you can operate on it?
    - [ ] once loaded, how long does it take to perform an exact match query?
        - [ ] how long does it take to perform one million queries of different exact-match strings?
        - [ ] how long does it take to perform one million different queries of strings of edit-distance 1 from the correct word?
            - [ ] edit distance 2?
            - [ ] are the times different?
    - [ ] how big is the structure on disk?
        - [ ] does this vary based on the edit distance you want to be able to query?


####
