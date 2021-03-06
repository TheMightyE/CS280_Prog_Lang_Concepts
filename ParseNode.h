/*
 * ParseNode.h
 *
 *  Created on: Apr 2, 2017
 *      Author: gerardryan, Syed Khalid
 */

#ifndef PARSENODE_H_
#define PARSENODE_H_

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cmath>
#include <array>
#include <algorithm>

using namespace std;

#include "polylex.h"

extern int globalErrorCount;

// objects in the language have one of these types
enum Type {
	INTEGERVAL,
	FLOATVAL,
	STRINGVAL,
	POLYNOMIAL,
	UNKNOWNVAL,
};

static void runtimeError(string message){
	cout << "RUNTIME ERROR: " << message << endl;
	exit(0);
}

// this class will be used in the future to hold results of evaluations
class Value {
	int	i;
	float f;
	string s;
	Type	t;
	vector<Value> coeffs;
public:
	Value(int i) : i(i), f(0), t(INTEGERVAL) {}
	Value(float f) : i(0), f(f), t(FLOATVAL) {}
	Value(string s) : i(0), f(0), s(s), t(STRINGVAL) {}
	Value(vector<Value> coeffs) : i(0), f(0), s(""), coeffs(coeffs), t(POLYNOMIAL) {}
	Value() : t(UNKNOWNVAL) {}


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
	vector<Value> GetPolyValue(){
		return coeffs;
	}

	friend ostream& operator<<(ostream& o, Value& v ){
		if( v.GetType() == INTEGERVAL  ) o << v.GetIntValue();
		if( v.GetType() == FLOATVAL ) o << v.GetFloatValue();
		if( v.GetType() == STRINGVAL) o << v.GetStringValue();
		if( v.GetType() == POLYNOMIAL){
			o << "{ " << v.GetPolyValue()[0];
			for (int i = 1; i < v.GetPolyValue().size(); i++) {
				o << ", " << v.GetPolyValue()[i];
			}
			o << " }";
		}
		if( v.GetType() == UNKNOWNVAL  ) o << "operator mismatch for: " << v;
		return o;
	}

	Value operator+(Value& v) {
 		if( t == INTEGERVAL ) {
 			if( v.t == INTEGERVAL ) return Value( i + v.i ); // INT + INT
			else if ( v.t == POLYNOMIAL){ // INT + POLY
				vector<Value> newCoeffs = v.coeffs;
				Value lastCoeff = newCoeffs[newCoeffs.size()-1];
				newCoeffs.pop_back();
				newCoeffs.push_back(Value(i) + lastCoeff);
				return newCoeffs;
			 }
 			else if( v.t == FLOATVAL ) return Value( i + v.f ); // INT + FLOAT

 		} else if( t == FLOATVAL ) {
			if( v.t == INTEGERVAL ) return Value( f + v.i ); // FLOAT + INT
 			else if( v.t == FLOATVAL ) return Value( f + v.f ); // FLOAT + FLOAT
			else if( v.t == POLYNOMIAL){ // FLOAT + POLY
				vector<Value> newCoeffs = v.coeffs;
				Value lastCoeff = newCoeffs[newCoeffs.size()-1];
				newCoeffs.pop_back();
				newCoeffs.push_back(Value(f) + lastCoeff);
				return newCoeffs;
			}

 		} else if( t == STRINGVAL ) {
 			if( v.t == STRINGVAL ) return Value( s + v.s ); // STRING + STRING
 		} else if( t == POLYNOMIAL){
				if( v.t == INTEGERVAL || v.t == FLOATVAL){ // POLY + INT or FLOAT
					Value lastCoeff = coeffs[coeffs.size()-1];
					coeffs.pop_back();
					coeffs.push_back(lastCoeff + v);
					return coeffs;

				}else if( v.t == POLYNOMIAL){ // POLY + POLY
					int lenLeft = coeffs.size();
					int lenRight = v.coeffs.size();
					// Get len of small and large list
					int small = min(lenLeft,lenRight) - 1;
					int large = max(lenLeft,lenRight) - 1;;
					vector<Value> largeCoeffList, smallCoeffList;
					// Identifiy the large and small list
					if(coeffs.size() >= v.coeffs.size()){
						largeCoeffList = coeffs;
						smallCoeffList = v.coeffs;
					}else{
						largeCoeffList = v.coeffs;
						smallCoeffList = coeffs;
					}

					/*
					* Make both vectors the same length
					* by adding zeros to the beginning of the samller list
					*/
					for (int i = 0; i < large - small; i++) {
						smallCoeffList.insert(smallCoeffList.begin(), Value(0));
					}
					vector<Value> newCoeffs;
					for (int i = 0; i < large + 1; i++){
							newCoeffs.push_back(smallCoeffList[i] + largeCoeffList[i]);
					}
					return Value(newCoeffs);
				}

	}
		return Value(); // invalid!
}

