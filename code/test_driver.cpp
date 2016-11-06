#include "Relation.h"

vector<string> explode(const string& str, const char& ch);
Relation create_Relation(string name, string attList, string tableVals);
parameter param(string name);
vector<parameter> param_list(string names);
void run_Test(int tNum, Relation iR1, Relation iR2, string operation_sequence, Relation output,
              Relation expected_output, int & tP);

int main() {
  int tests_passed = 0;
// ==============================TEST 1:========================================
/**
      Test if a simple union with unique elements works correctly
*/
  Relation RA1 = create_Relation("RA","A,B","0,1;2,3;4,5");
  Relation RA2 = create_Relation("RA","A,B","6,7;8,9");
  Relation RAUnion = create_Relation("RA","A,B","0,1;2,3;4,5;6,7;8,9");
  run_Test(1, RA1, RA2, "RA_1 + RA_2", RA1 + RA2, RAUnion, tests_passed);
// ==============================TEST 2:========================================
/**
      Test if union will ignore duplicate tuples
*/
  Relation S1 = create_Relation("S","A,B,C","0,1,2;3,4,5");
  Relation S2 = create_Relation("S","A,B,C","3,4,5;6,7,8");
  Relation SUnion = create_Relation("S","A,B,C","0,1,2;3,4,5;6,7,8");
  run_Test(2, S1, S2, "S_1 + S_2", S1 + S2, SUnion, tests_passed);
// ==============================TEST 3:========================================
/**
      Test the union of empty relations
*/
  Relation QS1 = create_Relation("Qs","A,B,C","");
  Relation QS2 = create_Relation("Qs","A,B,C","");
  Relation QSUnion = create_Relation("Qs","A,B,C","");
  run_Test(3, QS1, QS2, "Qs_1 + Qs_2", QS1 + QS2, QSUnion, tests_passed);
// ==============================TEST 4:========================================
/**
      Test if natural join handles disjoint relation attributes correctly
*/
  Relation T = create_Relation("T","A,B","1,2;3,4");
  Relation S = create_Relation("S","C,D","5,6;7,8;9,10");
  Relation T_NJ_S = create_Relation("T-NJ-S","A,B,C,D","1,2,5,6;1,2,7,8;1,2,9,10;3,4,5,6;3,4,7,8;3,4,9,10");
  run_Test(4, T, S, "T >< S", T % S, T_NJ_S, tests_passed);
// ==============================TEST 5:========================================
/**
      Test if natural join is commutative for the disjoint case
*/
  Relation S_NJ_T = create_Relation("S-NJ-T","A,B,C,D","1,2,5,6;1,2,7,8;1,2,9,10;3,4,5,6;3,4,7,8;3,4,9,10");
  run_Test(5, T, S, "S >< T", (S % T).Project(param_list("A,B,C,D")), S_NJ_T, tests_passed);
// ==============================TEST 6:========================================
/**
      Test if natural join handles union compatible cases correctly
*/
  Relation R = create_Relation("R","A,B","1,2;3,4;5,6");
  Relation Q = create_Relation("Q","A,B","3,4;5,6;7,8");
  Relation R_NJ_Q = create_Relation("R-NJ-Q","A,B","3,4;5,6");
  run_Test(6, R, Q, "R >< Q", R % Q, R_NJ_Q, tests_passed);
// ==============================TEST 7:========================================<<<< COMPOUND TESTS HERE
/**
      Test if natural join is commutative for the union compatible case
*/
  Relation Q_NJ_R = create_Relation("Q-NJ-R","A,B","3,4;5,6");
  run_Test(7, R, Q, "Q >< R", Q % R, Q_NJ_R, tests_passed);
// ==============================TEST 8:========================================
/**
      Test if natural join handles the "mixed attribute compatibility" case correctly
*/
  Relation M = create_Relation("M","A,B,C","1,2,3;4,5,6;7,8,9");
  Relation N = create_Relation("N","A,E,C","1,3,3;1,5,3;4,6,5;7,7,9;7,10,9;7,11,9");
  Relation M_NJ_N = create_Relation("M-NJ-N","A,B,C,E","1,2,3,3;1,2,3,5;7,8,9,7;7,8,9,10;7,8,9,11");
  run_Test(8, M, N, "M >< N", (M % N).Project(param_list("A,B,C,E")), M_NJ_N, tests_passed);
// ==============================TEST 9:========================================
/**
      Test if natural join is commutative for the "mixed attribute compatibility" case
*/
  Relation N_NJ_M = create_Relation("N-NJ-M","A,B,C,E","1,2,3,3;1,2,3,5;7,8,9,7;7,8,9,10;7,8,9,11");
  run_Test(9, M, N, "N >< M", (N % M).Project(param_list("A,B,C,E")), N_NJ_M, tests_passed);
// ==============================TEST 10:=======================================
/**
      Test if natural join handles the "mixed" case with no coinciding tuples correctly
*/
  Relation Y = create_Relation("Y","A,B,C","6,2,3;4,5,6;8,8,9");
  Relation Y_NJ_N = create_Relation("Y-NJ-N","A,B,C,E","");
  run_Test(10, Y, N, "Y >< N", (Y % N).Project(param_list("A,B,C,E")), Y_NJ_N, tests_passed);
// =============================================================================
  cout << "TESTS PASSED: " << tests_passed << "/10. (" << 10*tests_passed << "%)\n\n";
/*
  Relation a = create_Relation("Q","A,B,C","t1,t4,t2;t6,t7,t8");
  Relation b = create_Relation("Q","A,B,C","t6,t7,t8;s5,s3,s1");
  Relation c = a + b; cout << c.to_String() << endl;
  Relation d = a * b; cout << d.to_String() << endl;
  Relation e = a - b; cout << e.to_String() << endl;
  Relation f = b - a; cout << f.to_String() << endl;
*/
  return 0;
}

