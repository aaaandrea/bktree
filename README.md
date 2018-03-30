## BK Tree

BK implementation taken from [here](https://daniel-j-h.github.io/post/nearest-neighbors-in-metric-spaces/).
Using a basic Levenshtein Distance calculation.


To build:

```
# clone BKTree and build it
$ git clone https://github.com/aaaandrea/bktree.git
$ cd bktree
$ make
```

To query:
eg. query a structure built with a file titles `us-address-words.txt`

```
$ query.o us-address
```