		Value operator-(Value& v) {
	 		if( t == INTEGERVAL ) {
	 			if( v.t == INTEGERVAL ) return Value( i - v.i ); // INT - INT

	 			else if( v.t == FLOATVAL ) return Value( i - v.f ); // INT - FLOAT

				else if( v.t == POLYNOMIAL){ // INT - POLY
					vector<Value> newCoeffs;
					// Distribute the -1 first
					for (int i = 0; i < v.coeffs.size() - 1; i++) {
						v.coeffs[i] = Value(-1) * v.coeffs[i];
					}
					Value lastCoeff = v.coeffs[v.coeffs.size()-1];
					v.coeffs.pop_back();
					v.coeffs.push_back((Value(i) - lastCoeff));
					return v.coeffs;
				}

	 		} else if( t == FLOATVAL ) {
				if( v.t == INTEGERVAL ) return Value( f - v.i ); // FLOAT - INT

	 			else if( v.t == FLOATVAL ) return Value( f - v.f ); // FLOAT - FLOAT

	 		} else if( t == POLYNOMIAL){
					if( v.t == INTEGERVAL || v.t == FLOATVAL){ // POLY - INT or FLOAT
						Value lastCoeff = coeffs[coeffs.size()-1];
						coeffs.pop_back();
						coeffs.push_back(lastCoeff - v);
						return coeffs;

					}else if( v.t == POLYNOMIAL){ // POLY - POLY
						int lenLeft = coeffs.size();
						int lenRight = v.coeffs.size();
						int small = min(lenLeft,lenRight) - 1;
						int large = max(lenLeft,lenRight) - 1;;
						vector<Value> largeCoeffList, smallCoeffList;
						/*
						* For subtraction, it matters which list is larger,
						* even after zeros are added to the smaller list
					  */
						bool leftLarge = false;
						// Identifiy the large and small list
						if(coeffs.size() >= v.coeffs.size()){
							leftLarge = true;
							largeCoeffList = coeffs;
							smallCoeffList = v.coeffs;
						}else{
							largeCoeffList = v.coeffs;
							smallCoeffList = coeffs;
						}

						/*
						* Make both vectors the same length
						* by adding zeros to the beginning of the samller list
						*/
						for (int i = 0; i < large - small; i++) {
							smallCoeffList.insert(smallCoeffList.begin(), Value(0));
						}

						vector<Value> newCoeffs;
						for (int i = 0; i < large + 1; i++){
							if(leftLarge)
								newCoeffs.push_back(largeCoeffList[i] - smallCoeffList[i]);
							else
								newCoeffs.push_back(smallCoeffList[i] - largeCoeffList[i]);
						}
						return Value(newCoeffs);
					}
				}
	 			return Value(); // invalid!
			}
			// Used to keep track of the coefficient and its power
			struct PolyTerm{
				Value *coeff;
				int power;
			};
			Value operator*(Value& v) {
		 		if( t == INTEGERVAL ) {
		 			if( v.t == INTEGERVAL ) return Value( i * v.i ); // INT * INT
		 			else if( v.t == FLOATVAL ) return Value( i * v.f ); // INT * FLOAT
					else if( v.t == POLYNOMIAL){ // INT * POLY
						vector<Value> newCoeffs;
						for (int k = 0; k < v.coeffs.size(); k++){
							newCoeffs.push_back(Value(i) * v.coeffs[k]);
						}

						return Value(newCoeffs);
					}else if( v.t == STRINGVAL ) { // INT * STRING
						string str = "";
						for (int j = 0; j < i; j++) {
							str += v.GetStringValue();
						}
			 			return Value(str);
			 		}

		 		} else if( t == FLOATVAL ) {
					if( v.t == INTEGERVAL ) return Value( f * v.i ); // FLOAT * INT

		 			else if( v.t == FLOATVAL ) return Value( f * v.f ); // FLOAT * FLOAT
					else if( v.t == POLYNOMIAL){ // FLOAT * POLY
						vector<Value> newCoeffs;
						for (int k = 0; k < v.coeffs.size(); k++){
							newCoeffs.push_back(Value(f) * v.coeffs[k]);
						}

						return Value(newCoeffs);
					}

		 		} else if( t == STRINGVAL ) {
					string str = "";
		 			if( v.t == INTEGERVAL ){ // STRING * INT
						for (int i = 0; i < v.GetIntValue(); i++) {
							str += s;
						}
					}
		 				return Value(str);
		 		} else if( t == POLYNOMIAL){
					vector<Value> newCoeffs;
					if (v.t == INTEGERVAL || v.t == FLOATVAL){ // POLY * INT or FLOAT
						for (int i = 0; i < coeffs.size(); i++) {
							newCoeffs.push_back(coeffs[i] * v);

						}
						return Value(newCoeffs);

					} else if(v.t == POLYNOMIAL){ // POLY * POLY
						// Note to self: largest power is always -> length of two polynomals - 2. Coincidence?
					  int leftPower, rightPower;
					  vector<PolyTerm> polys;
					  Value product;
					  PolyTerm t;

						// Foil th two polynomals
					  for (int i = 0; i < coeffs.size(); ++i){
							// Current power of the left polynomal
					    leftPower = (coeffs.size() - 1) - i;
					    for (int j = 0; j < v.coeffs.size(); ++j) {
								// Current power of the right polynomal
					      rightPower = (v.coeffs.size() - 1) - j;
					      product = Value(coeffs[i] * v.coeffs[j]);
								// Init struct
					      t.coeff = new Value(product);
					      t.power = leftPower + rightPower;
					      polys.push_back(t);
					    }
					  }
						vector<Value> val; // Represents the final list of coefficients after like terms have been added
						vector<int> evaluatedPower; // To keep track of the already added same powers
						Value v;
						bool found; // Flag to see if current term had a like term or not
						// Add like terms
						for (int i = 0; i < polys.size(); i++) {
							v = *polys[i].coeff; //Represents the current coefficient in the loop
							found = false;
							// Check if the current coefficient has already been added
							if(find(evaluatedPower.begin(), evaluatedPower.end(), polys[i].power) ==
								evaluatedPower.end()){
									for (int j = i+1; j < polys.size(); j++) {
										// If two powers are the same, add their coefficients
										if(polys[i].power == polys[j].power){
											found = true;
											v = v + *polys[j].coeff;
										}
									}
									/* If !found, then current coefficient does not have a like term.
											Simply add it to the vector.
									*/
									if(!found){
										val.push_back(*polys[i].coeff);
									}else{
										evaluatedPower.push_back(polys[i].power);
										val.push_back(v);
									}
							}
						}
					  return Value(val);
					}
				}
		 			return Value(); // invalid!
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

/*
// Operator overload for printing ParseNode objects
ostream& operator<<(ostream& o, ParseNode *node){
if(node->left == NULL) o << "NULL";
	else									o << node->left;
	if(node->right == NULL) o << "NULL";
	else										o << node->right;
	return o;
}
*/


// a list of statements is represented by a statement to the left, and a list of statments to the right
class StatementList : public ParseNode {
public:
	StatementList(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}
};

// a SetStatement represents the idea of setting id to the value of the Expr pointed to by the left node
class SetStatement : public ParseNode {
	string id;
	ParseNode *exp;
public:
	SetStatement(string id, ParseNode* exp) : id(id), exp(exp), ParseNode(exp) {}
	Value eval(map<string,Value>& symbolTable){
		symbolTable[id] = exp->eval(symbolTable);
		return Value();
	}
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

		Value sum = leftVal + rightVal;

		if(sum.GetType() == UNKNOWNVAL){
			runtimeError("Type mismatch for PlusOp");
		}

		return sum;
	}

};
// represents subtracting the two child expressions
class MinusOp : public ParseNode {
public:
	MinusOp(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}

