/*
 * ParseNode.cpp
 *
 *  Created on: Apr 2, 2017
 *      Author: gerardryan
 */
#include <map>
#include <vector>
#include "ParseNode.h"

using namespace std;
// We want to use our routine unchanged... BUT we want to have the ability
// to push back a token if we read one too many; this is our "lookahead"
// so we implement a "wrapper" around getToken

static bool pushedBack = false;
static Token pushedToken;
map<string,Value> symbolTable;
map<string,bool> idMap;

// int globalErrorCount = 0;

Token GetToken(istream& in) {
	if( pushedBack ) {
		pushedBack = false;
		return pushedToken;
	}
	Token tok = getToken(in);
	// cout << tok.getLexeme() << endl;
	return tok;
}

void PutBackToken(Token& t) {
	if( pushedBack ) {
		cout << "You can only push back one token!" << endl;
		exit(0);
	}

	pushedBack = true;
	pushedToken = t;
}

// handy function to print out errors
void error(string s) {
	cout << "Line " << currentLine << ": "<< s << endl;
	++globalErrorCount;
}

// Prog := Stmt | Stmt Prog
ParseNode *Prog(istream& in) {
	ParseNode *stmt = Stmt(in);
	if( stmt != 0 )
		return new StatementList(stmt, Prog(in));
	return 0;
}

// Stmt := Set ID Expr SC | PRINT Expr SC
ParseNode *Stmt(istream& in) {
	Token cmd = GetToken(in);

	if( cmd == SET ) {

		Token idTok = GetToken(in);
		if( idTok != ID ) {
			error("PARSE ERROR: Identifier required after set");
			return 0;
		}
		ParseNode *exp = Expr(in);
		if( exp == 0 ) {
			error("PARSE ERROR: expression required after id in set");
			return 0;
		}

		if( GetToken(in) != SC ) {
			error("PARSE ERROR: semicolon required");
			return 0;
		}

		symbolTable[idTok.getLexeme()] = exp->eval(symbolTable);

		return new SetStatement(idTok.getLexeme(), exp);

	}else if( cmd == PRINT ) {
		ParseNode *exp = Expr(in);
		if( exp == 0 ) {
			error("PARSE ERROR: expression required after id in print");
			return 0;
		}

		if( GetToken(in) != SC ) {
			error("PARSE ERROR: semicolon required");
			return 0;
		}

		return new PrintStatement(exp);

	}else{
		PutBackToken(cmd);
		// error("PARSE ERROR: PROGRAM must start with a STATEMENT");
	}
	return 0;
}

bool hasOp = false;
ParseNode *Expr(istream& in) {
	ParseNode *term = Term(in);
	if (term == 0 && hasOp){
		error("PARSE ERROR: Missing term after operator");
	}
	hasOp = false;
	Token tok = GetToken(in);
	if(tok == PLUS || tok == MINUS){
		hasOp = true;
		if(tok == MINUS){
			return new MinusOp(term, Expr(in));
			// return new PlusMinusOp(term, new TimesOp(new Iconst(-1), Expr(in)));
		} else if(tok == PLUS)
			return new PlusOp(term, Expr(in));

	} else{
		PutBackToken(tok);
		return term;
	}

	return 0;
}

bool hasStar = false;
ParseNode *Term(istream& in) {
	ParseNode *term = Primary(in);
	if(term == 0 && hasStar){
		error("PARSE ERROR: primary required after * in term");
		return 0;
	}
	hasStar = false;
	Token tok = GetToken(in);

	if(tok == STAR){
		hasStar = true;

		return new TimesOp(term, Term(in));
	} else{

		PutBackToken(tok);
		return term;
	}

	return 0;
}

