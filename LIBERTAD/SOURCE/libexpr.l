
%{
// .LIB expression parser
// (c) David Berthelot 2008, all rights reserved.
// For licence of use: contact david.berthelot@gmail.com

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "LexemeTable.hxx"
#include "libobjects.hxx"
#include "libexpr.tab.hxx"
extern int DLIB_line;
extern LexemeTable *DLIB_LEXEMES;
%}
%option  noyywrap

NUMBER   [0-9]+
BNUMBER  1'b[01]
ID     [a-zA-Z][a-zA-Z0-9\-_\.]*

%%
"["                                       {return K_OPEN_SQUARE;} 
"]"                                       {return K_CLOSE_SQUARE;} 
"("                                       {return K_OPEN_PAREN;} 
")"                                       {return K_CLOSE_PAREN;} 
"=="                                      {return K_EQUAL;}
[|+]                                      {return K_OR;}
"^"                                       {return K_XOR;}
[&*]                                      {return K_AND;}
"!"                                       {return K_NOT;}
"'"                                       {return K_POST_NOT;}

{BNUMBER}                                 {libexprlval.c_lexeme = DLIB_LEXEMES->get(yytext);return W_NUMBER;} 
{NUMBER}                                  {libexprlval.i_number = atoi(yytext);return I_NUMBER;} 
{ID}                                      {libexprlval.c_lexeme = DLIB_LEXEMES->get(yytext);return W_ID;} 
[\n]                                      {DLIB_line++;}
[ \t\r]+
.                                         {printf("LIB-002:0: illegal expression token '%s'\n",yytext);/*yyerror("illegal token");*/}
%%

#ifndef yywrap
int yywrap() {
    return 0;
}
#endif






