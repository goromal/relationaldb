#ifndef _REL_
#define _REL_
#include "dp_Objects.h"
#include "center_text.h"
// uncomment to disable assert()
#define NDEBUG
#include <cassert>
#include <unordered_set>
#include <functional>
#include <iterator>
#include <iomanip>

class Relation;

/* NOTE: Implicit assumption that header will be ID type and items will be
         STRING type--anomalies will be handled in the "upper levels".

         This way the hashing function for unordered_set will be greatly
         simplified.
*/

struct Tuple { // NOTE: ASSUMES THAT YOU ARE RECEIVING "NORMAL" PARAMETERS (!!)
  vector<string> Items;
  Tuple() {}
  Tuple(vector<parameter> _Parameters);
  Tuple(vector<string> _Items);
  bool operator==(const Tuple & other) const;
  /** Copy constructor */
  Tuple(const Tuple & other);
  /** Move assignment operator */
  Tuple & operator=(Tuple && other) noexcept;
  /** Copy assignment operator */
  Tuple & operator=(const Tuple & other);
};

/** UNORDERED_SET SETUP
      - to allow for easy intersection and sorting of Tuples with
        std::unordered_set
*/

template <typename InIt1, typename InIt2, typename OutIt>
OutIt unordered_set_intersection(InIt1 b1, InIt1 e1, InIt2 b2, InIt2 e2, OutIt out) {
  while (!(b1 == e1)) {
    if (!(std::find(b2, e2, *b1) == e2)) {
      *out = *b1;
      ++out;
    }
    ++b1;
  }
  return out;
}

template <typename InIt1, typename InIt2, typename OutIt>
OutIt unordered_set_difference(InIt1 b1, InIt1 e1, InIt2 b2, InIt2 e2, OutIt out) {
  while (!(b1 == e1)) {
    if (std::find(b2, e2, *b1) == e2) {
      *out = *b1;
      ++out;
    }
    ++b1;
  }
  return out;
}

struct TupleHash {
  size_t operator()(const Tuple& T) const {
    std::hash<string> hasher;
    size_t seed = 0;
    for (auto& i : T.Items) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    return seed;
  }
};

struct index_tracker { // (#1,#2) index of first tuple and corresponding index in second
  size_t indexes[2];
  index_tracker(size_t i1, size_t i2);
  index_tracker();
  size_t& operator()(unsigned int i);
	const size_t& operator()(unsigned int i) const;
};

struct nat_join_helper {
  index_tracker max_indexes;
  vector<index_tracker> common_indexes;
  vector<size_t> first_disjoint_indexes;
  vector<size_t> second_disjoint_indexes;
  nat_join_helper(const Relation& a, const Relation& b);
};

using my_set = std::unordered_set<Tuple, TupleHash>;

/** RELATION CLASS SETUP
      - define all relevant operations
*/

class Relation {
public:
  Relation();
  Relation(const predicate & definition); // For external declarations
  Relation(const predicate & definition, const predicate & instance); // Also for external declarations
  Relation(string _Name, Tuple _Header, my_set _Rows); // For internal declarations
  void add_Row(predicate instance);
  Relation operator+(const Relation & other) const; // UNION OPERATOR
  Relation operator*(const Relation & other) const; // INTERSECTION OPERATOR
  Relation operator-(const Relation & other) const; // MINUS OPERATOR
  Relation operator%(const Relation & other) const; // NATURAL JOIN OPERATOR
  bool operator==(const Relation & other) const;
  Relation Select(parameter _Att, Comparator _comp, parameter _Inst);// NOTE: Assumes only EQUALS comparator
  Relation Project(const vector<parameter> & _Att_list);
  Relation Rename(const parameter & _Att, const parameter & _new_Att);
  Relation Rename(const vector<parameter> & new_header); // For safe, simultaneous full-header changes
  string to_String();
  string to_String_test();
  string & get_Name();
  const string & get_Name() const;
  Tuple & get_Header();
  const Tuple & get_Header() const;
  my_set & get_Rows();
  const my_set & get_Rows() const;
  ~Relation();
private:
  string Name;
  Tuple Header;
  size_t num_columns;
  my_set Rows;
};

#endif
