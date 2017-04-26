/*
 * ParseNode.h
 *
 *  Created on: Apr 2, 2017
 *      Author: gerardryan
 */

#ifndef PARSENODE_H_
#define PARSENODE_H_

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

#include "polylex.h"

extern int globalErrorCount;

// objects in the language have one of these types
enum Type {
	INTEGERVAL,
	FLOATVAL,
	STRINGVAL,
	UNKNOWNVAL,
};

// this class will be used in the future to hold results of evaluations
class Value {
	int	i;
	float f;
	string s;
	Type	t;
public:
	Value(int i) : i(i), f(0), t(INTEGERVAL) {}
	Value(float f) : i(0), f(f), t(FLOATVAL) {}
	Value(string s) : i(0), f(0), s(s), t(STRINGVAL) {}
	Value(){}


	Type GetType() { return t; }
	int GetIntValue(){
		return i;
	}
	float GetFloatValue(){
		return f;
	}
	string GetStringValue(){
		return s;
	}

	friend ostream& operator<<(ostream& o, Value& v ){
		if( v.GetType() == INTEGERVAL  ) o << v.GetIntValue();
		if( v.GetType() == FLOATVAL ) o << v.GetFloatValue();
		if(v.GetType() == STRINGVAL) o << v.GetStringValue();
		if( v.GetType() == UNKNOWNVAL  ) o << "ERROR";
		return o;
	}
};

extern map<string,bool> idMap;
extern map<string,Value> symbolTable;


// every node in the parse tree is going to be a subclass of this node
class ParseNode {
public:
	ParseNode	*left;
	ParseNode	*right;
	ParseNode(ParseNode *left = 0, ParseNode *right = 0) : left(left), right(right) {}
	virtual ~ParseNode() {}

	virtual Value eval(map<string,Value>& symbolTable){
		if(left)
			left -> eval(symbolTable);
		if(right)
			right -> eval(symbolTable);

		return Value();
	}

	virtual void RunStaticChecks(map<string,bool>& idMap) {
 		if( left )
  		left->RunStaticChecks(idMap);
 		if( right )
	  	right->RunStaticChecks(idMap);
		}
};

// ostream& operator<<(ostream& o, ParseNode *node){
// 	if(node->left == NULL) o << "NULL";
// 	else									o << node->left;
// 	if(node->right == NULL) o << "NULL";
// 	else										o << node->right;
// 	return o;
// }


// a list of statements is represented by a statement to the left, and a list of statments to the right
class StatementList : public ParseNode {
public:
	StatementList(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}
};

// a SetStatement represents the idea of setting id to the value of the Expr pointed to by the left node
class SetStatement : public ParseNode {
	string id;
public:
	SetStatement(string id, ParseNode* exp) : id(id), ParseNode(exp) {}
	void RunStaticChecks(map<string,bool>& idMap){
	  idMap[id] = true;
	}
};

// a PrintStatement represents the idea of printing the value of the Expr pointed to by the left node
class PrintStatement : public ParseNode {
public:
	PrintStatement(ParseNode* exp) : ParseNode(exp) {}
	Value eval(map<string,Value>& symbolTable){
		Value val = left->eval(symbolTable);
		cout << val << endl;
		return val;
	}
};

// represents adding the two child expressions
class PlusOp : public ParseNode {
public:
	PlusOp(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}

	Value eval(map<string,Value>& symbolTable){
		Value leftVal = left->eval(symbolTable);
		Value rightVal = right->eval(symbolTable);

		Type tLeft = leftVal.GetType();
		Type tRight = rightVal.GetType();

		if(tLeft == INTEGERVAL && tRight == INTEGERVAL){
			return Value(leftVal.GetIntValue() + rightVal.GetIntValue());
		}else if(tLeft == INTEGERVAL && tRight == FLOATVAL){
			return Value(leftVal.GetIntValue() + rightVal.GetFloatValue());
		}else if(tLeft == FLOATVAL && tRight == INTEGERVAL){
			return Value(leftVal.GetFloatValue() + rightVal.GetIntValue());
		}else if(tLeft == FLOATVAL && tRight == FLOATVAL){
			return Value(leftVal.GetFloatValue() + rightVal.GetFloatValue());
		}else if(tLeft == STRINGVAL && tRight == STRINGVAL){
			return Value(leftVal.GetStringValue() + rightVal.GetStringValue());
		}else{
			cout << "RUNTIME ERROR: " << ": type mismatch" << endl;
			return 0;
		}

		return 0;
	}

};
// represents subtracting the two child expressions
class MinusOp : public ParseNode {
public:
	MinusOp(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}

