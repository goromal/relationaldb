#include "datalogProgram.h"

datalogProgram::datalogProgram() : num_rule_iterations(0) {}

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
      Domain.push_back(Facts[i].parameters[j].value); // to . from ->
    }
  }
  sort(Domain.begin(), Domain.end());
  Domain.erase( unique( Domain.begin(), Domain.end() ), Domain.end() );
}

void datalogProgram::extract_map_sizes(map<string,size_t> & sizes) { // Should only have to call once
  map<string,Relation>::const_iterator it;
  for(it = Relations.begin(); it != Relations.end(); it++) {
    sizes[it->first] = it->second.get_Rows().size();
  }
}

Relation datalogProgram::process_rule(const vector<predicate> & predicates) {
  Relation natural_joined = construct_QAR(Relations[predicates[0].name],predicates[0].parameters);
  Show_Commands.clear();
  for (size_t i = 1; i < predicates.size(); i++) {
    natural_joined = natural_joined % construct_QAR(Relations[predicates[i].name],predicates[i].parameters);
    Show_Commands.clear();
  }
  return natural_joined;
}

void datalogProgram::evaluate_Rules() {
  map<string,Relation>::const_iterator it;
  size_t num_relations = Relations.size();
  map<string,size_t> relation_sizes;
  extract_map_sizes(relation_sizes);
  bool more_relations_added = false;
  do {
    more_relations_added = false;
    for (size_t i = 0; i < Rules.size(); i++) {
      // Natural Join right predicates
      Relation unfiltered_rule_relation = process_rule(Rules[i].predicates);
      // Project special params of head predicate
      unfiltered_rule_relation = unfiltered_rule_relation.Project(Rules[i].headPred.parameters);
      // Rename special params to be union compatible
      QAList QAL(Relations[Rules[i].headPred.name],Rules[i].headPred.parameters);
      make_Renames(unfiltered_rule_relation, QAL, true);
      // Union rule relation with original relation
      Relations[Rules[i].headPred.name] = Relations[Rules[i].headPred.name] + unfiltered_rule_relation;
    }
    // Check for changes
    if (Relations.size() == num_relations) { // if no new relations were added
      for (it = Relations.begin(); it != Relations.end(); it++) {
        if (it->second.get_Rows().size() > relation_sizes[it->first]) {
          more_relations_added = true;
          relation_sizes[it->first] = it->second.get_Rows().size();
        }
      }
    } else {
      more_relations_added = true;
      num_relations = Relations.size();
    }
    num_rule_iterations++;
  } while(more_relations_added);
}

void datalogProgram::answer_Queries() {
  for (size_t i = 0; i < Queries.size(); i++) {
    Query_Answers.push_back(construct_QAR(Relations[Queries[i].name],
      Queries[i].parameters));
  }
}

Relation datalogProgram::construct_QAR(Relation R, vector<parameter> P) {
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
    if (QAL(i,1).token == STRING) R = R.Select(QAL(i,0), EQUALS, QAL(i,1));
    else pure_IDs.push_back(QAL(i));
  }
  std::sort(pure_IDs.get_cdo().begin(), pure_IDs.get_cdo().end(), ParamPair_less());
  for (size_t i = 0; i < pure_IDs.size(); i++) {
    if (i < pure_IDs.size() - 1 && pure_IDs(i,1).value == pure_IDs(i+1,1).value) {
      R = R.Select(pure_IDs(i,0), EQUALS, pure_IDs(i+1,0));
    }
  }
}

void datalogProgram::make_Projects(Relation & R, const QAList & QAL) {
  vector<parameter> Att_List;
  if (QAL.size() == 0) {
    Show_Commands.push_back(false);
    return;
  }
  else Show_Commands.push_back(true);
  for (size_t i = 0; i < QAL.size(); i++) {
    Att_List.push_back(QAL(i,0));
  }
  R = R.Project(Att_List);
}

void datalogProgram::make_Renames(Relation & R, const QAList & QAL, bool reverse) {
  vector<parameter> new_header;
  if (reverse) {
    for (size_t i = 0; i < QAL.size(); i++) {
      new_header.push_back(QAL(i,0));
    }
  } else {
    for (size_t i = 0; i < QAL.size(); i++) {
      new_header.push_back(QAL(i,1));
    }
  }
  R = R.Rename(new_header);
}

string datalogProgram::to_String() {
  stringstream ss;
  ss << "Schemes populated after " << num_rule_iterations << " passes through the Rules.\n";
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
