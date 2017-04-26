#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <map>
#include <algorithm>
#include "polylex.h"

using namespace std;

void mostFreqID(vector<Token>);

bool comment = false;
// int currentLine = 1;
int idArr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
string allTokenTypes[] = {"ID", "ICONST", "FCONST", "STRING", "PRINT", "SET", "PLUS", "MINUS", "STAR", "COMMA", "LBR", "RBR", "LSQ", "RSQ", "LPAREN", "RPAREN", "SC"};
map<int, string> m;




void vArg(Token t){
	//for (int i = 0; i < v.size(); i++){
		//Token t = v[i];
		//cout << t.getType() << "(" << t.getLexeme() << ")" << endl;

		switch(t.getType()){
			case ID:
				cout << "ID" << " (" << t.getLexeme() << ")" << endl;
			break;

			case ICONST:
				cout << "ICONST" << " (" << t.getLexeme() << ")" << endl;
			break;

			case FCONST:
				cout << "FCONST" << " (" << t.getLexeme() << ")" << endl;
			break;

			case STRING:
				cout << "STRING" << " (" << t.getLexeme() << ")" << endl;
			break;

			case PRINT:
				cout << "PRINT" << endl;
			break;

			case SET:
				cout << "SET" << endl;
			break;

			case PLUS:
				cout << "PLUS" << endl;
			break;

			case MINUS:
				cout << "MINUS" << endl;
			break;

			case STAR:
				cout << "STAR" << endl;
			break;

			case COMMA:
				cout << "COMMA" << endl;
			break;

			case LBR:
				cout << "LBR" << endl;
			break;

			case RBR:
				cout << "RBR" << endl;
			break;

			case LSQ:
				cout << "LSQ" << endl;
			break;

			case RSQ:
				cout << "RSQ" << endl;
			break;

			case LPAREN:
				cout << "LPAREN" << endl;
			break;

			case RPAREN:
				cout << "RPAREN" << endl;
			break;

			case SC:
				cout << "SC" << endl;
			break;


		}
	//}

	return;
}

void statsArg(vector<Token> v){
	vector<Token> IDs;
	vector<string> stringIDs;

	for (int i = 0; i < v.size(); i++){
		if(v[i].getType() == ID){
			stringIDs.push_back(v[i].getLexeme());
		}
	}

	cout << "Total IDs: " << stringIDs.size() << endl;

	sort(stringIDs.begin(), stringIDs.end());
	stringIDs.erase( unique( stringIDs.begin(), stringIDs.end() ), stringIDs.end() );

	if(stringIDs.size() > 0){
		// sort(IDs.begin(), IDs.end());
		cout << "List of IDs: ";
		for (int i = 0; i < stringIDs.size(); i++){
			if(i != stringIDs.size() - 1)
				cout << stringIDs[i] << ", ";
			else
				cout << stringIDs[i];

		}
		cout << endl;
	}

return;
}

void sumArg(vector<Token> v){
	cout << "Total Lines: " << currentLine << endl;
	cout << "Total tokens: " << v.size() << endl;
	if(v.size() > 0)
		mostFreqID(v);
	// for(auto v: idArr){
	// 	cout << v << endl;
	// }

return;
}

void mostFreqID(vector<Token> v){
	for(int i = 0; i < 16; i++)
		m[i] = allTokenTypes[i];

	for (int i = 0; i < v.size(); ++i){
		Token t = v[i];

		switch(t.getType()){
			case ID:
				idArr[0] ++;
			break;

			case ICONST:
				idArr[1] ++;
			break;

			case FCONST:
				idArr[2] ++;
			break;

			case STRING:
				idArr[3] ++;
			break;

			case PRINT:
				idArr[4] ++;
			break;

			case SET:
				idArr[5] ++;
			break;

			case PLUS:
				idArr[6] ++;
			break;

			case MINUS:
				idArr[7] ++;
			break;

			case STAR:
				idArr[8] ++;
			break;

			case COMMA:
				idArr[9] ++;
			break;

			case LBR:
				idArr[10] ++;
			break;

			case RBR:
				idArr[11] ++;
			break;

			case LSQ:
				idArr[12] ++;
			break;

			case RSQ:
				idArr[13] ++;
			break;

			case LPAREN:
				idArr[14] ++;
			break;

			case RPAREN:
				idArr[15] ++;
			break;

			case SC:
				idArr[16] ++;
			break;

			default:
				break;
		}

	}

	int max = 0;
	vector<int> indx;
		for (int i = 0; i < 16; i++){
			if(idArr[i] > max){
				max = idArr[i];
				indx.clear();
				indx.push_back(i);
			} else if(idArr[i] == max){
				indx.push_back(i);
			}
		}

		cout << "Most frequently used tokens: ";

		cout << m[indx[0]];
		for(int i = 1; i < indx.size(); i++){
			cout << ", " << m[indx[i]];

		}

		cout << endl;
}

Token getToken(istream& stream){
	char ch;
	string lexeme = "";

	// ch = stream.get();
	// if(stream.eof()){
	// 	currentLine = 1;
	// }else{
	// 	stream.putback(ch);
	// }

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
						if(lexeme == "SET") return Token(SET, lexeme);
						else if(lexeme == "PRINT") return Token(PRINT, lexeme);
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
				lexeme+=ch;
				do{
					ch = stream.get();

					if(ch == '\n'){
						cout << "Error on line " << currentLine << " (" << lexeme << ")"<< endl;
						return Token(ERR, lexeme);
					} else{
						lexeme += ch;
					}

				} while(ch != '"');

				 return Token(STRING, lexeme);

			} else if (ch == '\n'){
				currentLine++;


			} else if(stream.eof()){
				currentLine--;
				Token token(DONE, lexeme);
				return token;

			}

		}


	}
	Token token(DONE, lexeme);
	return token;
}