vector<string> explode(const string& str, const char& ch) {
  string next;
  vector<string> result;
  for (string::const_iterator it = str.begin(); it != str.end(); it++) {
    if (*it == ch) {
      if (!next.empty()) {
        result.push_back(next);
        next.clear();
      }
    } else {
      next += *it;
    }
  }
  if (!next.empty())
    result.push_back(next);
  return result;
}

Relation create_Relation(string name, string attList, string tableVals) {
  vector<string> attribute_List = explode(attList, ',');
  auto pp = shared_ptr<parameter>(new parameter(ID, attribute_List[0]));
  predicate definition_predicate(name, pp);
  for (size_t i = 1; i < attribute_List.size(); i++) {
    auto pp = shared_ptr<parameter>(new parameter(ID, attribute_List[i]));
    definition_predicate.add_Param(pp);
  }
  Relation relation_creation(definition_predicate);
  vector<string> row_List = explode(tableVals, ';');
  for (size_t i = 0; i < row_List.size(); i++) {
    vector<string> instance_List = explode(row_List[i], ',');
    auto pp = shared_ptr<parameter>(new parameter(STRING, instance_List[0]));
    predicate an_instance_predicate(name, pp);
    for (size_t j = 1; j < instance_List.size(); j++) {
      auto pp = shared_ptr<parameter>(new parameter(STRING, instance_List[j]));
      an_instance_predicate.add_Param(pp);
    }
    relation_creation.add_Row(an_instance_predicate);
  }
  return relation_creation;
}

parameter param(string name) {
  return parameter(STRING, name);
}

vector<parameter> param_list(string names) {
  vector<string> name_list = explode(names, ',');
  vector<parameter> parameter_list;
  for (const auto& name : name_list) {
    parameter_list.push_back(param(name));
  }
  return parameter_list;
}

void run_Test(int tNum, Relation iR1, Relation iR2, string operation_sequence, Relation output,
              Relation expected_output, int & tP) {
  int title_width = 36;
  stringstream title;
  title << "TEST " << tNum;
  cout << std::setfill('=') << std::setw(title_width) << '=' << '\n'
       << std::setfill('=') << std::setw(title_width) << centered(title.str()) << '\n'
       << std::setfill('=') << std::setw(title_width) << '=' << "\n\n";
  cout << "Using relation '" << iR1.get_Name() << "'...\n\n";
  cout << iR1.to_String_test() << "\n";
  cout << "and relation '" << iR2.get_Name() << "'...\n\n";
  cout << iR2.to_String_test() << "\n";
  cout << "Performing operation: " << operation_sequence << "...\n\n";
  cout << output.to_String_test() << "\n";
  if (output == expected_output) {
    cout << std::setfill('-') << std::setw(title_width) << centered("PASSED");
    cout << "\n\n";
    tP++;
  }
  else {
    cout << std::setfill('-') << std::setw(title_width) << centered("FAILED");
    cout << "\n\n";
  }
}