	Value eval(map<string,Value>& symbolTable){
		Value leftVal = left->eval(symbolTable);
		Value rightVal = right->eval(symbolTable);

		Value sum = leftVal - rightVal;

		if(sum.GetType() == UNKNOWNVAL){
			runtimeError("Type mismatch for MinusOp");
		}

		return sum;
	}
};

// represents multiplying the two child expressions
class TimesOp : public ParseNode {
public:
	TimesOp(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}

	Value eval(map<string,Value>& symbolTable){
		Value leftVal = left->eval(symbolTable);
		Value rightVal = right->eval(symbolTable);

		Value sum = leftVal * rightVal;

		if(sum.GetType() == UNKNOWNVAL){
			runtimeError("Type mismatch for TimesOp");
		}

		return sum;
	}
};

/*
leaves of the parse tree
notice that the parent constructors take no arguments
that means this is a leaf
*/
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
			cout << "RUNTIME ERROR: Identifier " + id + " used before set" << endl;
			exit(0);
		}

	}
	Value eval(map<string,Value>& symbolTable){
		return symbolTable[id];
	}
};

// a representation of a list of coefficients must be developed
class Coefficients : public ParseNode {
	vector<ParseNode *> v;
public:
	Coefficients(vector<ParseNode *> v): v(v), ParseNode() {}
	Value eval(map<string,Value>& symbolTable){
			vector<Value> coeffs;
			Value coeffVal;
			// Converting the ParseNode* vector to a Value vector
			for (int i = 0; i < v.size(); i++) {
				coeffVal = v[i] -> eval(symbolTable);
				coeffs.push_back(coeffVal);
			}
			return Value(coeffs);
	}
};

