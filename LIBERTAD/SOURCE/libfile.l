
%{
// .LIB reader
// (c) David Berthelot 2008, all rights reserved.
// For licence of use: contact david.berthelot@gmail.com

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "LexemeTable.hxx"
#include "libobjects.hxx"
#include "libfile.tab.hxx"
extern int DLIB_line;
extern LexemeTable *DLIB_LEXEMES;
%}
%option  noyywrap
%x comment

NUMBER  (\-)?[0-9][0-9_]*(\.[0-9][0-9_]*)?((e|E)(\+|\-)[0-9]+)?
FNUMBER (\-)?\.[0-9][0-9_]*((e|E)(\+|\-)[0-9]+)?
BNUMBER  1'b[01]
ID     [a-zA-Z]([a-zA-Z0-9!\-_\.]|([\]\[][0-9\]\[]*[a-zA-Z_!\-\.]))*
USTR   [a-zA-Z0-9][a-zA-Z0-9_!\-\.]*
STR    \"(\\\"|[^"])*\"

%%
{BNUMBER}                                 {libfilelval.c_lexeme = DLIB_LEXEMES->get(yytext);return W_NUMBER;} 
{NUMBER}|{FNUMBER}                        {libfilelval.f_number = atof(yytext);return F_NUMBER;} 
{ID}                                      {libfilelval.c_lexeme = DLIB_LEXEMES->get(yytext);return W_ID;} 
"["                                       {return K_OPEN_SQUARE;} 
"]"                                       {return K_CLOSE_SQUARE;} 
"("                                       {return K_OPEN_PAREN;} 
")"                                       {return K_CLOSE_PAREN;} 
"{"                                       {return K_OPEN_BRACE;} 
"}"                                       {return K_CLOSE_BRACE;} 
":"                                       {return K_COLON;} 
";"                                       {return K_SEMICOLON;} 
","                                       {return K_COMMA;} 
"+"                                       {return K_PLUS;}
"-"                                       {return K_MINUS;}
"*"                                       {return K_MULT;}
"/"                                       {return K_DIV;}
"=="                                      {return K_EQUAL;}
[|+]                                      {return K_OR;}
"^"                                       {return K_XOR;}
"&"                                       {return K_AND;}
{STR}                                     {libfilelval.c_lexeme = DLIB_LEXEMES->get(string(yytext).substr(1,strlen(yytext)-2).c_str());
                                           return W_STRING_LITERAL;}
{USTR}                                    {libfilelval.c_lexeme = DLIB_LEXEMES->get(yytext);return W_STRING_LITERAL;}
"!"                                       {return K_NOT;}
"'"                                       {return K_POST_NOT;}
[\n]                                      {DLIB_line++;}
[ \t\r]+
"\\"
"/*"                                      BEGIN(comment);
<comment>[\n]                             {DLIB_line++;}
<comment>"*/"                             BEGIN(INITIAL);
<comment>.
.                                         {printf("LIB-002:%d: illegal token '%s'\n",DLIB_line,yytext);/*yyerror("illegal token");*/}
%%

#ifndef yywrap
int yywrap() {
    return 0;
}
#endif






