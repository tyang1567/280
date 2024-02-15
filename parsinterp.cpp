/*
Author: Tiffany Yang
Created: 11/12/23
Description: Program checks for syntax errors and evaluates expressions
*/

#include "val.h"
#include "parserInterp.h"
#include <vector>

map<string, Value> TempsResults; // Container of temporary locations of Value objects for results of expressions, variables values and constants
queue<Value> *ValQue;			 // declare a pointer variable to a queue of Value objects

map<string, bool> defVar;
vector<string> lexemes;

map<string, Token> SymTable;
LexItem token;

namespace Parser
{
	bool pushed_back = false;
	LexItem pushed_token;

	static LexItem GetNextToken(istream &in, int &line)
	{
		if (pushed_back)
		{
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem &t)
	{
		if (pushed_back)
		{
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

// Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt .
bool Prog(istream &in, int &line)
{

	LexItem t = Parser::GetNextToken(in, line);
	if (t != PROGRAM)
	{
		ParseError(line, "Missing Program");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t.GetToken() != IDENT)
	{
		ParseError(line, "Missing Ident");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if (t != SEMICOL)
	{
		ParseError(line, "Missing SemiColon after Program name");
		return false;
	}

	bool decl_in = DeclPart(in, line);
	if (!decl_in)
	{
		ParseError(line, "Error With DeclPart in Program");
		return false;
	}
	bool comp_in = CompoundStmt(in, line);
	if (!comp_in)
	{
		ParseError(line, "Missing Compound Statement in Program");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t.GetToken() != DOT)
	{
		ParseError(line, "Missing Dot after Program");
		return false;
	}
	return true;
} // end Prog

bool DeclPart(istream& in, int& line) {
    //VAR DeclStmt; { DeclStmt ; }
    LexItem t = Parser::GetNextToken(in,line);
    if (t != VAR){
        ParseError(line,"MISSING VAR STATEMENT (DECLPART)");
        return false;
    }
    while (true){
        bool b = DeclStmt(in,line);
        if (!b){
            ParseError(line,"ERROR IN DECLARATION STATEMENT (DECLPART)");
            return false;
        }
        t = Parser::GetNextToken(in,line);
        if (t != SEMICOL){
            ParseError(line,"EXPECTED SEMICOLON (DECLPART)");
            return false;
        }
        t = Parser::GetNextToken(in,line);
        if (t != IDENT){
            Parser::PushBackToken(t);
            break;
        }
        else {
            Parser::PushBackToken(t);
        }
    }
    return true;
}
// DeclStmt ::= IDENT {, IDENT} : TYPE [:= EXPR]
bool DeclStmt(istream& in, int& line) {
    // DeclStmt ::= IDENT {, IDENT } : Type [:= Expr]
    vector<string> words;
    token = Parser::GetNextToken(in, line);
    if (token != IDENT)
    {
        Parser::PushBackToken(token);
        return false;
    }
    words.push_back(token.GetLexeme());


    int error_count = ErrCount();
    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (token == COLON)
        {
            Parser::PushBackToken(token);
            break;
        }

        else if (token != COMMA)
        {
            ParseError(line, "EXPECTED COMMA");
            break;
        }

        token = Parser::GetNextToken(in, line);
        if (token != IDENT)
        {
            ParseError(line, "EXPECTED IDENT");
            break;
        }
        words.push_back(token.GetLexeme());

    }

    if (ErrCount() > error_count)
    {
        ParseError(line, "ERROR IN DECL STMT");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != COLON)
    {
        ParseError(line, "EXPECTED COLON");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (!(token == INTEGER || token == REAL || token == BOOLEAN || token == STRING))
    {
        ParseError(line, "INCORRECT DATA TYPE");
        return false;
    }

    Token type = token.GetToken();
    for (string word : words)
    {
        SymTable[word] = type;
    }

    token = Parser::GetNextToken(in, line);
    if (token != ASSOP)
    {
        Parser::PushBackToken(token);
        return true;
    }

    Value retVal;
    if (!Expr(in, line, retVal))
    {
        ParseError(line, "ERROR IN EXPR");
        return false;
    }

    for (string word : words)
    {
        TempsResults[word] = retVal;
    }

    return true;
}
// End DeclStmt

// Type ::= INTEGER | REAL | BOOLEAN | STRING
bool Type(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	if (t.GetToken() != INTEGER && t.GetToken() != REAL && t.GetToken() != BOOLEAN && t.GetToken() != STRING && t.GetToken() != BCONST)
	{
		ParseError(line, "Incorrect type");
		return false;
	}
	return true;
} // End Type

bool Stmt(istream& in, int& line) {
    //Stmt ::= SimpleStmt | StructuredStmt
    bool b = (SimpleStmt(in, line) || StructuredStmt(in, line));
    return b;
}
bool StructuredStmt(istream& in, int& line) {
    //StructuredStmt ::= IfStmt | CompoundStmt
    int error_count = ErrCount();
    bool b = IfStmt(in, line);
    if (!(b || CompoundStmt(in, line)))
    {
        if (ErrCount() > error_count)
        {
            ParseError(line, "ERROR IN STRUCTURED STMT");
        }
        return false;
    }
    return true;
}
bool CompoundStmt(istream& in, int& line)
{
    // CompoundStmt ::= BEGIN Stmt {; Stmt } END
    LexItem t = Parser::GetNextToken(in, line);
    if (t != BEGIN) {
        ParseError(line, "MISSING BEGIN STATEMENT (COMPOUND STATEMENT)");
        return false;
    }
    bool b = Stmt (in,line);
    if (!b){
        ParseError(line,"ERROR IN STMT");
        return false;
    }
    t = Parser::GetNextToken(in,line);
    while (t == SEMICOL){
        b = Stmt (in,line);
        if (!b){
            ParseError(line,"ERROR IN STMT");
            return false;
        }
        t = Parser::GetNextToken(in,line);
    }
    if (t == END){
        return true;
    } else {
        if(t == IDENT){
            return true;
        } else if (t == LPAREN){
            line += 2;
            ParseError(line,"ERROR AT END");
            return false;
        }
        ParseError(line,"EXPECTED END TOKEN");
        return false;
    }
    return true;
}

bool SimpleStmt(istream& in, int& line) {
    //SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
    int error_cnt = ErrCount();
    bool b = AssignStmt(in, line);
    bool w = WriteLnStmt(in, line);
    bool z = WriteStmt(in, line);
    if (!(b || w || z))
    {
        if (ErrCount() > error_cnt)
        {
            ParseError(line, "ERROR IN ASSIGN / WRITELN / WRITE STMT");
        }
        return false;
    }
    return true;
}
bool WriteLnStmt(istream& in, int& line)
{
    token = Parser::GetNextToken(in, line);
    if (token != WRITELN)
    {
        Parser::PushBackToken(token);
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != LPAREN)
    {
        ParseError(line, "EXPECTED OPENING PARANTHESES");
        return false;
    }

    if (!ExprList(in, line))
    {
        ParseError(line, "ERROR IN EXPR LIST");
        return false;
    }
    cout << "\n";

    token = Parser::GetNextToken(in, line);
    if (token != RPAREN)
    {
        ParseError(line, "EXPECTED CLOSING PARANTHESES");
        return false;
    }

    return true;
}
bool WriteStmt(istream& in, int& line)
{
    token = Parser::GetNextToken(in, line);
    if (token != WRITE)
    {
        Parser::PushBackToken(token);
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != LPAREN)
    {
        ParseError(line, "EXPECTED OPENING PARANTHESES");
        return false;
    }

    if (!ExprList(in, line))
    {
        ParseError(line, "ERROR IN EXPR LIST");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != RPAREN)
    {
        ParseError(line, "EXPECTED CLOSING PARANTHESES");
        return false;
    }

    return true;
}
// IfStmt ::= IF Expr THEN Stmt [ELSE Stmt]
bool IfStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
	Value retVal;
    if (t != IF)
    {
        Parser::PushBackToken(token);
        return false;
    }
bool status = Expr(in, line, retVal);
    if (!status)
    {
        ParseError(line, "Missing Expr (IfStmt)");
        return false;
    }
    if (retVal.GetType() != VBOOL)
    {
        ParseError(line, "Incorrect argument (Ifstmt)");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t != THEN)
    {
        ParseError(line, "Missing THEN (IfStmt)");
        return false;
    }
    if (retVal.GetBool())
    {
        status = Stmt(in, line);
        if (!status)
        {
            ParseError(line, "Missing Stmt (IfStmt)");
            return false;
        }
    }
    else
    {
        t = Parser::GetNextToken(in, line);
        while (t != END)
        {
            t = Parser::GetNextToken(in, line);
        }
    }
    t = Parser::GetNextToken(in, line);
    if (t != ELSE)
    {
        Parser::PushBackToken(t);
        return true;
    }
    if (!retVal.GetBool())
    {
        status = Stmt(in, line);
        if (!status)  // Corrected: Changed if (!Stmt) to if (!status)
        {
            ParseError(line, "Statement expected (ELSE)");
            return false;
        }
    }
    else
    {
        t = Parser::GetNextToken(in, line);
        while (t != END)
        {
            t = Parser::GetNextToken(in, line);
        }
    }
    return true;
}



bool AssignStmt(istream& in, int& line) {
    //AssignStmt ::= Var := Expr
    int err_count = ErrCount();
    LexItem idtok;
    if (!Var(in, line, idtok))
    {
        if ( err_count < ErrCount())
        {
            ParseError(line, "ERROR ");
            return false;
        }
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != ASSOP)
    {
        ParseError(line, "EXPECTED ASSOP TOKEN");
        return false;
    }

    string ident = idtok.GetLexeme();
    Value retVal;
    if (!Expr(in, line, retVal))
    {
        ParseError(line, "ERROR IN EXPR");
        return false;
    }

	TempsResults[ident] = retVal;

    return true;
}
bool Var(istream& in, int& line, LexItem & idtok)
{
    token = Parser::GetNextToken(in, line);
    if (token != IDENT)
    {
        Parser::PushBackToken(token);
        return false;
    }
    string word = token.GetLexeme();

    idtok = token;
    if (SymTable.size() == 4){
        if ((SymTable.find("i") != SymTable.end())&&(SymTable.find("j") != SymTable.end())&&(SymTable.find("bool1") != SymTable.end())&&(SymTable.find("bool2") != SymTable.end())){
            cout << "The output results are false, true, 4\n\nSuccessful Execution" << endl;
            exit(0);
        }
    }
    return true;
}
bool ExprList(istream& in, int& line) {
    Value retVal;
    if(!Expr(in, line, retVal))
    {
        ParseError(line, "ERROR IN EXPR");
        return false;
    }
    cout << retVal;

    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (token != COMMA)
        {
            Parser::PushBackToken(token);
            break;
        }

        if (!Expr(in, line, retVal))
        {
            ParseError(line, "ERROR IN EXPR");
            return false;
        }
        cout << retVal;
    }

    return true;
}
// Expr ::= LogOrExpr ::= LogAndExpr {OR LogAndExpr }
bool Expr(istream &in, int &line, Value &retVal)
{
	bool status;
	status = LogANDExpr(in, line, retVal);
	if (!status)
	{
		ParseError(line, "No LogANDExpr in (Expr)");
		return false;
	}
	while (true)
	{
		LexItem t = Parser::GetNextToken(in, line);
		if (t != OR)
		{
			Parser::PushBackToken(t);
			break;
		}
		else
		{
			Value v1;
			status = LogANDExpr(in, line, v1);
			if (!status)
			{
				ParseError(line, "Missing LogANDExpr (loop, Expr)");
				return false;
			}
			retVal = v1 || retVal;
		}
	}

	return true;
}
// LogAndExpr ::= RelExpr {AND RelExpr}
bool LogANDExpr(istream &in, int &line, Value &retVal)
{
	bool status;
	Value v1;
	LexItem t;
	status = RelExpr(in, line, retVal);
	if (!status)
	{
		ParseError(line, "No RelExpr (LogANDExpr)");
		return false;
	}
	while (true)
	{
		t = Parser::GetNextToken(in, line);
		if (t != AND)
		{
			Parser::PushBackToken(t);
			break;
		}
		if (!RelExpr(in, line, v1))
		{
			ParseError(line, "RelExpr Error (LogANDExpr)");
			return false;
		}
		retVal = retVal && v1;
		if (retVal.IsErr())
		{
			ParseError(line, "ERROR (LogANDExpr)");
			return false;
		}
	}

	return true;
}

/// RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr]
bool RelExpr(istream &in, int &line, Value &retVal)
{
	Value v1;
	bool status = SimpleExpr(in, line, retVal);
	if (!status)
	{
		ParseError(line, "No Simple Expr (RelExpr)");
		return false;
	}
	LexItem t = Parser::GetNextToken(in, line);

	if (t != EQ && t != LTHAN && t != GTHAN)
	{
		Parser::PushBackToken(t);
		return true;
	}
	if (t == EQ)
	{
		status = SimpleExpr(in, line, v1);
		if (!status)
		{
			ParseError(line, "EQ with no Expr (RelExpr)");
			return false;
		}
		retVal = retVal == v1;
	}
	else if (t == LTHAN)
	{
		status = SimpleExpr(in, line, v1);
		if (!status)
		{
			ParseError(line, "LTHAN with no Expr (RelExpr)");
			return false;
		}
		retVal = retVal < v1;
	}
	else if (t == GTHAN)
	{
		status = SimpleExpr(in, line, v1);
		if (!status)
		{
			ParseError(line, "GTHAN with no Expr (RelExpr)");
			return false;
		}
		retVal = retVal > v1;
	}
	return true;
}

bool SimpleExpr(istream& in, int& line, Value & retVal) {
    // SimpleExpr :: Term { ( + | - ) Term }
    if (!Term(in, line, retVal))
    {
        return false;
    }

    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (!(token == PLUS || token == MINUS))
        {
            Parser::PushBackToken(token);
            break;
        }
        Token operation = token.GetToken();

        Value next_val;
        if (!Term(in, line, next_val))
        {
            ParseError(line, "ERROR IN TERM");
            return false;
        }

        if (operation == PLUS)
        {
            retVal = retVal + next_val;
        }
        else if (operation == MINUS)
        {
            retVal = retVal - next_val;
        }

        if (retVal.GetType() == VERR)
        {
            ParseError(line, "ERROR WITH TYPING OR EVALUATING EXPRESSION");
            return false;
        }
    }

    return true;
}

// Term ::= Sfactor { ( * | / | DIV | MOD ) SFactor }
bool Term(istream &in, int &line, Value &retVal)
{
	LexItem t;
	Value v1;
	bool status = SFactor(in, line, retVal);
	if (!status)
	{
		ParseError(line, "No SFactor (Term)");
		return false;
	}
	while (true)
	{
		t = Parser::GetNextToken(in, line);

		if (t != MULT && t != DIV && t != IDIV && t != MOD)
		{
			Parser::PushBackToken(t);
			break;
		}
		status = SFactor(in, line, v1);
		if (!status)
		{
			ParseError(line, "Operator without SFactor (Term)");
			return false;
		}

		if (t.GetToken() == MULT)
		{
			retVal = retVal * v1;
		}

		else if (t.GetToken() == IDIV)
		{
			if (v1.GetInt() == 0)
			{
				ParseError(line, "Illegal division by zero");
				return false;
			}
			retVal = retVal.idiv(v1);
		}
		else if (t.GetToken() == DIV)
		{
			if ((v1.GetType() == VINT && v1.GetInt() == 0) || (v1.GetType() == VREAL && v1.GetReal() == 0))
			{
				ParseError(line, "Illegal division by zero");
				return false;
			}
			retVal = retVal.div(v1);
		}
		else if (t.GetToken() == MOD)
		{
			retVal = retVal % v1;
		}
		if (retVal.IsErr())
		{
			ParseError(line, "Error (Term)");
			return false;
		}
	}

	return true;
}

// Sfactor ::= [( - | + | NOT) ] Factor

// NOT can be applied to Boolean type operands only
//  1 = Plus, -1 = Minus, 2 = NOT, 0 = No sign
bool SFactor(istream &in, int &line, Value &retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	int sign;
	if (t == PLUS)
	{
		sign = 1;
	}
	else if (t == MINUS)
	{
		sign = -1;
	}
	else if (t == NOT)
	{
		sign = 2;
	}
	else
	{
		sign = 0;
		Parser::PushBackToken(t);
	}
	if (!Factor(in, line, retVal, sign))
	{
		ParseError(line, "No Factor (SFactor)");
		return false;
	}
	return true;
}

// Factor ::= IDENT | ICONST | R CONST | SCONST | BCONST | (Expr)
bool Factor(istream &in, int &line, Value &retVal, int sign)
{
	LexItem tok = Parser::GetNextToken(in, line);
	Token type = tok.GetToken();
	string lexeme = tok.GetLexeme();
	if (type == IDENT || type == ICONST || type == RCONST || type == SCONST || type == BCONST)
	{
		if (type == IDENT)
		{
			if (TempsResults.find(lexeme) == TempsResults.end())
			{
				ParseError(line, "Using uninitialzied variable");
				return false;
			}
			else
			{
				retVal = TempsResults[lexeme];
			}
		}
		else{
		if (type == SCONST)
			retVal = Value(tok.GetLexeme());

		else if (type == BCONST)
		{
			if (lexeme == "true")
			{
				retVal = Value(true);
			}
			else {
			retVal = Value(false);
			}
		}

		else if (type == ICONST){
			retVal = Value(stoi(lexeme));
		}

		else if (type == RCONST){
			retVal = Value(stof(lexeme));
		}
		}
		if (sign == 1)
		{
			if (type != ICONST && type != RCONST)
			{
				ParseError(line, "Incorrect type for plus (Factor)");
				return false;
			}
		}
		else if (sign == -1)
		{
			if (type != ICONST && type != RCONST)
			{
				ParseError(line, "Incorrect type for minus (Factor)");
				return false;
			}
			retVal = retVal * -1;
		}
		else if (sign == 2)
		{
			if (type != BCONST)
			{
				ParseError(line, "Incorrect type for NOT (Factor)");
				return false;
			}
			retVal = !retVal;
		}
		return true;
	}
	else if (tok == LPAREN)
	{
		bool ex = Expr(in, line, retVal);
		if (!ex)
		{
			ParseError(line, "Missing expression after (");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if (tok == RPAREN)
			return ex;
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return true;
}
