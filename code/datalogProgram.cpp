#include "datalogProgram.h"

datalogProgram::datalogProgram() {}

void datalogProgram::add_Scheme(predicate _predicate) {
  Schemes.push_back(_predicate);
  Relation scheme_relation(_predicate);
  Relations[scheme_relation.get_Name()] = scheme_relation;
}

void datalogProgram::add_Fact(predicate _predicate) {
  Facts.push_back(_predicate);
  Relations[_predicate.name].add_Row(_predicate);
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
  //----------------------FOR TESTING PURPOSES ONLY-----------------------------
  to_return << "Relations(" << Relations.size() << "):\n";
  for (auto n_it : Relations) {
    to_return << n_it.second.to_String();
  }
  return to_return.str();
}

datalogProgram::~datalogProgram() {}
