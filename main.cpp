/*
 * prog3.cpp
 *
 *  Created on: Feb 20, 2017
 *      Author: gerardryan
 */

#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

#include "ParseNode.h"



//int currentLine = 0;
int globalErrorCount = 0;
extern map<string,Value> symbolTable;
extern map<string,bool> idMap;

int main(int argc, char *argv[]) {
	ifstream file;
	bool use_stdin = true;

	for( int i=1; i<argc; i++ ) {
		string arg = argv[i];

		if( use_stdin == false ) {
			cout << "Too many file names" << endl;
			return 1;
		}
		use_stdin = false;

		file.open(arg);
		if( file.is_open() == false ) {
			cout << "Could not open " << arg << endl;
			return 1;
		}
	}

	istream& in = use_stdin ? cin : file;

	ParseNode *program = Prog(in);

	if( program == 0 || globalErrorCount > 0 ) {
		cout << "PARSE ERROR: Program failed!" << endl;
		return 1;
	}
	// need to look at the parse here
	program -> RunStaticChecks(idMap);
	program -> eval(symbolTable);

	return 0;

}
