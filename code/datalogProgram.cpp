#include "datalogProgram.h"

datalogProgram::datalogProgram() : num_rules(0) {}

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
  program_print_map[num_rules] = "R" + std::to_string(num_rules);
  num_rules++;
}

void datalogProgram::construct_Graph() {
  /**
      Reason for dependencies pointing to their source and not the other way around:
      ...
  */
  vector<rule>::iterator rule_it;
  for (size_t i = 0; i < Rules.size(); i++) {
    rule_graph.add_node(i);
    for (size_t j = 0; j < Rules[i].predicates.size(); j++) {
      rule search_rule(Rules[i].predicates[j]);
      for (rule_it = std::find(Rules.begin(), Rules.end(), search_rule);
           rule_it != Rules.end(); rule_it = std::find(rule_it+1, Rules.end(), search_rule))
        rule_graph.add_edge(i, rule_it - Rules.begin());
    }
  }
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
      Domain.push_back(Facts[i].parameters[j].value);
    }
  }
  sort(Domain.begin(), Domain.end());
  Domain.erase( unique( Domain.begin(), Domain.end() ), Domain.end() );
}

bool datalogProgram::evaluate_Rule(const rule & r) {
  // Natural Join right predicates
  Relation natural_joined = construct_QAR(Relations[r.predicates[0].name],r.predicates[0].parameters);
  Show_Commands.clear();
  for (size_t i = 1; i < r.predicates.size(); i++) {
    natural_joined = natural_joined % construct_QAR(Relations[r.predicates[i].name],r.predicates[i].parameters);
    Show_Commands.clear();
  }
  // Project special params of head predicate
  natural_joined = natural_joined.Project(r.headPred.parameters);
  // Rename special params to be union compatible
  QAList QAL(Relations[r.headPred.name],r.headPred.parameters);
  make_Renames(natural_joined, QAL, true);
  // Union rule relation with original relation and check for size change
  size_t size_before = Relations[r.headPred.name].get_Rows().size();
  Relations[r.headPred.name] = Relations[r.headPred.name] + natural_joined;
  return (Relations[r.headPred.name].get_Rows().size() > size_before);
}

void datalogProgram::evaluate_until_fixed(vector<int> rule_indexes) {
  bool items_added = false;
  num_rule_iterations.push_back(0);
  do {
    items_added = false;
    for (const auto& rule_index : rule_indexes)
      if (evaluate_Rule(Rules[rule_index]))
        items_added = true;
    num_rule_iterations[num_rule_iterations.size()-1]++;
  } while(items_added);
}

void datalogProgram::evaluate_Rules() {
  construct_Graph();
  rule_graph.get_SCC(SCCs);
  // Process one SCC at a time
  for (const auto& SCC : SCCs)
    if (SCC.size() == 1 && !rule_graph.has_self_loop(SCC[0])) { // Trivial case
      num_rule_iterations.push_back(1);
      evaluate_Rule(Rules[SCC[0]]);
    }
    else
      evaluate_until_fixed(SCC);
}

void datalogProgram::answer_Queries() {
  if (Rules.size() > 0)
    evaluate_Rules();
  for (size_t i = 0; i < Queries.size(); i++) {
    Query_Answers.push_back(construct_QAR(Relations[Queries[i].name],
      Queries[i].parameters));
  }
}

Relation datalogProgram::construct_QAR(Relation R, vector<parameter> P) { // X&
  QAList QAL(R, P);
  Relation RCopy(R.get_Name(), R.get_Header(), R.get_Rows());
  make_Selects(RCopy, QAL);
  QAL.sort();
  make_Projects(RCopy, QAL);
  make_Renames(RCopy, QAL);
  return RCopy;
}

void datalogProgram::make_Selects(Relation & R, const QAList & QAL) {
  QAList pure_IDs;
  for (size_t i = 0; i < QAL.size(); i++) {
    if (QAL(i,1).token == STRING)
      R = R.Select(QAL(i,0), EQUALS, QAL(i,1));
    else
      pure_IDs.push_back(QAL(i));
  }
  std::sort(pure_IDs.get_cdo().begin(), pure_IDs.get_cdo().end(), ParamPair_less());
  for (size_t i = 0; i < pure_IDs.size(); i++)
    if (i < pure_IDs.size() - 1 && pure_IDs(i,1).value == pure_IDs(i+1,1).value)
      R = R.Select(pure_IDs(i,0), EQUALS, pure_IDs(i+1,0));
}

void datalogProgram::make_Projects(Relation & R, const QAList & QAL) {
  vector<parameter> Att_List;
  if (QAL.size() == 0) {
    Show_Commands.push_back(false);
    return;
  }
  else Show_Commands.push_back(true);
  for (size_t i = 0; i < QAL.size(); i++)
    Att_List.push_back(QAL(i,0));
  R = R.Project(Att_List);
}

void datalogProgram::make_Renames(Relation & R, const QAList & QAL, bool reverse) {
  vector<parameter> new_header;
  if (reverse)
    for (size_t i = 0; i < QAL.size(); i++)
      new_header.push_back(QAL(i,0));
  else
    for (size_t i = 0; i < QAL.size(); i++)
      new_header.push_back(QAL(i,1));
  R = R.Rename(new_header);
}

string datalogProgram::to_String() {
  stringstream ss;
  ss << "Dependency Graph\n";
  ss << rule_graph.to_String(program_print_map);
  ss << '\n';
  ss << "Rule Evaluation\n";
  for (size_t i = 0; i < num_rule_iterations.size(); i++) {
    ss << num_rule_iterations[i] << " passes: ";
    string delim = "";
    for (const auto& scc_int : SCCs[i]) {
      ss << delim << program_print_map[scc_int];
      delim = ",";
    }
    ss << '\n';
  }
  ss << '\n';
  ss << "Query Evaluation\n";
  for (size_t i = 0; i < Queries.size(); i++) {
    ss << Queries[i].to_String() << "? ";
    int num_answer_rows = Query_Answers[i].get_Rows().size();
    if (num_answer_rows == 0) ss << "No\n";
    else {
      ss << "Yes(" << num_answer_rows << ")\n";
      if (Show_Commands[i]) ss << Query_Answers[i].to_String();
    }
  }
  return ss.str();
}

datalogProgram::~datalogProgram() {}
