#ifndef _FSA
#define _FSA
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <ctype.h>
#include <stdio.h>
#include <algorithm>
#include <memory>

using std::string;
using std::vector;
using std::cout; using std::endl;
using std::stringstream;
using std::sort;
using std::shared_ptr;
using std::map;

enum TokenType {COMMA,PERIOD,Q_MARK,LEFT_PAREN,RIGHT_PAREN,COLON,COLON_DASH,MULTIPLY,ADD,SCHEMES,FACTS,RULES,
QUERIES,ID,STRING,COMMENT,WHITESPACE,UNDEFINED,MY_EOF,NEWLINE};

class FiniteStateAutomoton {
public:
	FiniteStateAutomoton() {}
	bool processInput(string input) {
		resetState();
		string_input = input;
		iter = string_input.begin();
		return state0();
	}
	TokenType token;
	int internal_newlines = 0;
	int numChars = 0;
	virtual ~FiniteStateAutomoton() {}
protected:
	virtual bool state0() { return false; }
	string::const_iterator iter;
	string string_input;
	char current_char;
	bool getChar() {
		if (iter != string_input.end()) {
			current_char = *iter;
			++iter;
			return true;
		}
		else return false;
	}
	void resetState() {
		internal_newlines = 0;
		numChars = 0;
	}
};

//========THE MACHINES========

class ADD_FSA : public FiniteStateAutomoton {
public:
	ADD_FSA() : FiniteStateAutomoton() {
		token = ADD;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '+') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class BLOCK_COMMENT_FSA : public FiniteStateAutomoton {
public:
	BLOCK_COMMENT_FSA() : FiniteStateAutomoton() {
		token = COMMENT;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '#') {
			numChars++;
			return state1();
		}
		else return state5();
	}
	bool state1() {
		if (getChar() && current_char == '|') {
			numChars++;
			return state2();
		}
		else return state5();
	}
	bool state2() {
		if (getChar()) {
			numChars++; // <- since it's the intermediate state
			if (current_char == '|') {
				return state3();
			}
			else if (current_char == '\n') {
				internal_newlines++;
				return state2();
			}
			else {
				return state2();
			}
		}
		else return state5();
	}
	bool state3() {
		if (getChar() && current_char == '#') {
			numChars++;
			return state4();
		}
		else return state5();
	}
	bool state4() {
		return true;
	}
	bool state5() {
		return false;
	}
};

class COLON_DASH_FSA : public FiniteStateAutomoton {
public:
	COLON_DASH_FSA() : FiniteStateAutomoton() {
		token = COLON_DASH;
	}
protected:
	bool state0() {
		if (getChar() && current_char == ':') {
			numChars++;
			return state1();
		}
		else return state3();
	}
	bool state1() {
		if (getChar() && current_char == '-') {
			numChars++;
			return state2();
		}
		else return state3();
	}
	bool state2() {
		return true;
	}
	bool state3() {
		return false;
	}
};

