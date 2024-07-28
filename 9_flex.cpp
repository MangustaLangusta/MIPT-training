//C++ lecture 9 additional library FlexLexer

/*
HOW TO USE IT (easy start):
	- download source files from github (flex)
	- build (use INSTALL.md for instructions)
	- create rules file with extension .l (see example 9_flex_rules.l)
	- process this file with flex (cmd: flex 9_flex_rules.l)
		file "lex.yy.cc" will be generated
	- compile file "lex.yy.cc" with your project
		(g++ 9_flex.cpp lex.yy.cc)
	- create input file (example: 9_flex_test)
	- pass input file as input to compiled program 
		(./a.out < numtest.inp)

*/

#include <iostream>

#include "flex/src/FlexLexer.h" 

int main() {
	
	FlexLexer* lexer = new yyFlexLexer;
	while(lexer->yylex() != 0) {
		//do nothing, all is in rules
	}
	
	delete lexer;
	return 0;
}