	Value eval(map<string,Value>& symbolTable){
		Value leftVal = left->eval(symbolTable);
		Value rightVal = right->eval(symbolTable);

		Type tLeft = leftVal.GetType();
		Type tRight = rightVal.GetType();

		if(tLeft == INTEGERVAL && tRight == INTEGERVAL){
			return Value(leftVal.GetIntValue() - rightVal.GetIntValue());
		}else if(tLeft == INTEGERVAL && tRight == FLOATVAL){
			return Value(leftVal.GetIntValue() - rightVal.GetFloatValue());
		}else if(tLeft == FLOATVAL && tRight == INTEGERVAL){
			return Value(leftVal.GetFloatValue() - rightVal.GetIntValue());
		}else if(tLeft == FLOATVAL && tRight == FLOATVAL){
			return Value(leftVal.GetFloatValue() - rightVal.GetFloatValue());
		}else{
			cout << "RUNTIME ERROR: " << ": type mismatch" << endl;
			return 0;
		}
	}
};

// represents multiplying the two child expressions
class TimesOp : public ParseNode {
public:
	TimesOp(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}

	Value eval(map<string,Value>& symbolTable){
		Value leftVal = left->eval(symbolTable);
		Value rightVal = right->eval(symbolTable);

		Type tLeft = leftVal.GetType();
		Type tRight = rightVal.GetType();

		if(tLeft == INTEGERVAL && tRight == INTEGERVAL){
			return Value(leftVal.GetIntValue() * rightVal.GetIntValue());
		}else if(tLeft == INTEGERVAL && tRight == FLOATVAL){
			return Value(leftVal.GetIntValue() * rightVal.GetFloatValue());
		}else if(tLeft == FLOATVAL && tRight == INTEGERVAL){
			return Value(leftVal.GetFloatValue() * rightVal.GetIntValue());
		}else if(tLeft == FLOATVAL && tRight == FLOATVAL){
			return Value(leftVal.GetFloatValue() * rightVal.GetFloatValue());
		}else if(tLeft == STRINGVAL && tRight == INTEGERVAL){
			string s = "";
			for (int i = 0; i < rightVal.GetIntValue(); i++) {
				s += leftVal.GetStringValue();
			}
			return Value(s);
		}else{
			cout << "RUNTIME ERROR: " << ": type mismatch" << endl;
			return 0;
		}
	}
};

// a representation of a list of coefficients must be developed
class Coefficients : public ParseNode {
	std::vector<ParseNode *> v;
public:
	Coefficients(std::vector<ParseNode *> v): v(v), ParseNode() {}
};

// leaves of the parse tree
// notice that the parent constructors take no arguments
// that means this is a leaf
class Iconst : public ParseNode {
	int	iValue;
public:
	Iconst(int iValue) : iValue(iValue), ParseNode() {}
	Type GetType() { return INTEGERVAL; }
	Value eval(map<string,Value>& symbolTable){
		return Value(iValue);
	}//jbb24

};

class Fconst : public ParseNode {
	float	fValue;
public:
	Fconst(float fValue) : fValue(fValue), ParseNode() {}
	Type GetType() { return FLOATVAL; }
	Value eval(map<string,Value>& symbolTable){
		return Value(fValue);
	}
};

class Sconst : public ParseNode {
	string	sValue;
public:
	Sconst(string sValue) : sValue(sValue), ParseNode() {}
	Type GetType() { return STRINGVAL; }
	Value eval(map<string,Value>& symbolTable){
		return Value(sValue);
	}
	//operator+
};

class Ident : public ParseNode {
	string	id;
public:
	Ident(string id) : id(id), ParseNode() {}
	Type GetType(){
		for( auto it = symbolTable.begin(); it != symbolTable.end(); ++it ){
      string key = it->first;
			if(key == id){
				Value& value = it->second;
				return value.GetType();
			}
    }

		return UNKNOWNVAL;
	} // not known until run time!

	void RunStaticChecks(map<string,bool>& idMap) {
		if( idMap[id] == false ) {
			cout << "RUNTIME ERROR: Identifier " + id + " used before set";
		}

	}
	Value eval(map<string,Value>& symbolTable){
		return symbolTable[id];
	}
};

class Eval : public ParseNode {
	ParseNode *l;
 	ParseNode *r;
public:
 	Eval(ParseNode *l, ParseNode *r) : l(l), r(r), ParseNode() {}
 };


extern ParseNode *Prog(istream& in);
extern ParseNode *Stmt(istream& in);
extern ParseNode *Expr(istream& in);
extern ParseNode *Term(istream& in);
extern ParseNode *Primary(istream& in);
extern ParseNode *Poly(istream& in);
extern ParseNode *Coeffs(istream& in);
extern ParseNode *Coeff(istream& in);
extern ParseNode *EvalAt(istream& in);


#endif /* PARSENODE_H_ */
