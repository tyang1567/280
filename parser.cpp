/*
Author: Tiffany Yang
Created: 11/12/23
Description: Implementation of Recursive-Descent Parser
	for a Simple Pasacal-Like Language
*/

#include "parser.h"

map<string, bool> defVar;
map<string, Token> SymTable;

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

// WriteLnStmt ::= writeln (ExprList)
bool WriteLnStmt(istream &in, int &line)
{

	LexItem t = Parser::GetNextToken(in, line);

	if (t != WRITELN)
	{
		ParseError(line, "Missing WRITELN (WriteLnStmt)");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex)
	{
		ParseError(line, "Missing expression list for WriteLn statement");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	// Evaluate: print out the list of expressions values

	return ex;
} // End of WriteLnStmt

// WriteStmt ::= Write(ExprList)
bool WriteStmt(istream &in, int &line)
{
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if (t != WRITE)
	{
		ParseError(line, "Missing WRITE Keyword (WriteStmt)");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{
		ParseError(line, "Missing left parenthesis (WriteStmt)");
		return false;
	}
	bool status = ExprList(in, line);
	if (!status)
	{
		ParseError(line, "Missing ExprList (Writestmt)");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{
		ParseError(line, "Missing right parenthesis (WriteStmt)");
		return false;
	}
	return true;
}

// ExprList:= Expr {,Expr}
bool ExprList(istream &in, int &line)
{
	bool status = false;
	status = Expr(in, line);
	if (!status)
	{
		ParseError(line, "Missing Expression");
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA)
	{
		status = ExprList(in, line);
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
} // ExprList

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

// DeclPart ::= VAR DeclStmt; {DeclStmt ; }
bool DeclPart(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	if (t != VAR)
	{
		ParseError(line, "Missing VAR (DeclPart)");
		return false;
	}
	bool status = DeclStmt(in, line);
	if (!status)
	{
		ParseError(line, "Missing Declaration Statement (DeclPart)");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	while (t == SEMICOL)
	{
		status = DeclStmt(in, line);
		if (!status)
		{
			ParseError(line, "Semicolon with no Declaration (DeclPart)");
			return false;
		}
		t = Parser::GetNextToken(in, line);
	}
	Parser::PushBackToken(t);

	return true;
} // end DeclPart

// DeclStmt ::= IDENT {, IDENT } : Type [:= Expr]
bool DeclStmt(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);

	if (t.GetToken() == BEGIN)
	{
		Parser::PushBackToken(t);
		return true;
	}
	bool status = true;
	if (t.GetToken() != IDENT)
	{
		ParseError(line, "No IDENT in DeclStmt");
		return false;
	}
	defVar.insert({t.GetLexeme(), true});

	t = Parser::GetNextToken(in, line);

	while (t == COMMA)
	{
		t = Parser::GetNextToken(in, line);

		if (t.GetToken() != IDENT)
		{
			ParseError(line, "No Identifier after Comma (DeclStmt)");
			return false;
		}
		if (defVar[t.GetLexeme()])
		{
			ParseError(line, "Redefinition of Variable");
			return false;
		}
		defVar.insert({t.GetLexeme(), true});
		t = Parser::GetNextToken(in, line);
	}

	if (t != COLON)
	{
		ParseError(line, "Missing colon in (DeclStmt)");
		return false;
	}
	status = Type(in, line);
	if (!status)
	{
		ParseError(line, "No Type (DeclStmt)");
		return false;
	}
	t = Parser::GetNextToken(in, line);

	if (t.GetToken() == ASSOP)
	{
		status = Expr(in, line);
		if (!status)
		{
			ParseError(line, "ASSOP but no EXPR (DeclStmt)");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(t);
	}

	return true;
}
// End DeclStmt

// StructuredStmt ::= IfStmt | Compound Stmt

bool StructuredStmt(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	if (t == IF)
	{
		Parser::PushBackToken(t);
		return IfStmt(in, line);
	}
	else if (t == BEGIN)
	{
		Parser::PushBackToken(t);
		return CompoundStmt(in, line);
	}
	ParseError(line, "Incorrect Structured Statement (StructuredStmt)");
	return false;
}

// Stmt ::= SimpleStmt | StructuredStmt
bool Stmt(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	if (t == IF || t == BEGIN)
	{
		Parser::PushBackToken(t);
		return StructuredStmt(in, line);
	}
	else if (t.GetToken() == IDENT || t.GetToken() == WRITELN || t.GetToken() == WRITE)
	{
		Parser::PushBackToken(t);
		return SimpleStmt(in, line);
	}
	ParseError(line, "Incorrect Statement (Stmt)");
	return false;
}

// SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
bool SimpleStmt(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
	bool is_stmt;
	if (t.GetToken() == IDENT || t.GetToken() == WRITELN || t.GetToken() == WRITE)
	{

		if (t.GetToken() == IDENT)
		{
			is_stmt = (AssignStmt(in, line));
			if (!is_stmt)
			{
				ParseError(line, "Incorrect Assignment Statement (SimpleStmt)");
				return false;
			}
			return true;
		}
		else if (t.GetToken() == WRITELN)
		{
			is_stmt = (WriteLnStmt(in, line));
			if (!is_stmt)
			{
				ParseError(line, "Error in Writeln Statement (SimpleStmt)");
				return false;
			}
			return true;
		}
		else if (t.GetToken() == WRITE)
		{
			is_stmt = (WriteStmt(in, line));
			if (!is_stmt)
			{
				ParseError(line, "Error in Write Statement (SimpleStmt)");
				return false;
			}
		}
		return true;
	}

	return true;
} // End SimpleStmt

// Compound Stmt ::= BEGIN Stmt {; Stmt } END
bool CompoundStmt(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	if (t.GetToken() != BEGIN)
	{
		ParseError(line, "Missing Begin (CompoundStmt)");
		return false;
	}
	bool status = Stmt(in, line);
	if (!status)
	{
		ParseError(line, "Missing Statement in (CompountStmt)");
		return false;
	}
	t = Parser::GetNextToken(in, line);

	while (t == SEMICOL)
	{
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "SemiCol with no Stmt (CompountStmt)");
			return false;
		}
		t = Parser::GetNextToken(in, line);
	}

	if (t.GetToken() != END)
	{
		ParseError(line, "Missing END (CompoundStmt)");
		return false;
	}
	return true;
}

// IfStmt ::= IF Expr THEN Stmt [ELSE Stmt]
bool IfStmt(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	if (t != IF)
	{
		ParseError(line, "Missing IF keyword in (IfStmt)");
		return false;
	}
	bool status;
	status = Expr(in, line);
	if (!status)
	{
		ParseError(line, "Missing Expr in (IfStmt)");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t != THEN)
	{
		ParseError(line, "Missing THEN keyword in (IfStmt) ");
		return false;
	}
	status = Stmt(in, line);
	if (!status)
	{
		ParseError(line, "Missing Stmt in (IfStmt)");
		return false;
	}
	while (true)
	{
		t = Parser::GetNextToken(in, line);
		if (t != ELSE)
		{
			Parser::PushBackToken(t);
			break;
		}
		else
		{
			bool loop_st = Stmt(in, line);
			if (!loop_st)
			{
				ParseError(line, "No Stmt after ELSE (IfStmt)");
				return false;
			}
		}
	}
	return true;
}

// AssignStmt ::= Var:= Expr
bool AssignStmt(istream &in, int &line)
{
	bool status = Var(in, line);
	if (!status)
	{
		ParseError(line, "Missing Var in (AssignStmt)");
		return false;
	}
	LexItem t = Parser::GetNextToken(in, line);
	if (t.GetToken() != ASSOP)
	{
		ParseError(line, "Missing ASSOP after Var in AssignStmt");
		return false;
	}

	status = Expr(in, line);
	if (!status)
	{
		ParseError(line, "Missing Expr in AssignStmt");
		return false;
	}

	return true;
}

// Var ::= IDENT
bool Var(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	if (t.GetToken() != IDENT)
	{
		ParseError(line, "IDENT not in Var (Var)");
		return false;
	}

	if (defVar.find(t.GetLexeme()) == defVar.end())
	{
		ParseError(line, "Undec");
		return false;
	}

	return true;
}

// Expr ::= LogOrExpr ::= LogAndExpr {OR LogAndExpr }
bool Expr(istream &in, int &line)
{
	bool status;
	status = LogANDExpr(in, line);
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
			status = LogANDExpr(in, line);
			if (!status)
			{
				ParseError(line, "Missing LogANDExpr (loop, Expr)");
				return false;
			}
		}
	}

	return true;
}

// LogAndExpr ::= RelExpr {AND RelExpr}
bool LogANDExpr(istream &in, int &line)
{
	bool status;
	status = RelExpr(in, line);
	if (!status)
	{
		ParseError(line, "No RelExpr (LogANDExpr)");
		return false;
	}

	while (true)
	{
		LexItem t = Parser::GetNextToken(in, line);
		if (t != AND)
		{
			Parser::PushBackToken(t);
			break;
		}
		else
		{
			status = RelExpr(in, line);
			if (!status)
			{
				ParseError(line, "Missing RelExpr (loop, LogANDExpr)");
				return false;
			}
		}
	}
	return true;
}

// RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr]
bool RelExpr(istream &in, int &line)
{
	bool status = SimpleExpr(in, line);
	if (!status)
	{
		ParseError(line, "No Simple Expr (RelExpr)");
		return false;
	}
	LexItem t = Parser::GetNextToken(in, line);
	if (t.GetToken() == EQ)
	{
		status = SimpleExpr(in, line);
		if (!status)
		{
			ParseError(line, "EQ with no Expr (RelExpr)");
			return false;
		}
	}
	else if (t.GetToken() == LTHAN)
	{
		status = SimpleExpr(in, line);
		if (!status)
		{
			ParseError(line, "LTHAN with no Expr (RelExpr)");
			return false;
		}
	}
	else if (t.GetToken() == GTHAN)
	{
		status = SimpleExpr(in, line);
		if (!status)
		{
			ParseError(line, "GTHAN with no Expr (RelExpr)");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(t);
	}
	return true;
}

// SimpleExpr ::= Term { ( +|- ) Term}
bool SimpleExpr(istream &in, int &line)
{
	bool status;
	status = Term(in, line);
	if (!status)
	{
		ParseError(line, "Missing Term (SimpleExpr)");
		return false;
	}
	while (true)
	{
		LexItem t = Parser::GetNextToken(in, line);
		if (t.GetToken() == PLUS)
		{
			status = Term(in, line);
			if (!status)
			{
				ParseError(line, "Plus with no term (SimpleExpr)");
				return false;
			}
		}
		if (t.GetToken() == MINUS)
		{
			status = Term(in, line);
			if (!status)
			{
				ParseError(line, "Minus with no term (SimpleExpr)");
				return false;
			}
		}
		else
		{
			Parser::PushBackToken(t);
			break;
		}
	}

	return true;
}

// Term ::= Sfactor { ( * | / | DIV | MOD ) SFactor }
bool Term(istream &in, int &line)
{
	bool status = SFactor(in, line);
	if (!status)
	{
		ParseError(line, "No SFactor (Term)");
		return false;
	}

	while (true)
	{
		LexItem t = Parser::GetNextToken(in, line);

		if (t.GetToken() == MULT || t.GetToken() == IDIV || t.GetToken() == DIV || t.GetToken() == MOD)
		{
			status = SFactor(in, line);
			if (!status)
			{
				ParseError(line, "Operator without SFactor (Term)");
				return false;
			}
		}
		else
		{
			Parser::PushBackToken(t);
			break;
		}
	}

	return true;
}

// Sfactor ::= [( - | + | NOT) ] Factor

// NOT can be applied to Boolean type operands only
//  1 = Plus, -1 = Minus, 2 = NOT, 0 = No sign
bool SFactor(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	int sign;
	bool status;
	if (t.GetToken() == MINUS)
	{
		sign = -1;
		bool status = Factor(in, line, sign);
		if (!status)
		{
			ParseError(line, "Minus with no Factor (SFactor)");
			return false;
		}
	}
	else if (t.GetToken() == PLUS)
	{
		sign = 1;
		bool status = Factor(in, line, sign);
		if (!status)
		{
			ParseError(line, "Plus with no Factor (SFactor)");
			return false;
		}
	}
	else if (t.GetToken() == NOT)
	{
		sign = 2;
		bool status = Factor(in, line, sign);
		if (!status)
		{
			ParseError(line, "NOT with no Factor (Sfactor)");
			return false;
		}
	}
	else
	{
		sign = 0;
		Parser::PushBackToken(t);
		status = Factor(in, line, sign);
		if (!status)
		{
			ParseError(line, "No Factor (Sfactor)");
			return false;
		}
	}

	return true;
}

// Factor ::= IDENT | ICONST | R CONST | SCONST | BCONST | (Expr)
bool Factor(istream &in, int &line, int sign)
{
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT)
	{
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;
		}
		return true;
	}
	else if (tok == ICONST)
	{
		return true;
	}
	else if (tok == SCONST)
	{
		return true;
	}
	else if (tok == RCONST)
	{
		return true;
	}
	else if (tok == BCONST)
	{
		return true;
	}
	else if (tok == LPAREN)
	{
		bool ex = Expr(in, line);
		if (!ex)
		{
			ParseError(line, "Missing expression after (");
			return false;
		}
		if (Parser::GetNextToken(in, line) == RPAREN)
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
	Parser::PushBackToken(tok);
	return false;
}
