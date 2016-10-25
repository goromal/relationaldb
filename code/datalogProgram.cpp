#include "datalogProgram.h"

datalogProgram::datalogProgram() {}

void datalogProgram::add_Scheme(predicate _predicate) {
  Schemes.push_back(_predicate);
}

void datalogProgram::add_Fact(predicate _predicate) {
  Facts.push_back(_predicate);
}

void datalogProgram::add_Rule(rule _rule) {
  Rules.push_back(_rule);
}

void datalogProgram::add_Query(predicate _predicate) {
  Queries.push_back(_predicate);
}

void datalogProgram::compile_Domain() { // perhaps call this at end of parser
  /*
    Needs to:
              - only gather strings found in Facts
              - sort them using normal string sort
  */
  Domain.clear();
  for (size_t i = 0; i < Facts.size(); i++) {
    for (size_t j = 0; j < Facts[i].parameters.size(); j++) {
      Domain.push_back(Facts[i].parameters[j]->value);
    }
  }
  sort(Domain.begin(), Domain.end());
  Domain.erase( unique( Domain.begin(), Domain.end() ), Domain.end() );
}

string datalogProgram::to_String() {
  stringstream to_return;
  const char* indent = "  ";
  to_return << "Schemes(" << Schemes.size() << "):\n";
  for (size_t i = 0; i < Schemes.size(); i++) {
    to_return << indent << Schemes[i].to_String() << '\n';
  }
  to_return << "Facts(" << Facts.size() << "):\n";
  for (size_t i = 0; i < Facts.size(); i++) {
    to_return << indent << Facts[i].to_String() << ".\n";
  }
  to_return << "Rules(" << Rules.size() << "):\n";
  for (size_t i = 0; i < Rules.size(); i++) {
    to_return << indent << Rules[i].to_String() << ".\n";
  }
  to_return << "Queries(" << Queries.size() << "):\n";
  for (size_t i = 0; i < Queries.size(); i++) {
    to_return << indent << Queries[i].to_String() << "?\n";
  }
  to_return << "Domain(" << Domain.size() << "):\n";
  for (size_t i = 0; i < Domain.size(); i++) {
    to_return << indent << Domain[i] << "\n";
  }
  return to_return.str();
}

datalogProgram::~datalogProgram() {}

expression::expression() {
  lp = shared_ptr<parameter>(new parameter());
  rp = shared_ptr<parameter>(new parameter());
  op = UNDEFINED;
}

expression::expression(shared_ptr<parameter> _lp, TokenType _op, shared_ptr<parameter> _rp) {
  lp = _lp;
  rp = _rp;
  op = _op;
}

string expression::to_String() {
  stringstream to_return;
  to_return << "(";
  to_return << lp->to_String();
  if (op == ADD) to_return << '+';
  else to_return << '*';
  to_return << rp->to_String();
  to_return << ")";
  return to_return.str();
}

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

predicate::predicate(string _name, shared_ptr<parameter> _p) {
  name = _name;
  parameters.push_back(_p);
}

void predicate::add_Param(shared_ptr<parameter> _p) {
  parameters.push_back(_p);
}

string predicate::to_String() {
  stringstream to_return;
  const char* separator = "";
  to_return << name << "(";
  for (size_t i = 0; i < parameters.size(); i++) {
    to_return << separator << parameters[i]->to_String();
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
