#include "Lexer.h"

Lexer::Lexer() {
	currentLineNumber = 1;

	// Initialize FSAset, order is opposite of precedence
	Machines.push_back(new NO_BLOCK_COMMENT_FSA());
	Machines.push_back(new NO_STRING_FSA());
	Machines.push_back(new WHITESPACE_FSA());
	Machines.push_back(new LINE_COMMENT_FSA());
	Machines.push_back(new BLOCK_COMMENT_FSA());
	Machines.push_back(new STRING_FSA());
	Machines.push_back(new NEWLINE_FSA());
	Machines.push_back(new ID_FSA());
	Machines.push_back(new QUERIES_FSA());
	Machines.push_back(new RULE_FSA());
	Machines.push_back(new FACT_FSA());
	Machines.push_back(new SCHEME_FSA());
	Machines.push_back(new ADD_FSA());
	Machines.push_back(new MULTIPLY_FSA());
	Machines.push_back(new COLON_DASH_FSA());
	Machines.push_back(new COLON_FSA());
	Machines.push_back(new RIGHT_PAREN_FSA());
	Machines.push_back(new LEFT_PAREN_FSA());
	Machines.push_back(new Q_MARK_FSA());
	Machines.push_back(new PERIOD_FSA());
	Machines.push_back(new COMMA_FSA());
}

void Lexer::process_file(string filename) {
	if (import_file(filename)) {
		compile_tokens();
	}
	else cout << "Error importing file." << endl;
}

void Lexer::compile_tokens() {
	string input_string = extract_string();
	while (input_string.size() > 0) {
		ReadInfo SLinfo = get_longest_substr_info(input_string);
		parse_info(input_string, SLinfo);
	}
	add_token_info(MY_EOF, "");
}

void Lexer::parse_info(string & input, ReadInfo info) {
	if (info.numChars == 0) {
		add_token_info(UNDEFINED, input.substr(0, 1));
		input = input.substr(1);
	} else {
		if (info.token == NEWLINE) {
			currentLineNumber++;
		}
		else if (info.token == WHITESPACE) {
			currentLineNumber += info.num_internal_newlines;
		}
		else {
			temp_LineNumber_buffer = info.num_internal_newlines;
			add_token_info(info.token, input.substr(0, info.numChars));
		}
		input = input.substr(info.numChars);
	}
}

bool Lexer::import_file(string filename) {
	infile.open(filename);
	if (infile.is_open()) return true;
	else return false;
}

string Lexer::extract_string() {
	string str((std::istreambuf_iterator<char>(infile)),
		std::istreambuf_iterator<char>());
	return str;
}

ReadInfo Lexer::get_longest_substr_info(string input) {
	ReadInfo longest_info;
	for (size_t i = 0; i < Machines.size(); i++) {
		if (Machines[i]->processInput(input) && Machines[i]->numChars >= longest_info.numChars) {
			longest_info.numChars = Machines[i]->numChars;
			longest_info.token = Machines[i]->token;
			longest_info.num_internal_newlines = Machines[i]->internal_newlines;
		}
	}
	return longest_info;
}

void Lexer::add_token_info(TokenType token, string value) {
	TokenInfo temp(token, value, currentLineNumber);
	currentLineNumber += temp_LineNumber_buffer;
	temp_LineNumber_buffer = 0;
	TokenInfo_vector.push_back(temp);
}

void Lexer::print_tokens() {
	for (size_t i = 0; i < TokenInfo_vector.size(); i++) {
		cout << "(" << _TC.convert(TokenInfo_vector[i].token) << ",\"" << TokenInfo_vector[i].value <<
			"\"," << TokenInfo_vector[i].lineNumber << ")\n";
	}
	cout << "Total Tokens = " << TokenInfo_vector.size() << endl;
}

void Lexer::reset_data() {
	currentLineNumber = 1;
	TokenInfo_vector.clear();
	infile.close();
}

Lexer::~Lexer() {
	for (size_t i = 0; i < Machines.size(); i++) {
		delete Machines[i];
	}
	Machines.clear();
	reset_data();
}
