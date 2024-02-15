/*
Created: Wednesday, October 11, 20:00
    Title: CS 280 - Project Assignment 1
        Author: Tiffany Yang
*/
#include <iostream>
#include "lex.h"
#include <regex>
#include <string>
#include <map>
#include <cstring>

LexItem getNextToken(std::istream& in, int& linenum){

    enum TokState{
        START, INID, ININT, INSTRING, INRCONST, ERRORS
    }
    lexstate = START;

    std::string lexeme;
    char ch;
    

    while(in.get(ch)){
        if (ch == '\n')
            linenum++;

        switch(lexstate){
    

            //Errors
            if (in.peek() == EOF)
            lexstate = ERRORS;

            case START:
                //Check for comments
                 if(ch == '{'){
                while (ch != '}'){
                    continue;
                }
                lexstate = START;

            }//end bracket of (if) loop for comments


            //String Literals

            if(ch == '\''){
                lexstate = INSTRING;
            }

            //Operators
            if(ch == '+'){
                return LexItem(PLUS, "+", linenum);
            }

            if(ch == '-'){
                return LexItem(MINUS, "-", linenum);
            }

            if(ch == '*'){
                return LexItem(MULT, "*", linenum);
            }

            if(ch == '/'){
                return LexItem(DIV, "/", linenum);
            }

            if(ch == '='){
                return LexItem(EQ, "=", linenum);
            }

            if(ch == '<'){
                return LexItem(LTHAN, "<", linenum);
            }

            if(ch == '>'){
                return LexItem(GTHAN, ">", linenum);
            }
            
            //UNHANDLED: ASSOP :=

            //Is the character a number? --> Integer
            if(isdigit(ch)){
                lexstate = ININT;
            }

            //Is the character a letter from the alphabet? --> Identifier 
            if(isalpha(ch)){
                lexstate = INID;
            }
            break; //end START 

            case INID:
            while(ch != ' ' && ch != '\n'){
                lexeme += ch;
                if(std::regex_match(lexeme, std::regex("^[a-zA-Z]+[a-zA-Z]*[0-9]*[_$]*") ) )
                    return LexItem(IDENT, lexeme, linenum);
                else if (lexeme == "True" || lexeme == "False"){
                    return LexItem(BCONST, lexeme, linenum);
                }
            }
            break;

            case ININT:
            while( (isdigit(in.peek()) ) )
                lexeme += ch;
                return LexItem(INTEGER, lexeme, linenum);
            if(in.peek() == '.')
                lexstate = INRCONST;
            break;

            case INSTRING:
            break;

            case INRCONST:
            break;

            

        }

    }
    return LexItem(DONE, lexeme, linenum);

}

LexItem id_or_kw (const string& lexeme, int linenum){
    Token token = IDENT;

    std::map<std::string, Token> keywords = {
        {"and", AND}, {"begin", BEGIN}, {"boolean", BOOLEAN}, {"idiv", IDIV}, {"else", ELSE}, {"false", FALSE}, 
        {"if", IF}, {"integer", INTEGER}, {"mod", MOD}, {"not", NOT}, {"or", OR}, {"program", PROGRAM},
        {"real", REAL}, {"string", STRING}, {"write", WRITE}, {"writeln", WRITELN}, {"var", VAR}, {"end", END}, {"end", IDENT}

};

    for(auto &i : keywords){
       
        if( strcmp(lexeme.c_str(), i.first.c_str()) == 0 ){
            return LexItem(token, lexeme, linenum);
        }
        
    }
            
    return LexItem(IDENT, lexeme, linenum);
    
}

ostream& operator<< (ostream& out, const LexItem& tok){
    int line = tok.GetLinenum();
    Token toks = tok.GetToken();
    string lexeme = tok.GetLexeme();



    switch(toks){

        //Keywords
        case AND:
        out << "AND: " << lexeme << "\n";
        break;
        case BEGIN:
        out << "BEGIN: " << lexeme << "\n";
        break;
        case BOOLEAN:
        out << "BOOLEAN: " << lexeme << "\n";
        break;
        case IDIV:
        out << "IDIV: " << lexeme << "\n";
        break;
        case END:
        out << "END: " << lexeme << "\n";
        break;
        case ELSE:
        out << "ELSE: " << lexeme << "\n";;
        break;
        case FALSE:
        out << "FALSE: " << lexeme << "\n";
        break;
        case IF:
        out << "IF: " << lexeme << "\n";
        break;
        case INTEGER:
        out << "INTEGER: " << lexeme << "\n";
        break;
        case MOD:
        out << "MOD: " << lexeme << "\n";
        break;
        case NOT:
        out << "NOT: " << lexeme << "\n";
        break;
        case OR:
        out << "OR: " << lexeme << "\n";
        break;
        case PROGRAM:
        out << "PROGRAM: " << lexeme << "\n";
        break;
        case REAL:
        out << "REAL: " << lexeme << "\n";
        break;
        case STRING:
        out << "STRING: " << lexeme << "\n";
        break;
        case WRITE:
        out << "WRITE: " << lexeme << "\n";
        break;
        case WRITELN:
        out << "WRITELN: " << lexeme << "\n";
        break;
        case VAR:
        out << "VAR: " << lexeme << "\n";
        break;
        case ERR:
        out << "Error: {" << lexeme << "} at line no. " << line << "\n";
        break;
        case DONE:
        out << "DONE: " << lexeme << "\n";
        break;
        case TRUE:
        out << "TRUE: " << lexeme << "\n";
        break;
        case THEN:
        out << "THEN: " << lexeme << "\n";
        break;

        //Constants

        case ICONST:
        out << "ICONST: " << lexeme << "\n";
        break;
        case RCONST:
        out << "RCONST: " << lexeme << "\n";
        break;
        case SCONST:
        out << "SCONST: " << lexeme << "\n";
        break;
        case BCONST:
        out << "BCONST: " << lexeme << "\n";
        break;


        
        
    
        //Identifiers
        case IDENT:
        if(lexeme == "end")
            out << "END: [" << lexeme << "] at line no. " << line << "\n";
        else{
            out << "IDENT: \"" << lexeme << "\"" << "\n";
        }
        break;


        //Operators
        case PLUS:
        out << "PLUS" << "\n";
        break;
        case MINUS:
        out << "MINUS" << "\n";
        break;
        case MULT:
        out << "MULT" << "\n";
        break;
        case DIV:
        out << "DIV" << "\n";
        break;
        case ASSOP:
        out << "ASSOP" << "\n";
        break;
        case EQ:
        out << "EQ:" << "\n";
        break;
        case LTHAN:
        out << "LTHAN" << "\n";
        break;
        case GTHAN:
        out << "GTHAN" << "\n";
        break;

        //DELIMITERS
        case DOT:
        out << "DOT" << "\n";
        break;
        case COMMA:
        out << "COMMA" << "\n";
        break;
        case LPAREN:
        out << "LPAREN" << "\n";
        break;
        case RPAREN:
        out << "RPAREN" << "\n";
        break;
        case SEMICOL:
        out << "SEMICOL" << "\n";
        break;
        case COLON:
        out << "COLON" << "\n";
        break;


      
    

    }
    return out;
}