class COLON_FSA : public FiniteStateAutomoton {
public:
	COLON_FSA() : FiniteStateAutomoton() {
		token = COLON;
	}
protected:
	bool state0() {
		if (getChar() && current_char == ':') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class COMMA_FSA : public FiniteStateAutomoton {
public:
	COMMA_FSA() : FiniteStateAutomoton() {
		token = COMMA;
	}
protected:
	bool state0() {
		if (getChar() && current_char == ',') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class FACT_FSA : public FiniteStateAutomoton {
public:
	FACT_FSA() : FiniteStateAutomoton() {
		token = FACTS;
	}
protected:
	bool state0() {
		if (getChar() && current_char == 'F') {
			numChars++;
			return state1();
		}
		else return state6();
	}
	bool state1() {
		if (getChar() && current_char == 'a') {
			numChars++;
			return state2();
		}
		else return state6();
	}
	bool state2() {
		if (getChar() && current_char == 'c') {
			numChars++;
			return state3();
		}
		else return state6();
	}
	bool state3() {
		if (getChar() && current_char == 't') {
			numChars++;
			return state4();
		}
		else return state6();
	}
	bool state4() {
		if (getChar() && current_char == 's') {
			numChars++;
			return state5();
		}
		else return state6();
	}
	bool state5() {
		return true;
	}
	bool state6() {
		return false;
	}
};

class ID_FSA : public FiniteStateAutomoton {
public:
	ID_FSA() : FiniteStateAutomoton() {
		token = ID;
	}
protected:
	bool state0() {
		if (getChar() && isalpha(current_char)) {
			numChars++;
			return state1();
		}
		else return state3();
	}
	bool state1() {
		if (getChar()) {
			if (isalpha(current_char) || isalnum(current_char)) {
				numChars++;
				return state1();
			}
			else {
				return state2();
			}
		}
		else {
			return true;
		}
	}
	bool state2() {
		return true;
	}
	bool state3() {
		return false;
	}
};

class LEFT_PAREN_FSA : public FiniteStateAutomoton {
public:
	LEFT_PAREN_FSA() : FiniteStateAutomoton() {
		token = LEFT_PAREN;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '(') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class LINE_COMMENT_FSA : public FiniteStateAutomoton {
public:
	LINE_COMMENT_FSA() : FiniteStateAutomoton() {
		token = COMMENT;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '#') {
			numChars++;
			return state1();
		}
		else return state3();
	}
	bool state1() {
		if (getChar()) {
			if (current_char == '|' || current_char == '\n') {
				return true; // <<<<
			}
			else {
				numChars++;
				return state2();
			}
		}
		else return true;
	}
	bool state2() {
		if (getChar()) {
			if (current_char == '\n') {
				return true;
			}
			else {
				numChars++;
				return state2();
			}
		}
		else return true;
	}
	bool state3() {
		return false;
	}
};

class MULTIPLY_FSA : public FiniteStateAutomoton {
public:
	MULTIPLY_FSA() : FiniteStateAutomoton() {
		token = MULTIPLY;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '*') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class NEWLINE_FSA : public FiniteStateAutomoton {
public:
	NEWLINE_FSA() : FiniteStateAutomoton() {
		token = NEWLINE;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '\n') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class NO_BLOCK_COMMENT_FSA : public FiniteStateAutomoton {
public:
	NO_BLOCK_COMMENT_FSA() : FiniteStateAutomoton() {
		token = UNDEFINED;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '#') {
			numChars++;
			return state1();
		}
		else return state5();
	}
	bool state1() {
		if (getChar() && current_char == '|') {
			numChars++;
			return state2();
		}
		else return state5();
	}
	bool state2() {
		if (getChar()) {
			numChars++; // <- since it's the intermediate state
			if (current_char == '|') {
				return state3();
			}
			else if (current_char == '\n') {
				internal_newlines++;
				return state2();
			}
			else {
				return state2();
			}
		}
		else return state5();
	}
	bool state3() {
		if (getChar() && current_char == '#') {
			numChars++;
			return state4();
		}
		else return state5();
	}
	bool state4() {
		return false;
	}
	bool state5() {
		return true;
	}
};

class NO_STRING_FSA : public FiniteStateAutomoton {
public:
	NO_STRING_FSA() : FiniteStateAutomoton() {
		token = UNDEFINED;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '\'') {
			numChars++;
			return state1();
		}
		else return state3();
	}
	bool state1() {
		if (getChar()) {
			if (current_char == '\'') {
				numChars++;
				return state2();
			}
			else if (current_char == '\n') {
				internal_newlines++;
				numChars++;
				return state1();
			}
			else {
				numChars++;
				return state1();
			}
		}
		else return state3();
	}
	bool state2() {
		if (getChar() && current_char == '\'') {
			numChars++;
			return state1();
		}
		else return false;
	}
	bool state3() {
		return true;
	}
};

class PERIOD_FSA : public FiniteStateAutomoton {
public:
	PERIOD_FSA() : FiniteStateAutomoton() {
		token = PERIOD;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '.') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class Q_MARK_FSA : public FiniteStateAutomoton {
public:
	Q_MARK_FSA() : FiniteStateAutomoton() {
		token = Q_MARK;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '?') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class QUERIES_FSA : public FiniteStateAutomoton {
public:
	QUERIES_FSA() : FiniteStateAutomoton() {
		token = QUERIES;
	}
protected:
	bool state0() {
		if (getChar() && current_char == 'Q') {
			numChars++;
			return state1();
		}
		else return state8();
	}
	bool state1() {
		if (getChar() && current_char == 'u') {
			numChars++;
			return state2();
		}
		else return state8();
	}
	bool state2() {
		if (getChar() && current_char == 'e') {
			numChars++;
			return state3();
		}
		else return state8();
	}
	bool state3() {
		if (getChar() && current_char == 'r') {
			numChars++;
			return state4();
		}
		else return state8();
	}
	bool state4() {
		if (getChar() && current_char == 'i') {
			numChars++;
			return state5();
		}
		else return state8();
	}
	bool state5() {
		if (getChar() && current_char == 'e') {
			numChars++;
			return state6();
		}
		else return state8();
	}
	bool state6() {
		if (getChar() && current_char == 's') {
			numChars++;
			return state7();
		}
		else return state8();
	}
	bool state7() {
		return true;
	}
	bool state8() {
		return false;
	}
};

class RIGHT_PAREN_FSA : public FiniteStateAutomoton {
public:
	RIGHT_PAREN_FSA() : FiniteStateAutomoton() {
		token = RIGHT_PAREN;
	}
protected:
	bool state0() {
		if (getChar() && current_char == ')') {
			numChars++;
			return state1();
		}
		else return state2();
	}
	bool state1() {
		return true;
	}
	bool state2() {
		return false;
	}
};

class RULE_FSA : public FiniteStateAutomoton {
public:
	RULE_FSA() : FiniteStateAutomoton() {
		token = RULES;
	}
protected:
	bool state0() {
		if (getChar() && current_char == 'R') {
			numChars++;
			return state1();
		}
		else return state6();
	}
	bool state1() {
		if (getChar() && current_char == 'u') {
			numChars++;
			return state2();
		}
		else return state6();
	}
	bool state2() {
		if (getChar() && current_char == 'l') {
			numChars++;
			return state3();
		}
		else return state6();
	}
	bool state3() {
		if (getChar() && current_char == 'e') {
			numChars++;
			return state4();
		}
		else return state6();
	}
	bool state4() {
		if (getChar() && current_char == 's') {
			numChars++;
			return state5();
		}
		else return state6();
	}
	bool state5() {
		return true;
	}
	bool state6() {
		return false;
	}
};

class SCHEME_FSA : public FiniteStateAutomoton {
public:
	SCHEME_FSA() : FiniteStateAutomoton() {
		token = SCHEMES;
	}
protected:
	bool state0() {
		if (getChar() && current_char == 'S') {
			numChars++;
			return state1();
		}
		else return state8();
	}
	bool state1() {
		if (getChar() && current_char == 'c') {
			numChars++;
			return state2();
		}
		else return state8();
	}
	bool state2() {
		if (getChar() && current_char == 'h') {
			numChars++;
			return state3();
		}
		else return state8();
	}
	bool state3() {
		if (getChar() && current_char == 'e') {
			numChars++;
			return state4();
		}
		else return state8();
	}
	bool state4() {
		if (getChar() && current_char == 'm') {
			numChars++;
			return state5();
		}
		else return state8();
	}
	bool state5() {
		if (getChar() && current_char == 'e') {
			numChars++;
			return state6();
		}
		else return state8();
	}
	bool state6() {
		if (getChar() && current_char == 's') {
			numChars++;
			return state7();
		}
		else return state8();
	}
	bool state7() {
		return true;
	}
	bool state8() {
		return false;
	}
};

class STRING_FSA : public FiniteStateAutomoton {
public:
	STRING_FSA() : FiniteStateAutomoton() {
		token = STRING;
	}
protected:
	bool state0() {
		if (getChar() && current_char == '\'') {
			numChars++;
			return state1();
		}
		else return state3();
	}
	bool state1() {
		if (getChar()) {
			if (current_char == '\'') {
				numChars++;
				return state2();
			}
			else if (current_char == '\n') {
				internal_newlines++;
				numChars++;
				return state1();
			}
			else {
				numChars++;
				return state1();
			}
		}
		else {
			return state3();
		}
	}
	bool state2() {
		if (getChar() && current_char == '\'') {
			numChars++;
			return state1();
		}
		else return true;
	}
	bool state3() {
		return false;
	}
};

class WHITESPACE_FSA : public FiniteStateAutomoton {
public:
	WHITESPACE_FSA() : FiniteStateAutomoton() {
		token = WHITESPACE;
	}
protected:
	bool state0() {
		if (getChar() && isspace(current_char) && current_char != '\n') {
			numChars++;
			return state1();
		}
		else {
			return state2();
		}
	}
	bool state1() {
		if (getChar()) {
			if (!isspace(current_char)) {
				return true;
			}
			else if (current_char == '\n') {
				numChars++;
				internal_newlines++;
				return state1();
			}
			else { // is space and not a newline
				numChars++;
				return state1();
			}
		}
		else return true;
	}
	bool state2() {
		return false;
	}
};

#endif
