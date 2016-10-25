#ifndef _DLP_
#define _DLP_
#include "dp_Objects.h"
#include "Relation.h"

class datalogProgram {
public:
  datalogProgram();
  void add_Scheme(predicate _predicate);
  void add_Fact(predicate _predicate);
  void add_Rule(rule _rule);
  void add_Query(predicate _predicate);
  void compile_Domain();
  string to_String();
  ~datalogProgram();
//
  vector<predicate> Schemes;
  vector<predicate> Facts;
  vector<rule> Rules;
  vector<predicate> Queries;
  vector<string> Domain;
private:
  map<string,Relation> Relations;
};

#endif
