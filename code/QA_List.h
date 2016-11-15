#ifndef _QA_L_
#define _QA_L_
#include "dp_Objects.h"
#include "Relation.h"

struct ParamPair {
  parameter params[2];
  ParamPair(parameter p1, parameter p2);
  parameter& operator()(unsigned int i);
	const parameter& operator()(unsigned int i) const;
};

struct ParamPairPtr_less { // These structs compare the second, newID value
  template<class PPIt>
  inline bool operator() (const PPIt& pp1, const PPIt& pp2) {
    return ((*pp1)(1).value < (*pp2)(1).value);
  }
};

struct ParamPair_less {
  inline bool operator() (const ParamPair& pp1, const ParamPair& pp2) {
    return (pp1(1).value < pp2(1).value);
  }
};

struct ParamPairPtr_equal {
  template<class PPIt>
  inline bool operator() (const PPIt& pp1, const PPIt& pp2) {
    return ((*pp1)(1).value == (*pp2)(1).value);
  }
};

class QAList {
public:
  QAList();
  QAList(Relation R, vector</*shared_ptr<parameter>*/parameter> P);
  void push_back(ParamPair PP);
  parameter& operator()(unsigned int i, unsigned int j);
	const parameter& operator()(unsigned int i, unsigned int j) const;
  ParamPair& operator()(unsigned int i);
	const ParamPair& operator()(unsigned int i) const;
	unsigned int size();
	unsigned int size() const;
  void sort();
  vector<ParamPair> & get_cdo();
  ~QAList();
private:
  vector<ParamPair> core_data_obj;
  unsigned int cdo_size;
};

template<class It> It uniquify(It begin, It const end) {
    std::vector<It> v;
    v.reserve(static_cast<size_t>(std::distance(begin, end)));
    for (It i = begin; i != end; ++i) {
      v.push_back(i);
    }
    std::sort(v.begin(), v.end(), ParamPairPtr_less());
    v.erase(std::unique(v.begin(), v.end(), ParamPairPtr_equal()), v.end());
    std::sort(v.begin(), v.end());
    size_t j = 0;
    for (It i = begin; i != end && j != v.size(); ++i) {
        if (i == v[j]) {
            using std::iter_swap; iter_swap(i, begin);
            ++j;
            ++begin;
        }
    }
    return begin;
}

#endif
