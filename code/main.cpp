#include "Lexer.h"
#include "Parser.h"

int main(int argc, char* argv[]) {
	Lexer LexicalAnalyzer;
	if (argc > 1) {
		LexicalAnalyzer.process_file(argv[1]);
		Parser GrammarParser(LexicalAnalyzer.TokenInfo_vector);
		GrammarParser.process_input();
		GrammarParser.print_results();
	}
	else cout << "Please pass an argument." << endl;
	return 0;
}
