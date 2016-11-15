#include "Parser.h"

Parser::Parser(vector<TokenInfo> _TIV) {
  TokenInfo_vector = _TIV;
}

TokenType Parser::get_top() {
  // Ignore comments
  while (TokenInfo_vector[current_position].token == COMMENT &&
  current_position < TokenInfo_vector.size() - 1) {
      current_position++;
  }
  // Return first element if not past end
  if (current_position < TokenInfo_vector.size()) {
    return TokenInfo_vector[current_position].token;
  } else return UNDEFINED;
}

void Parser::process_input() {
  processed_valid_language = _datalogProgram();
  if (processed_valid_language) {
    //Program.compile_Domain();
    Program.evaluate_Rules();
    Program.answer_Queries();
  }
}

void Parser::print_results() {
  if (processed_valid_language) {
    cout << Program.to_String();
  }
  else {
    cout << "Failure!\n  " <<
    "(" << _TC.convert(TokenInfo_vector[current_position].token) << ",\"" <<
    TokenInfo_vector[current_position].value << "\"," <<
    TokenInfo_vector[current_position].lineNumber << ")\n";
  }
}

bool Parser::_datalogProgram() { // Scheme handler
  if (get_top() == SCHEMES) current_position++;
  else return false;
  if (get_top() == COLON) current_position++;
  else return false;
  if (!_scheme()) return false;
  if (!_schemeList()) return false;
  return _datalogProgram2();
}

bool Parser::_datalogProgram2() { // Facts handler
  if (get_top() == FACTS) current_position++;
  else return false;
  if (get_top() == COLON) current_position++;
  else return false;
  if (!_factList()) return false;
  return _datalogProgram3();
}

bool Parser::_datalogProgram3() { // Rules handler
  if (get_top() == RULES) current_position++;
  else return false;
  if (get_top() == COLON) current_position++;
  else return false;
  if (!_ruleList()) return false;
  return _datalogProgram4();
}

bool Parser::_datalogProgram4() { // Queries handler
  if (get_top() == QUERIES) current_position++;
  else return false;
  if (get_top() == COLON) current_position++;
  else return false;
  if (!_query()) return false;
  if (!_queryList()) return false;
  if (get_top() == MY_EOF) return true;
  else return false;
}

bool Parser::_scheme() {
  string scheme_name;
  predicate newScheme;

  if (get_top() == ID) {
    scheme_name = TokenInfo_vector[current_position].value;
    current_position++;
  }
  else return false;
  if (get_top() == LEFT_PAREN) current_position++;
  else return false;
  if (get_top() == ID) {
    /*
    auto p = shared_ptr<parameter>(new parameter(ID,
      TokenInfo_vector[current_position].value));
      */
    parameter p(ID, TokenInfo_vector[current_position].value);
    newScheme = predicate(scheme_name, p);
    current_position++;
  }
  else return false;
  return _scheme2(newScheme);
}

bool Parser::_scheme2(predicate scheme_obj) {
  if (!_idList()) return false;
  for (size_t i = 0; i < param_buffer.size(); i++) {
    /*
    auto p = shared_ptr<parameter>(new parameter(ID,
      param_buffer[i]));
      */
    parameter p(ID, param_buffer[i]);
    scheme_obj.add_Param(p);
  }
  param_buffer.clear();
  if (get_top() == RIGHT_PAREN) {
    current_position++;
    Program.add_Scheme(scheme_obj);
    return true;
  }
  else return false;
}

bool Parser::_schemeList() {
  if (get_top() == FACTS) {
    return true;
  }
  if (!_scheme()) return false;
  if (!_schemeList()) return false;
  else return true;
}

bool Parser::_idList() {
  if (get_top() == RIGHT_PAREN) {
    return true;
  }
  if (get_top() == COMMA) current_position++;
  else return false;
  if (get_top() == ID) {
    param_buffer.push_back(TokenInfo_vector[current_position].value);
    current_position++;
  }
  else return false;
  if (!_idList()) return false;
  else return true;
}

