#include "dp_Objects.h"

parameter::parameter() {
  token = UNDEFINED;
  value = "";
}

parameter::parameter(TokenType _token, string _value) : parameter() {
  token = _token;
  value = _value;
}

string parameter::to_String() {
  return value;
}

parameter::~parameter() {}

predicate::predicate() {
  name = "";
}

predicate::predicate(string _name, /*shared_ptr<parameter>*/parameter _p) {
  name = _name;
  parameters.push_back(_p);
}

predicate::predicate(const predicate& other) {
  name = other.name;
  for (const auto & param_ptr : other.parameters) {
    parameters.push_back(param_ptr);
  }
}

predicate& predicate::operator=(predicate && other) noexcept {
  name = other.name;
  parameters.clear();
  for (const auto & param_ptr : other.parameters) {
    parameters.push_back(param_ptr);
  }
  other.parameters.clear();
  return *this;
}

predicate& predicate::operator=(const predicate& other) {
  predicate tmp(other);
  *this = std::move(tmp);
  return *this;
}

void predicate::add_Param(/*shared_ptr<parameter>*/parameter _p) {
  parameters.push_back(_p);
}

string predicate::to_String() {
  stringstream to_return;
  const char* separator = "";
  to_return << name << "(";
  for (size_t i = 0; i < parameters.size(); i++) {
    to_return << separator << parameters[i].to_String(); // to . from ->
    separator = ",";
  }
  to_return << ")";
  return to_return.str();
}

rule::rule() {
  predicate _headPred;
  headPred = _headPred;
}

rule::rule(predicate _headPred) {
  headPred = _headPred;
}

void rule::add_Pred(predicate _p) {
  predicates.push_back(_p);
}

string rule::to_String() {
  stringstream to_return;
  const char* separator = "";
  to_return << headPred.to_String() << " :- ";
  for (size_t i = 0; i < predicates.size(); i++) {
    to_return << separator << predicates[i].to_String();
    separator = ",";
  }
  return to_return.str();
}

rule::~rule() {}
