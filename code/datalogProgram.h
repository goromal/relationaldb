#ifndef _DLP_
#define _DLP_
#include "FiniteStateAutomoton.h"

class parameter; // (STRING, ID, expression) << these are arguments
class expression; // parameter operator parameter
struct predicate; // ID(parameter[s]) << essentially a function
struct rule; // predicate :- predicate[s]

/*
  Schemes: simply a list of declared predicates
  Facts: a list of (data) predicates that store real info
  Rules: their own class, operation of predicates
  Queries: a list of passed commands in the form of (operational) predicates
*/

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
};

class parameter { // NOTE: there is only ambiguity within rules (after the :-)
public:
  parameter();
  parameter(TokenType _token, string _value);
  virtual string to_String();
  ~parameter();
//
  TokenType token;
  string value;
};

class expression : public parameter {
public:
  expression();
  expression(shared_ptr<parameter> _lp, TokenType _op, shared_ptr<parameter> _rp);
  expression(const expression&) = default;
  expression& operator=(const expression& other) = default;
  string to_String();
  ~expression() = default;
  shared_ptr<parameter> lp;
  shared_ptr<parameter> rp;
  TokenType op;
};

struct predicate {
  predicate();
  predicate (string _name, shared_ptr<parameter> _p); //                  &&
  predicate (const predicate&) = default; //                              &&
  predicate& operator=(const predicate& other) = default; //              &&
  void add_Param(shared_ptr<parameter> _p); //                            &&
  string to_String();
  ~predicate() = default; //                                              &&
  string name;
  vector<shared_ptr<parameter>> parameters; //                            &&
};

struct rule {
  rule();
  //rule(predicate _headPred, predicate _p);
  rule(predicate _headPred);
  void add_Pred(predicate _p);
  string to_String();
  ~rule();
  predicate headPred;
  vector<predicate> predicates;
};

#endif