bool Parser::_fact() {
  string fact_name;
  predicate newFact;
  if (get_top() == ID) {
    fact_name = TokenInfo_vector[current_position].value;
    current_position++;
  }
  else return false;
  if (get_top() == LEFT_PAREN) current_position++;
  else return false;
  if (get_top() == STRING) {
    /*
    auto p = shared_ptr<parameter>(new parameter(STRING,
      TokenInfo_vector[current_position].value));*/
    parameter p(STRING, TokenInfo_vector[current_position].value);
    newFact = predicate(fact_name, p);
    current_position++;
  }
  else return false;
  return _fact2(newFact);
}

bool Parser::_fact2(predicate fact_obj) {
  if (!_stringList()) return false;
  for (size_t i = 0; i < param_buffer.size(); i++) {
    /*auto p = shared_ptr<parameter>(new parameter(STRING,
      param_buffer[i]));*/
    parameter p(STRING, param_buffer[i]);
    fact_obj.add_Param(p);
  }
  param_buffer.clear();
  if (get_top() == RIGHT_PAREN) current_position++;
  else return false;
  if (get_top() == PERIOD) {
    current_position++;
    Program.add_Fact(fact_obj);
    return true;
  }
  else return false;
}

bool Parser::_factList() {
  if (get_top() == RULES) return true;
  if (!_fact()) return false;
  if (!_factList()) return false;
  else return true;
}

bool Parser::_rule() {
  if (!_headPredicate()) return false; // has now added new rule
  if (get_top() == COLON_DASH) current_position++;
  else return false;
  if (!_predicate()) return false; // will add predicate to queue
  return _rule2();
}

bool Parser::_rule2() {
  if (!_predicateList()) return false; // will continue to add predicates to queue
  if (get_top() == PERIOD) {
    for (size_t i = 0; i < pred_buffer.size(); i++) {
      Program.Rules[Program.Rules.size()-1].add_Pred(pred_buffer[i]);
    }
    pred_buffer.clear();
	  current_position++;
	  return true;
  }
  else return false;
}

bool Parser::_ruleList() {
  if (get_top() == QUERIES) return true;
  if (!_rule()) return false;
  if (!_ruleList()) return false;
  else return true;
}

bool Parser::_headPredicate() {
  string headPred_name;
  predicate newPred;
  if (get_top() == ID) {
    headPred_name = TokenInfo_vector[current_position].value;
    current_position++;
  }
  else return false;
  if (get_top() == LEFT_PAREN) current_position++;
  else return false;
  if (get_top() == ID) {
    /*auto p = shared_ptr<parameter>(new parameter(ID,
      TokenInfo_vector[current_position].value));*/
    parameter p(ID, TokenInfo_vector[current_position].value);
    newPred.name = headPred_name;
    newPred.parameters.push_back(p);
    current_position++;
  }
  else return false;
  return _headPredicate2(newPred);
}

bool Parser::_headPredicate2(predicate Pred) {
  if (!_idList()) return false;
  for (size_t i = 0; i < param_buffer.size(); i++) {
    /*auto p = shared_ptr<parameter>(new parameter(ID,
      param_buffer[i]));*/
    parameter p(ID, param_buffer[i]);
    Pred.add_Param(p);
  }
  param_buffer.clear();
  if (get_top() == RIGHT_PAREN) {
    rule newRule(Pred);
    Program.add_Rule(newRule);
	  current_position++;
	  return true;
  }
  else return false;
}

bool Parser::_predicate() {
  string Pred_name;
  if (get_top() == ID) {
    Pred_name = TokenInfo_vector[current_position].value;
    current_position++;
  }
  else return false;
  if (get_top() == LEFT_PAREN) current_position++;
  else return false;
  if (!_parameter()) return false; // will add a single parameter to mixed buffer
  predicate newPred(Pred_name, mixed_param_buffer[0]);
  pred_buffer.push_back(newPred);
  mixed_param_buffer.clear();
  return _predicate2(Pred_name);
}

