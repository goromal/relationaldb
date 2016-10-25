#include "Relation.h"

vector<string> explode(const string& str, const char& ch);
Relation create_Relation(string name, string attList, string tableVals);
parameter param(string name);
vector<parameter> param_list(string names);
void run_Test(int tNum, Relation iR, string operation_sequence, Relation output,
              Relation expected_output, int & tP);

int main() {
  int tests_passed = 0;
// ==============================TEST 1:========================================
/**
      Test if projection on a two-column relation works correctly
*/
  Relation S = create_Relation("S","A,B","Alan,Andrew;Josh,Sam;Isaac,Nate");
  Relation SproA = create_Relation("S","A","Alan;Josh;Isaac");
  run_Test(1, S, "Project[A](S)",
           S.Project(param_list("A")),
           SproA, tests_passed);
// ==============================TEST 2:========================================
/**
      Test if 2-column projection on a four-column relation works correctly
*/
  stringstream tableVals;
  tableVals << "12345,C. Brown,12 Apple,555-1231;33333,Snoopy,12 Apple,555-1234";
  Relation SNAP = create_Relation("snap","S,N,A,P",tableVals.str());
  Relation SNAPproNP = create_Relation("snap","N,P","C. Brown,555-1231;Snoopy,555-1234");
  run_Test(2, SNAP, "Project[N,P](snap)",
           SNAP.Project(param_list("N,P")),
           SNAPproNP, tests_passed);
// ==============================TEST 3:========================================
/**
      Test if a one-column rename works correctly
*/
  Relation QS = create_Relation("Qs","A,B,C","1,2,3;4,5,6;7,8,9");
  Relation QSrenAZ = create_Relation("Qs","Z,B,C","1,2,3;4,5,6;7,8,9");
  run_Test(3, QS, "Rename[Z\\A](Qs)",
           QS.Rename(parameter(ID,"A"),parameter(ID,"Z")),
           QSrenAZ, tests_passed);
// ==============================TEST 4:========================================
/**
      Test if a 3-column rename works correctly
*/
  tableVals.str("");
  tableVals << "1,2,3,4,5,6;7,8,9,10,11,12;13,14,15,16,17,18;";
  tableVals << "19,20,21,22,23,24";
  Relation T = create_Relation("T","A,B,C,D,E,F",tableVals.str());
  Relation Tren_BCD_LMN = create_Relation("T","A,L,M,N,E,F",tableVals.str());
  run_Test(4, T, "Rename[N\\D](Rename[M\\C](Rename[L\\B](T)))",
           T.Rename(parameter(ID,"B"),parameter(ID,"L")).Rename(parameter(ID,"C"),parameter(ID,"M")).Rename(parameter(ID,"D"),parameter(ID,"N")),
           Tren_BCD_LMN, tests_passed);
// ==============================TEST 5:========================================
/**
      Test if a single select operation works correctly
*/
  Relation couples = create_Relation("couples","Husband,Wife","Alex,Jessica;Frank,Molly;Kyle,Sarah;Alex,Melaine");
  Relation couples_selHA = create_Relation("couples","Husband,Wife","Alex,Jessica;Alex,Melaine");
  run_Test(5, couples, "Select[Husband=Alex](couples)",
           couples.Select(parameter(ID,"Husband"),EQUALS,parameter(STRING,"Alex")),
           couples_selHA, tests_passed);
// ==============================TEST 6:========================================
/**
      Test if a compounded select operation works correctly
*/
  Relation couples_selHAWM = create_Relation("couples","Husband,Wife","Alex,Melaine");
  run_Test(6, couples, "Select[Wife=Melaine](Select[Husband=Alex](couples))",
           couples.Select(parameter(ID,"Husband"),EQUALS,parameter(STRING,"Alex")).Select(parameter(ID,"Wife"),EQUALS,parameter(STRING,"Melaine")),
           couples_selHAWM, tests_passed);
// ==============================TEST 7:========================================<<<< COMPOUND TESTS HERE
/**
      Testing Select-Project-Rename combination Pt. 1
        - Straight-forward sequence
*/
  tableVals.str("");
  tableVals << "Andrew S.,Andrew S.,25,S. Carolina;Mckay L.,John L.,16,California;";
  tableVals << "Julia T.,Samuel T.,42,Brisbon;Max L.,Max L.,23,Iowa;";
  tableVals << "Henry R.,Cynthia R.,19,Oklahoma;Maria D.,Juan D.,34,New Mexico";
  string headerVals = "Individual,Head of House,Age,Origin";
  Relation aBook = create_Relation("aBook",headerVals,tableVals.str());

  Relation aBookt1 = create_Relation("aBook","A","Mckay L.");
  run_Test(7, aBook, "Rename[A\\Individual](Project[Individual](Select[Origin=California](aBook)))",
           aBook.Select(parameter(ID,"Origin"),EQUALS,parameter(STRING,"California")).Project(param_list("Individual")).Rename(parameter(ID,"Individual"),parameter(ID,"A")),
           aBookt1, tests_passed);
// ==============================TEST 8:========================================
/**
      Testing Select-Project-Rename combination Pt. 2
        - Projecting multiple columns
*/
  Relation aBookt2 = create_Relation("aBook","Name,Age","Henry R.,19");
  run_Test(8, aBook, "Rename[Name\\Individual](Project[Individual,Age](Select[Origin=Oklahoma](aBook)))",
          aBook.Select(parameter(ID,"Origin"),EQUALS,parameter(STRING,"Oklahoma")).Project(param_list("Individual,Age")).Rename(parameter(ID,"Individual"),parameter(ID,"Name")),
          aBookt2, tests_passed);
// ==============================TEST 9:========================================
/**
      Testing Select-Project-Rename combination Pt. 3
        - When no item in database matches criteria
*/
  Relation aBookt3 = create_Relation("aBook","Name","");
  run_Test(9, aBook, "Rename[Name\\Individual](Project[Name](Select[Age=44](aBook)))",
          aBook.Select(parameter(ID,"Age"),EQUALS,parameter(STRING,"44")).Project(param_list("Individual")).Rename(parameter(ID,"Individual"),parameter(ID,"Name")),
          aBookt3, tests_passed);
// ==============================TEST 10:=======================================
/**
      Testing Select-Project-Rename combination Pt. 4
        - Setting an ID equal to another ID rather than a STRING
*/
  Relation aBookt4 = create_Relation("aBook","Name,Age","Andrew S.,25;Max L.,23");
  run_Test(10, aBook, "Rename[Name\\Individual](Project[Individual,Age](Select[Individual=Head of House](aBook)))",
          aBook.Select(parameter(ID,"Individual"),EQUALS,parameter(ID,"Head of House")).Project(param_list("Individual,Age")).Rename(parameter(ID,"Individual"),parameter(ID,"Name")),
          aBookt4, tests_passed);
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

void run_Test(int tNum, Relation iR, string operation_sequence, Relation output,
              Relation expected_output, int & tP) {
  int title_width = 36;
  stringstream title;
  title << "TEST " << tNum;
  cout << std::setfill('=') << std::setw(title_width) << '=' << '\n'
       << std::setfill('=') << std::setw(title_width) << centered(title.str()) << '\n'
       << std::setfill('=') << std::setw(title_width) << '=' << "\n\n";
  cout << "Using relation '" << iR.get_Name() << "'...\n\n";
  cout << iR.to_String() << "\n";
  cout << "Performing operation: " << operation_sequence << "...\n\n";
  cout << output.to_String() << "\n";
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
