#ifndef _DLP_
#define _DLP_
#include "QA_List.h"

class datalogProgram {
public:
  datalogProgram();
  void add_Scheme(predicate _predicate);
  void add_Fact(predicate _predicate);
  void add_Rule(rule _rule);
  void add_Query(predicate _predicate);
  void compile_Domain();
  void evaluate_Rules();
  void answer_Queries();
  string to_String();
  ~datalogProgram();
  vector<predicate> Schemes;
  vector<predicate> Facts;
  vector<rule> Rules;
  vector<predicate> Queries;
  vector<string> Domain;
private:
  int num_rule_iterations;
  Relation construct_QAR(Relation R, vector</*shared_ptr<parameter>*/parameter> P);
  void make_Selects(Relation & R, const QAList & QAL);
  void make_Projects(Relation & R, const QAList & QAL);
  void make_Renames(Relation & R, const QAList & QAL, bool reverse = false);
  map<string,Relation> Relations;
  void extract_map_sizes(map<string,size_t> & sizes);
  Relation process_rule(const vector<predicate> & predicates);
  vector<Relation> Query_Answers;
  vector<bool> Show_Commands;
};

#endif