class Eval : public ParseNode {
public:
 	Eval(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}
	Value eval(map<string,Value>& symbolTable){
		Value leftVal = left -> eval(symbolTable);
		Value rightVal = right -> eval(symbolTable);

		if(rightVal.GetType() != INTEGERVAL && rightVal.GetType() != FLOATVAL){
			runtimeError("Coefficients can only be evaluated at int and float values.");
		}
		vector<Value> coeffs(leftVal.GetPolyValue());
		if(leftVal.GetType() == POLYNOMIAL){
			// Get the highest power of the polynomal (leftmost coefficient)
			int amount_of_x = coeffs.size() - 1;
			Value ans(0), curr_x, val;

			for (int i = 0; i < coeffs.size() - 1; i++){
				if(rightVal.GetType() == INTEGERVAL)
					val = Value(int(pow(rightVal.GetIntValue(), amount_of_x)));
				else
					val = Value(float(pow(rightVal.GetFloatValue(), amount_of_x)));


				curr_x = Value(coeffs[i] * val);
				ans = ans + curr_x;
				// Decrement the power by one because the next coefficient to the right has one less power
				amount_of_x--;
			}
			// add the last coefficient to the answer
			ans = ans + coeffs[coeffs.size()-1];
			return Value(ans);
		} else{
			runtimeError("Only polynomals can be evaluted");
		}
	}
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
