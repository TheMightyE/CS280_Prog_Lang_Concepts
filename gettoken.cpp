/*
 * gettoken.cpp
 *
 *  Created on: Apr 2, 2017
 *      Author: Syed Khalid
 */
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "polylex.h"

using namespace std;

bool comment = false;
int currentLine = 1;

Token getToken(istream& stream){
	char ch;
	string lexeme = "";

	while(true){
		ch = stream.get();

		if(ch == '#'){
			currentLine++;
			stream.ignore(256, '\n');

		} else {
			if(ch == '+'){
				lexeme = ch;
				return Token(PLUS, lexeme);

			} else if(ch == '-'){
				lexeme = ch;
				return Token(MINUS, lexeme);

			} else if(ch == '*'){
				lexeme = ch;
				return Token(STAR, lexeme);

			} else if(ch == ','){
				lexeme = ch;
				return Token(COMMA, lexeme);

			} else if(ch == '('){
				lexeme = ch;
				return Token(LPAREN, lexeme);

			} else if(ch == ')'){
				lexeme = ch;
				return Token(RPAREN, lexeme);

			} else if(ch == '['){
				lexeme = ch;
				return Token(LSQ, lexeme);

			} else if(ch == ']'){
				lexeme = ch;
				return Token(RSQ, lexeme);

			} else if(ch == '{'){
				lexeme = ch;
				return Token(LBR, lexeme);

			} else if(ch == '}'){
				lexeme = ch;
				return Token(RBR, lexeme);

			} else if(ch == ';'){
				lexeme = ch;
				return Token(SC, lexeme);

			}

			// Get ID
			else if(isalpha(ch)){
				lexeme += ch;
				char next;
				while(true){
					next = stream.peek();
					if(isalnum(next)){
						lexeme += next;
						ch = stream.get();

					}
					// Check if lexeme is SET or PRINT
					else{
						if(lexeme == "set") return Token(SET, lexeme);
						else if(lexeme == "print") return Token(PRINT, lexeme);
						else return Token(ID, lexeme);
					}
				}

			// Get ICONST or FCONST
			} else if(isdigit(ch)){
				lexeme += ch;
				char next;
				bool isFCONST = false;
				while(true){
					next = stream.peek();
					if(isdigit(next)){
						lexeme += next;
						ch = stream.get();

						// If float
					} else if(next == '.'){
						lexeme += next;
						ch = stream.get();
						isFCONST = true;

					} else {

						if(isFCONST){
							return Token(FCONST, lexeme);
						} else{
							return Token(ICONST, lexeme);
						}

					}
				}

			// FLOAT starting with '.'
			} else if(ch == '.'){
				lexeme += ch;
				cout << "Error on line " << currentLine << " (" << lexeme << ")"<< endl;
				return Token(ERR, lexeme);

			} else if(ch == '"'){
				// lexeme+=ch;
				do{
					ch = stream.get();

					if(ch == '\n'){
						cout << "Error on line " << currentLine << " (" << lexeme << ")"<< endl;
						return Token(ERR, lexeme);
					} else{
						if(ch != '"')
							lexeme += ch;
					}

				} while(ch != '"');

				 return Token(STRING, lexeme);

			} else if (ch == '\n'){
				currentLine++;


			} else if(stream.eof()){
				// currentLine--;
				Token token(DONE, lexeme);
				return token;

			}
		}
	}
}
