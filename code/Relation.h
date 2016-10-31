#ifndef _REL_
#define _REL_
#include "dp_Objects.h"
#include "center_text.h"
// uncomment to disable assert()
// #define NDEBUG
#include <cassert>
#include <unordered_set>
#include <functional>
#include <iterator>
#include <iomanip>

/* NOTE: Implicit assumption that header will be ID type and items will be
         STRING type--anomalies will be handled in the "upper levels".

         This way the hashing function for unordered_set will be greatly
         simplified.
*/

struct Tuple { // NOTE: ASSUMES THAT YOU ARE RECEIVING "NORMAL" PARAMETERS (!!)
  vector<string> Items;
  Tuple() {}
  Tuple(vector<shared_ptr<parameter>> _Parameters) {
    for (size_t i = 0; i < _Parameters.size(); i++) {
      Items.push_back(_Parameters[i]->value);
    }
  }
  Tuple(vector<string> _Items) {
    for (const auto& item : _Items) Items.push_back(item);
  }
  bool operator==(const Tuple & other) const {
    if (Items.size() != other.Items.size()) return false;
    for (size_t i = 0; i < Items.size(); i++) {
      if (Items[i] != other.Items[i]) return false;
    }
    return true;
  }
  /** Copy constructor */
  Tuple(const Tuple & other) {
    for (size_t i = 0; i < other.Items.size(); i++) {
      Items.push_back(other.Items[i]);
    }
  }
  /** Move assignment operator */
  Tuple & operator=(Tuple && other) noexcept {
    Items.clear();
    for (size_t i = 0; i < other.Items.size(); i++) {
      Items.push_back(other.Items[i]);
    }
    other.Items.clear();
    return *this;
  }
  /** Copy assignment operator */
  Tuple & operator=(const Tuple & other) {
    Tuple tmp(other);               // re-use Copy constructor
    *this = std::move(tmp);         // re-use Move assignment operator
    return *this;
  }
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
  bool operator==(const Relation & other) const;
  Relation Select(parameter _Att, Comparator _comp, parameter _Inst);// NOTE: Assumes only EQUALS comparator
  Relation Project(vector<parameter> _Att_list);
  Relation Rename(parameter _Att, parameter _new_Att);
  //Relation Nat_Join();
  string to_String();
  string to_String_test();
  string get_Name();
  const string & get_Name() const;
  Tuple get_Header();
  const Tuple & get_Header() const;
  my_set get_Rows();
  const my_set & get_Rows() const;
  ~Relation();
private:
  string Name;
  Tuple Header;
  size_t num_columns;
  my_set Rows;
};

#endif
