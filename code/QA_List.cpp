#include "QA_List.h"

ParamPair::ParamPair(parameter p1, parameter p2) {
  params[0] = p1;
  params[1] = p2;
}

parameter& ParamPair::operator()(unsigned int i) {
  assert(i <= 1);
  return params[i];
}

const parameter& ParamPair::operator()(unsigned int i) const {
  assert(i <= 1);
  return params[i];
}

QAList::QAList() {
  cdo_size = 0;
}

QAList::QAList(Relation R, vector<shared_ptr<parameter>> P) {
  cdo_size = P.size();
  for (size_t i = 0; i < P.size(); i++) {
    parameter Hp(ID, R.get_Header().Items[i]);
    core_data_obj.push_back(ParamPair(Hp, *P[i]));
  }
}

void QAList::push_back(ParamPair PP) {
  core_data_obj.push_back(PP);
  cdo_size++;
}

parameter& QAList::operator()(unsigned int i, unsigned int j) {
  assert(i < cdo_size);
  return core_data_obj[i](j);
}

const parameter& QAList::operator()(unsigned int i, unsigned int j) const {
  assert(i < cdo_size);
  return core_data_obj[i](j);
}

ParamPair& QAList::operator()(unsigned int i) {
  assert(i < cdo_size);
  return core_data_obj[i];
}

const ParamPair& QAList::operator()(unsigned int i) const {
  assert(i < cdo_size);
  return core_data_obj[i];
}

unsigned int QAList::size() {
  return cdo_size;
}

unsigned int QAList::size() const {
  return cdo_size;
}

/**
    sort()
    Removes duplicate newID's, removes tuples with strings, all while preserving
    the order.
*/
void QAList::sort() {
  core_data_obj.erase(uniquify(core_data_obj.begin(), core_data_obj.end()),
    core_data_obj.end());
  core_data_obj.erase(std::remove_if(core_data_obj.begin(), core_data_obj.end(),
    [](const ParamPair& x) {
      if (x(1).token == STRING) return true;
      else return false;
    }), core_data_obj.end());
  cdo_size = core_data_obj.size();
}

vector<ParamPair> & QAList::get_cdo() {
  return core_data_obj;
}

QAList::~QAList() {}
