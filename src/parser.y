%code requires {
    #include "ast.hpp"
}

%{
#include <iostream>
#include <cstdlib>
#include "ast.hpp"
extern int yylex();
void yyerror(const char* s);
ASTNode* root;
%}

%union {
    int num;
    char* str;
    ASTNode* node;
    std::vector<ASTNode*>* block;
}

%token PRINT
%token <num> NUMBER
%token <str> IDENTIFIER
%token PLUS MINUS ASSIGN SEMICOLON
%token MULT DIV
%token WHILE LESS GREATER LPAREN RPAREN LBRACE RBRACE
%token EQ NEQ LEQ GEQ
%token IF ELSE

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%left EQ NEQ
%left LESS GREATER LEQ GEQ
%left PLUS MINUS
%left MULT DIV

%type <node> expression statement
%type <block> statement_list

%%
program:
    statement_list { 
        BlockNode* b = new BlockNode();
        b->statements = *$1;
        root = b;
        delete $1;
    }
    ;

statement_list:
    statement { $$ = new std::vector<ASTNode*>(); $$->push_back($1); }
    | statement_list statement { $1->push_back($2); $$ = $1; }
    ;

statement:
    IDENTIFIER ASSIGN expression SEMICOLON { 
        $$ = new BinOpNode("=", new VarNode($1), $3); 
        free($1);
    }
    | PRINT LPAREN expression RPAREN SEMICOLON {
        $$ = new PrintNode($3);
    }
    | WHILE LPAREN expression RPAREN LBRACE statement_list RBRACE {
        BlockNode* b = new BlockNode();
        b->statements = *$6;
        $$ = new WhileNode($3, b);
        delete $6;
    }
    | IF LPAREN expression RPAREN LBRACE statement_list RBRACE %prec LOWER_THAN_ELSE {
        BlockNode* b = new BlockNode();
        b->statements = *$6;
        $$ = new IfNode($3, b);
        delete $6;
    }
    | IF LPAREN expression RPAREN LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE {
        BlockNode* bIf = new BlockNode();
        bIf->statements = *$6;
        BlockNode* bElse = new BlockNode();
        bElse->statements = *$10;
        $$ = new IfNode($3, bIf, bElse);
        delete $6;
        delete $10;
    }
    ;

expression:
    NUMBER { $$ = new NumberNode($1); }
    | IDENTIFIER { 
        $$ = new VarNode($1); 
        free($1); 
    }
    | expression PLUS expression { $$ = new BinOpNode("+", $1, $3); }
    | expression MINUS expression { $$ = new BinOpNode("-", $1, $3); }
    | expression MULT expression { $$ = new BinOpNode("*", $1, $3); }
    | expression DIV expression { $$ = new BinOpNode("/", $1, $3); }
    | expression LESS expression { $$ = new BinOpNode("<", $1, $3); }
    | expression GREATER expression { $$ = new BinOpNode(">", $1, $3); }
    | expression EQ expression { $$ = new BinOpNode("==", $1, $3); }
    | expression NEQ expression { $$ = new BinOpNode("!=", $1, $3); }
    | expression LEQ expression { $$ = new BinOpNode("<=", $1, $3); }
    | expression GEQ expression { $$ = new BinOpNode(">=", $1, $3); }
    ;
%%

void yyerror(const char* s) {
    std::cerr << "Parse error: " << s << std::endl;
}