bool Parser::_predicate2(string pred_name) {
  if (!_parameterList()) return false; // will add more params to mixed buffer
  if (get_top() == RIGHT_PAREN) {
    //ADD TO PRED BUFFER
    for (size_t i = 0; i < mixed_param_buffer.size(); i++) {
      pred_buffer[pred_buffer.size()-1].add_Param(mixed_param_buffer[i]);
    }
    mixed_param_buffer.clear();
	  current_position++;
	  return true;
  }
  else return false;
}

bool Parser::_predicateList() {
  if (get_top() == PERIOD) return true;
  if (get_top() == COMMA) current_position++;
  else return false;
  if (!_predicate()) return false;
  if (!_predicateList()) return false;
  else return true;
}

bool Parser::_parameter() { // THREE OPTIONS
  /*if (get_top() == LEFT_PAREN) {
	  if (!_expression()) return false; // will add param ptr to mixed buffer
	  else return true;
  }
  else */if (get_top() == ID) {
    /*auto p = shared_ptr<parameter>(new parameter(ID,
      TokenInfo_vector[current_position].value));*/
    parameter p(ID, TokenInfo_vector[current_position].value);
    mixed_param_buffer.push_back(p);
	  current_position++;
	  return true;
  }
  else if (get_top() == STRING) {
    /*auto p = shared_ptr<parameter>(new parameter(STRING,
      TokenInfo_vector[current_position].value));*/
    parameter p(STRING, TokenInfo_vector[current_position].value);
    mixed_param_buffer.push_back(p);
	  current_position++;
	  return true;
  }
  else return false;
}

bool Parser::_parameterList() {
  if (get_top() == RIGHT_PAREN) return true;
  if (get_top() == COMMA) current_position++;
  else return false;
  if (!_parameter()) return false;
  if (!_parameterList()) return false;
  else return true;
}
/*
bool Parser::_expression() {
  if (get_top() == LEFT_PAREN) current_position++;
  else return false;
  if (!_parameter()) return false;
  if (!_operator()) return false; // will add operator to single buffer
  return _expression2();
}

bool Parser::_expression2() {
  if (!_parameter()) return false;
  if (get_top() == RIGHT_PAREN) {
    // USE LAST TWO PARAMETERS IN MIXED BUFFER TO MAKE AN EXPRESSION, ADD TO MIXED BUFFER
    auto _p1 = mixed_param_buffer[mixed_param_buffer.size()-2];
    auto _p2 = mixed_param_buffer[mixed_param_buffer.size()-1];
    auto p = shared_ptr<parameter>(new expression(_p1,token_buffer[token_buffer.size()-1],_p2));
    token_buffer.pop_back();
    //delete last two items
    mixed_param_buffer.pop_back();
    mixed_param_buffer.pop_back();

    mixed_param_buffer.push_back(p);
	  current_position++;
	  return true;
  }
  else return false;
}
*/
bool Parser::_operator() {
  if (get_top() == MULTIPLY) {
    token_buffer.push_back(MULTIPLY);
	  current_position++;
	  return true;
  }
  else if (get_top() == ADD) {
    token_buffer.push_back(ADD);
	  current_position++;
	  return true;
  }
  else return false;
}

bool Parser::_query() {
  if (!_predicate()) return false;
  Program.add_Query(pred_buffer[0]);
  pred_buffer.clear();
  if (get_top() == Q_MARK) {
	current_position++;
	return true;
  }
  else return false;
}

bool Parser::_queryList() {
  if (get_top() == MY_EOF) return true;
  if (!_query()) return false;
  if (!_queryList()) return false;
  else return true;
}

bool Parser::_stringList() {
  if (get_top() == RIGHT_PAREN) return true;
  if (get_top() == COMMA) current_position++;
  else return false;
  if (get_top() == STRING) {
    param_buffer.push_back(TokenInfo_vector[current_position].value);
    current_position++;
  }
  else return false;
  if (!_stringList()) return false;
  else return true;
}

Parser::~Parser() {}
