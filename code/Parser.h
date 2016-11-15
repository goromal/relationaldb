#ifndef _PARSER_
#define _PARSER_
#include "Lexer.h"
#include "datalogProgram.h"

class Parser {
public:
  Parser(vector<TokenInfo> _TIV);
  void process_input();
  void print_results();
  ~Parser();
private:
  datalogProgram Program;
  vector<TokenInfo> TokenInfo_vector;
  TokenConverter _TC;
  size_t current_position = 0;
  bool processed_valid_language = false;
  TokenType get_top();
  vector<string> param_buffer;
  vector<parameter> mixed_param_buffer;
  vector<predicate> pred_buffer;
  vector<TokenType> token_buffer;
  // The recursive tree nodes:
  bool _datalogProgram();
  bool _datalogProgram2();
  bool _datalogProgram3();
  bool _datalogProgram4();
  bool _scheme();
  bool _scheme2(predicate scheme_obj);
  bool _schemeList();
  bool _idList();
  bool _fact();
  bool _fact2(predicate fact_obj);
  bool _factList();
  bool _rule();
  bool _rule2();
  bool _ruleList();
  bool _headPredicate();
  bool _headPredicate2(predicate Pred);
  bool _predicate();
  bool _predicate2(string pred_name);
  bool _predicateList();
  bool _parameter();
  bool _parameterList();
  bool _operator();
  bool _query();
  bool _queryList();
  bool _stringList();
};

#endif
