#ifndef _DLP_
#define _DLP_
#include "QA_List.h"
#include "graph.h"

// (*) = Not used

class datalogProgram {
public:
  datalogProgram();
  void add_Scheme(predicate _predicate);
  void add_Fact(predicate _predicate);
  void add_Rule(rule _rule);
  void add_Query(predicate _predicate);
  void compile_Domain(); // (*)
  void answer_Queries();
  string to_String();
  ~datalogProgram();
  vector<predicate> Schemes;
  vector<predicate> Facts;
  vector<rule> Rules;
  vector<predicate> Queries;
  vector<string> Domain; // (*)
private:
  int num_rules;
  map<int,string> program_print_map;
  void construct_Graph();
  graph rule_graph;
  Relation construct_QAR(Relation R, vector<parameter> P);
  void make_Selects(Relation & R, const QAList & QAL);
  void make_Projects(Relation & R, const QAList & QAL);
  void make_Renames(Relation & R, const QAList & QAL, bool reverse = false);
  map<string,Relation> Relations;
  vector<vector<int>> SCCs;
  vector<int> num_rule_iterations;
  void evaluate_Rules();
  bool evaluate_Rule(const rule & r); // true if new tuples added, otherwise false
  void evaluate_until_fixed(vector<int> rule_indexes);
  vector<Relation> Query_Answers;
  vector<bool> Show_Commands;
};

#endif