// Primary :=  ICONST | FCONST | STRING | ( Expr ) | Poly
ParseNode *Primary(istream& in) {
	// check tokens... or call Poly
	Token tok = GetToken(in);

	if(tok == ICONST){
		return new Iconst(stoi(tok.getLexeme()));
	} else if(tok == FCONST){
		return new Fconst(stof(tok.getLexeme()));
	} else if(tok == STRING){
		return new Sconst(tok.getLexeme());
	} else if(tok == LPAREN){

		ParseNode *exp = Expr(in);
		if( exp == 0 ){
			error("PARSE ERROR: Missing expression in primary");
		}
		tok = GetToken(in);
		if(tok != RPAREN){
			error("PARSE ERROR: Right parenthesis required after expression in primary");
			return 0;
		}
		return exp;

	} else{
		PutBackToken(tok);

		ParseNode *poly = Poly(in);
		if(poly == 0){
			// error("PARSE ERROR: At least one COEF required after LBR in POLY");
		}
		return poly;
	}
	PutBackToken(tok);
	return 0;
}

// Poly := LCURLY Coeffs RCURLY { EvalAt } | ID { EvalAt }
ParseNode *Poly(istream& in) {
	// note EvalAt is optional
	Token tok = GetToken(in);

	if(tok != LBR){
		if(tok == ID){
			ParseNode *eval = EvalAt(in);
			if(eval == 0){
				return new Ident(tok.getLexeme());
			}else{
				return new Eval(new Ident(tok.getLexeme()), eval);
			}

		}
		PutBackToken(tok);
		return 0;

	} else{
		ParseNode *coeffs = Coeffs(in);
		if((tok = GetToken(in)) == RBR){
			ParseNode *eval = EvalAt(in);
			if(eval == 0){
				return coeffs;
			}else{
				return new Eval(coeffs, eval);
			}
		} else{
				error("PARSE ERROR: Missing RBR after coefficients in POLY");
				return 0;
		}
	}

	return 0;
}

// notice we don't need a separate rule for ICONST | FCONST
// this rule checks for a list of length at least one
ParseNode *Coeffs(istream& in){
    vector <ParseNode *> coeffs;
    ParseNode *coeff = Coeff(in);
    if (coeff == 0)
        return 0;
    coeffs.push_back(coeff);

    while(true)
    {
        Token t = GetToken(in);
        if (t != COMMA)
        {
        	PutBackToken(t);
        	break;
        }
        coeff = Coeff(in);
        if (coeff == 0)
        {
            error("Missing coefficient after comma");
            return 0;
        }
        coeffs.push_back(coeff);
    }

	return new Coefficients(coeffs);
}

ParseNode *Coeff(istream& in) {
	bool isNegative = false;
	Token tok = GetToken(in);
	// cout << tok.getLexeme() << endl;
	if(tok.getLexeme() == "-"){
		isNegative = true;
		tok = GetToken(in);
	}
	if(tok == ICONST){
		if(isNegative)
			return new Iconst(stoi(tok.getLexeme()) * -1);
		return new Iconst(stoi(tok.getLexeme()));
	}
	else if(tok == FCONST){
		if(isNegative)
			return new Fconst(stof(tok.getLexeme()) * -1);
		return new Fconst(stof(tok.getLexeme()));
	}
	else{
		error("PARSE ERROR: Only int and float values allowed in coefficients");
		return 0;
	}

}

ParseNode *EvalAt(istream& in) {
	Token tok = GetToken(in);

	if(tok == LSQ){
		ParseNode *exp = Expr(in);
		if(exp == 0){
			error("PARSE ERROR: Expression required after LSQ in EvalAt");
			return 0;
		}
		tok = GetToken(in);
		if(tok != RSQ){
			error("PARSE ERROR: RSQ required after expression in EvalAt");
			return 0;
		}
		/*Value val = exp  -> eval(symbolTable);
		if(val.GetType() != INTEGERVAL && val.GetType() != FLOATVAL){
			cout << "RUNTIME ERROR: Coeffs can only be evaluated at int and float values.";

		}*/
		return exp;

	}

	PutBackToken(tok);
	return 0;
}
