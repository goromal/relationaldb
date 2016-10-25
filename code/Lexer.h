#ifndef _LEXER
#define _LEXER
#include "FiniteStateAutomoton.h"
//#include <stack>-------------------------------------------------------------
#include <climits>

struct ReadInfo {
	int numChars;
	TokenType token;
	int num_internal_newlines;
	ReadInfo() {
		numChars = 0;
		token = UNDEFINED;
	}
	ReadInfo(int _nC, TokenType _t, int _nin) : numChars(_nC), token(_t), num_internal_newlines(_nin) {}
};

struct TokenInfo {
	TokenType token;
	string value;
	int lineNumber;
	TokenInfo(TokenType _t, string _v, int _l) : token(_t), value(_v), lineNumber(_l) {}
};

class TokenConverter {
public:
	TokenConverter() {
		TokenStringMapper[STRING] = "STRING";
		TokenStringMapper[COMMA] = "COMMA";
		TokenStringMapper[MY_EOF] = "EOF";
		TokenStringMapper[UNDEFINED] = "UNDEFINED";
		TokenStringMapper[PERIOD] = "PERIOD";
		TokenStringMapper[Q_MARK] = "Q_MARK";
		TokenStringMapper[LEFT_PAREN] = "LEFT_PAREN";
		TokenStringMapper[RIGHT_PAREN] = "RIGHT_PAREN";
		TokenStringMapper[COLON] = "COLON";
		TokenStringMapper[COLON_DASH] = "COLON_DASH";
		TokenStringMapper[MULTIPLY] = "MULTIPLY";
		TokenStringMapper[ADD] = "ADD";
		TokenStringMapper[SCHEMES] = "SCHEMES";
		TokenStringMapper[FACTS] = "FACTS";
		TokenStringMapper[RULES] = "RULES";
		TokenStringMapper[COMMENT] = "COMMENT";
		TokenStringMapper[QUERIES] = "QUERIES";
		TokenStringMapper[ID] = "ID";
	}
	string convert(TokenType T) {
		return TokenStringMapper[T];
	}
	~TokenConverter() {}
private:
	std::map<TokenType, string> TokenStringMapper;
};

class Lexer {
public:
	Lexer();
	void process_file(string filename);
	void print_tokens();
	vector<TokenInfo> TokenInfo_vector;
	~Lexer();
private:
	int currentLineNumber;
	int temp_LineNumber_buffer = 0;
	bool import_file(string filename); // true if successful, false otherwise
	void compile_tokens();
	void parse_info(string & input, ReadInfo info);
	string extract_string();
	ReadInfo get_longest_substr_info(string input);
	void add_token_info(TokenType token, string value);
	void reset_data();
	std::ifstream infile;
	vector<FiniteStateAutomoton*> Machines;
	TokenConverter _TC;
};

#endif
