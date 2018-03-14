#include <climits>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <bitset>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <ostream>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

// copied from: https://daniel-j-h.github.io/post/nearest-neighbors-in-metric-spaces/


// Traits

template <typename Fn, typename... Args> //
using ReturnType = decltype(std::declval<Fn>()(std::declval<Args>()...));

template <typename Ret, typename Fn, typename... Args> //
using HasReturnType = typename std::is_same<Ret, ReturnType<Fn, Args...>>::type;


// Specialize for tree printing your own distance and value types:
// - StdoutRep must be constructible from T
// - StdoutRep must have an overloaded operator<<(std::ostream&, StdoutRep<T>)
template <typename T> struct StdoutRep;

template <> struct StdoutRep<std::uint32_t>
{
  StdoutRep(std::uint32_t v) : value(v) {}

  friend std::ostream& operator<<(std::ostream& out, StdoutRep rep) {
    return out << std::bitset<sizeof(std::uint32_t) * CHAR_BIT>{rep.value};
  }

  const std::uint32_t value;
};

template <typename T> auto makeStdoutRep(const T& v) { return StdoutRep<T>(v); }


// Burkhard-Keller Tree for fast nearest neighbor queries in metric spaces.
// "Some approaches to best-match file searching", Burkhard and Keller
//
// Works best for discrete metric spaces with a mix of equal and inequal pairwise
// distances. As this property will shape the tree, the worst case for nearest
// neighbor queries is O(n) in case all pairwise distances are the same.
//
// The tree will store immutable values of type T in its nodes. Feel free to
// use std::ref or pointers as T and adapt your Distance type to the indirection.
//
// Distance must be metric on T, i.e. for x, y, z of type T and d of type Distance:
//  - d(x, y) >= 0
//  - d(x, y) = 0 <=> x = y
//  - d(x, y) = d(y, x)
//  - d(x, z) <= d(x, y) + d(y, z)
template <typename T, typename Distance> class BKTree
{
private:
  using DistanceReturnType = ReturnType<Distance, T, T>;

  // The tree is made up of nodes storing a value and edges to sub-trees.
  // The edges represent distances between the node's value and its sub-tree.

  struct Node {
    using Edges = std::vector<DistanceReturnType>;
    using Children = std::vector<Node>;

    Node(T value_) : value{std::move(value_)}, edges{}, children{} {}
    Node(T value_, Edges edges_, Children children_)
        : value{std::move(value_)}, edges{std::move(edges_)}, children{std::move(children_)} {}

    // Sorts the sub-tree parallel arrays by edge representation.
    // Builds index vector sorted by edges, then reorders via indices.
    void sort()
    {
      const auto len = edges.size();

      if (len < 2)
        return;

      std::vector<std::size_t> index(len);
      std::iota(begin(index), end(index), 0);
      std::sort(begin(index), end(index), [&](auto lhs, auto rhs) { return edges[lhs] < edges[rhs]; });

      Edges sortedEdges;
      Children sortedChildren;

      sortedEdges.reserve(len);
      sortedChildren.reserve(len);

      for (std::size_t idx = 0; idx < len; ++idx)
      {
        sortedEdges.push_back(std::move(edges[index[idx]]));
        sortedChildren.push_back(std::move(children[index[idx]]));
      }

      swap(edges, sortedEdges);
      swap(children, sortedChildren);
    }

    // Layout: value is always needed for distance comparison, edges will be
    // scanned via binary search, children are selectively used for descending.
    // Note: parallel arrays with edge representation and sub-tree nodes.

    const T value;
    Edges edges;
    Children children;
  };

  // Inserting values happens one at a time: traverse the tree and follow edges
  // where the edge representation is equal to d(node.value, value). If there is
  // no such edge insert a new child. Otherwise recurse down the tree.
  //
  // Note: insertion temporarily violates the pre-conditions for querying, i.e.
  // edge representations need to be sorted for binary search. To restore the
  // pre-conditions sort() must be called. Inserting all values and sorting
  // afterwards avoids us having to re-sort after inserting each value.

  void insert(T value)
  {
    if (root)
      insert(*root, std::move(value));
    else
      root = std::make_unique<Node>(std::move(value));
  }

  template <typename InputIt> void insert(InputIt first, InputIt last) {
    std::for_each(first, last, [this](auto v) { this->insert(v); });
  }

  void insert(Node& node, T value)
  {
    std::stack<Node*> recursion;
    recursion.push(&node);

    while (!recursion.empty())
    {
      auto next = recursion.top();
      recursion.pop();

      auto dist = distance(next->value, value);

      // Either no subtree with this distance, insert new node
      // Or there is a subtree with this distance, recurse down
      auto it = std::find(begin(next->edges), end(next->edges), dist);

      if (it == end(next->edges))
      {
        next->edges.push_back(std::move(dist));
        next->children.push_back(Node{std::move(value)});
      } else {
        auto at = std::distance(begin(next->edges), it);
        recursion.push(&next->children[at]);
      }
    }
  }

  // Sorting the tree is required for doing binary searches on the edge representations.
  // Needs to be called after bulk-insertion and before querying for nearest neighbors.
  // Walks the tree recursively sorting all node's childrens by edge representation.
  void sort()
  {
    if (!root)
      return;

    std::stack<Node*> recursion;
    recursion.push(root.get());

    while (!recursion.empty())
    {
      auto next = recursion.top();
      recursion.pop();

      next->sort();

      for (std::size_t at = 0; at < next->edges.size(); ++at)
      {
        recursion.push(&next->children[at]);
      }
    }
  }

  // Querying for nearest neighbors happens by traversing the tree following edges
  // recursively where the edge representation is in the range [d - k, d + k] with:
  // d is d(node.value, value) and k is the max. allowed difference to the distance.
  //
  // Note: pre-condition for nearest neighbor queries is a sorted tree. We need the
  // edge representations to be sorted for doing binary searches on them. See sort().

  template <typename OutputIt> void nearest(const Node& node, const T& value, std::size_t delta, OutputIt out) const
  {
    std::stack<const Node*> recursion;
    recursion.push(&node);

    while (!recursion.empty())
    {
      auto next = recursion.top();
      recursion.pop();

      const auto dist = distance(next->value, value);

      if (dist <= delta) {
        *out = next->value;
        ++out;
      }

      const auto minDist = dist > delta ? dist - delta : 0;
      const auto maxDist = dist + delta;

      const auto first = std::lower_bound(begin(next->edges), end(next->edges), minDist);
      const auto last = std::upper_bound(first, end(next->edges), maxDist);

      for (auto it = first; it != last; ++it)
      {
        auto at = std::distance(first, it);
        recursion.push(&next->children[at]);
      }
    }
  }

  // Printing happens by traversing the tree following edges recursively.

  void printTextToStdout(const Node& node) const
  {
    using Indentation = std::size_t;

    std::stack<std::pair<const Node*, Indentation>> recursion;
    recursion.push(std::make_pair(&node, 0));

    while (!recursion.empty())
    {
      auto next = recursion.top();
      recursion.pop();

      const auto valueRep = makeStdoutRep(next.first->value);

      std::cout << std::string(next.second, ' ') << "- Node: [ " << valueRep << " ] with Edges: [ ";

      for (auto dist : next.first->edges)
      {
        const auto distRep = makeStdoutRep(dist);
        std::cout << distRep << ' ';
      }

      std::cout << "]" << std::endl;

      for (const auto& child : next.first->children)
      {
        recursion.push(std::make_pair(&child, next.second + 4));
      }
    }
  }

  void printDotToStdout(const Node& node) const
  {
    std::stack<const Node*> recursion;
    recursion.push(&node);

    std::cout << "graph bktree {" << std::endl;

    while (!recursion.empty()) {
      auto next = recursion.top();
      recursion.pop();

      for (std::size_t at = 0; at < next->edges.size(); ++at)
      {
        const auto& edge = next->edges[at];
        const auto& child = next->children[at];

        const auto valueRep = makeStdoutRep(next->value);
        const auto childValueRep = makeStdoutRep(child.value);
        const auto edgeRep = makeStdoutRep(edge);

        std::cout << "  " << valueRep << " -- " << childValueRep << "[label=\"" << edgeRep << "\"];" << std::endl;

        recursion.push(&next->children[at]);
      }
    }

    std::cout << "}" << std::endl;
  }

public:
  // Constructs a BK-Tree based on a range of items and a distance function for pairs of items
  template <typename InputIt> //
  BKTree(InputIt first, InputIt last, const Distance& distance_ = Distance()) : distance{distance_}
  {
    insert(first, last);
    sort();
  }

  // Queries the BK-Tree for items with a distance of at most delta away from value.
  // Writes results into out. Results are not sorted by distance.
  template <typename OutputIt> //
  void nearest(const T& value, const DistanceReturnType delta, OutputIt out) const
  {
    if (root)
      nearest(*root, value, delta, out);
  }

  // Queries the BK-Tree for items with a distance of at most delta away from value.
  // Writes the n best results sorted by distance into out.
  template <typename OutputIt> //
  void nearest(const T& value, const DistanceReturnType delta, std::size_t n, OutputIt out) const
  {
    std::vector<T> near;
    near.reserve(n);

    nearest(value, delta, back_inserter(near));

    n = std::min(n, near.size());

    auto byDistanceToValue = [&](const T& lhs, const T& rhs) { return distance(value, lhs) < distance(value, rhs); };

    std::nth_element(begin(near), begin(near) + n, end(near), byDistanceToValue);
    std::sort(begin(near), begin(near) + n, byDistanceToValue);

    std::copy_n(begin(near), n, out);
  }

  // Outputs the BK-Tree to stdout.
  void printTextToStdout() const
  {
    if (root)
      printTextToStdout(*root);
  }

  // Outputs the BK-Tree in DOT format to stdout.
  // dot -Tsvg -Grankdir="LR" bk.dot -o bk.svg
  void printDotToStdout() const
  {
    if (root)
      printDotToStdout(*root);
  }

private:
  const Distance& distance;
  std::unique_ptr<Node> root;
};

// Convenience construction helper for type deduction; see BKTree's constructor.
template <typename InputIt, typename Distance>
auto makeBKTree(InputIt first, InputIt last, const Distance& distance = Distance())
{
  return BKTree<typename std::iterator_traits<InputIt>::value_type, Distance>{first, last, distance};
}
