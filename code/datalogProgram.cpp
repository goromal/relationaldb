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

void datalogProgram::answer_Queries() {
  for (size_t i = 0; i < Queries.size(); i++) {
    Query_Answers.push_back(construct_QAR(Relations[Queries[i].name],
      Queries[i].parameters));
  }
}

Relation datalogProgram::construct_QAR(Relation R, vector<shared_ptr<parameter>> P) {
  QAList QAL(R, P);
  Relation RCopy(R.get_Name(), R.get_Header(), R.get_Rows());
  make_Selects(RCopy, QAL);
  QAL.sort();
  make_Projects(RCopy, QAL);
  make_Renames(RCopy, QAL);
  return RCopy;
}

void datalogProgram::make_Selects(Relation & R, QAList QAL) {
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

void datalogProgram::make_Projects(Relation & R, QAList QAL) {
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

void datalogProgram::make_Renames(Relation & R, QAList QAL) {
  for (size_t i = 0; i < QAL.size(); i++) {
    R = R.Rename(QAL(i,0), QAL(i,1));
  }
}

string datalogProgram::to_String() {
  stringstream ss;
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
