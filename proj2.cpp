/*
 * prog2.cpp
 *
 *      Author: gerardryan
 */

#include <string>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;

#include "polylex.h"

int currentLine = 0;

map<TokenTypes,string> tokenNames = {
		{ ID, "ID" },
		{ ICONST, "ICONST" },
		{ FCONST, "FCONST" },
		{ STRING, "STRING" },
		{ PRINT, "PRINT" },
		{ SET, "SET" },
		{ PLUS, "PLUS" },
		{ MINUS, "MINUS" },
		{ STAR, "STAR" },
		{ COMMA, "COMMA" },
		{ LBR, "LBR" },
		{ RBR, "RBR" },
		{ LSQ, "LSQ" },
		{ RSQ, "RSQ" },
		{ LPAREN, "LPAREN" },
		{ RPAREN, "RPAREN" },
		{ SC, "SC" },
		{ ERR, "ERR" },
		{ DONE, "DONE" }
};

map<TokenTypes,bool> tokenExtra = {
		{ ID, true },
		{ ICONST, true },
		{ FCONST, true },
		{ STRING, true },
};

std::ostream& operator<<(std::ostream& out, const Token& tok) {
	out << tokenNames[ tok.getType() ];
	if( tokenExtra[ tok.getType() ] )
		out << " (" << tok.getLexeme() << ")";
	return out;
}

int
main(int argc, char *argv[])
{
	ifstream file;
	bool use_stdin = true;

	bool verbose = false;
	bool stats = false;
	bool sum = false;

	for( int i=1; i<argc; i++ ) {
		string arg = argv[i];

		if( arg == "-v" )
			verbose = true;
		else if( arg == "-stats" )
			stats = true;
		else if( arg == "-sum" )
			sum = true;
		else if( arg[0] == '-' ) {
			cout << "Invalid argument " << arg << endl;
			return 1;
		}
		else {
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
	}

	istream& in = use_stdin ? cin : file;

	Token tok;
	int allTokens = 0;
	map<TokenTypes,int> tokenCounts;
	map<string,bool> identSeen;

	while( (tok = getToken(in)) != DONE && tok != ERR ) {
		allTokens++;
		tokenCounts[ tok.getType() ]++;

		if( tok == ID ) {
			identSeen[ tok.getLexeme() ] = true;
		}
		if( verbose )
			cout << tok << endl;
	}

	if( tok == ERR ) {
		cout << "Error on line " << currentLine+1 << " (" << tok.getLexeme() << ")" << endl;
		return 1;
	}

	if( stats ) {
		cout << "Total IDs: " << tokenCounts[ID] << endl;
		if( tokenCounts[ID] > 0 ) {
			auto iIt = identSeen.begin();
			cout << "List of IDs: " << iIt->first;
			iIt++;
			for( ; iIt != identSeen.end(); iIt++ )
				cout << ", " << iIt->first;
			cout << endl;
		}
	}

	if( sum ) {
		cout << "Total Lines: " << currentLine << endl;
		cout << "Total tokens: " << allTokens << endl;
		if( allTokens > 0 ) {
			// find largest value in token map
			int maxval = 0;
			for( auto iIt = tokenCounts.begin(); iIt != tokenCounts.end(); iIt++ ) {
				if( iIt->second > maxval )
					maxval = iIt->second;
			}

			// print everything with that value
			bool printedOne = false;
			for( auto iIt = tokenCounts.begin(); iIt != tokenCounts.end(); iIt++ ) {
				if( iIt->second == maxval ) {
					if( printedOne )
						cout << ", ";
					else {
						cout << "Most frequently used tokens: ";
						printedOne = true;
					}
					cout << tokenNames[iIt->first];
				}
			}
			cout << endl;
		}
	}

	return 0;
}
