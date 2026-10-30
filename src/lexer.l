%{
#include "parser.hpp"
#include <cstdlib>
#include <cstring>
%}

%option noyywrap
%option nounistd
%option never-interactive

%%
"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"=="                    { return EQ; }
"!="                    { return NEQ; }
"<="                    { return LEQ; }
">="                    { return GEQ; }
"<"                     { return LESS; }
">"                     { return GREATER; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"print"                 { return PRINT; }
[0-9]+                  { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = _strdup(yytext); return IDENTIFIER; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MULT; }
"/"                     { return DIV; }
"="                     { return ASSIGN; }
";"                     { return SEMICOLON; }
[ \t\n\r]+              { }
.                       { return yytext[0]; }
%%