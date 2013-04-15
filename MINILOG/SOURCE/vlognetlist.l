
%{
// Verilog netlist reader
// (c) David Berthelot 2007-2008, all rights reserved.
// For licence of use: contact david.berthelot@gmail.com

#include <stdio.h>
#include <stdlib.h>
#include "LexemeTable.hxx"
#include "vlogobjects.hxx"
#include "vlognetlist.tab.hxx"
extern int VLP_line;
extern LexemeTable *VLP_LEXEMES;

static inline const string replace_returns(const char *cs) {
    string s(cs);
    string t;
    int pos1 = 0;
    int pos2 = s.find('\n',pos1);

    while (pos2 > 0) {
        t += s.substr(pos1,pos2);
        pos1 = pos2+1;
        pos2 = s.find('\n',pos1);
        VLP_line++;
    }
    return t + s.substr(pos1);
}
%}
%option  noyywrap
%x comment

UNSIGNED [0-9][0-9_]*
BINARY   ([1-9][0-9_]*)?['][sS]?[bB][01xXzZ?][01xXzZ?_]*
ID       [a-zA-Z_][a-zA-Z0-9_]*
ID2      \\([^ \n]|(\\[ ]))+[ ]
ID3      \\([^ ]|(\\[ ]))+[ ]
COM      \/\/[^\n]*

%%
{COM}
module                  {return KW_MODULE;}
endmodule               {return KW_ENDMODULE;}
input                   {return KW_INPUT;}
inout                   {return KW_INOUT;}
output                  {return KW_OUTPUT;}
wire                    {return KW_WIRE;}
wand                    {return KW_WAND;}
wor                     {return KW_WOR;}
supply0                 {return KW_SUPPLY0;}
supply1                 {return KW_SUPPLY1;}
triand                  {return KW_TRIAND;}
trior                   {return KW_TRIOR;}
tri0                    {return KW_TRI0;}
tri1                    {return KW_TRI1;}
tri                     {return KW_TRI;}
or                      {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext); return KW_OR;}
nor                     {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext); return KW_NOR;}
xor                     {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext); return KW_XOR;}
xnor                    {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext); return KW_XNOR;}
and                     {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext); return KW_AND;}
nand                    {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext); return KW_NAND;}
not                     {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext); return KW_NOT;}
assign                  {return KW_ASSIGN;}
{BINARY}                {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext); return CONSTANT;}
{UNSIGNED}              {vlognetlistlval.i_int    = atoi(yytext); return INT;}
"="                     {return ASSIGN;} 
"'"                     {return QUOTE;} 
":"                     {return COLON;} 
";"                     {return SEMICOLON;} 
"."                     {return DOT;} 
","                     {return COMMA;} 
"("                     {return OPEN_PAREN;} 
")"                     {return CLOSE_PAREN;} 
"{"                     {return OPEN_BRACE;} 
"}"                     {return CLOSE_BRACE;} 
"["                     {return OPEN_SQUARE;} 
"]"                     {return CLOSE_SQUARE;}
{ID}                    {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext);return ID;} 
{ID2}                   {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(yytext);return ID;} 
{ID3}                   {vlognetlistlval.i_lexeme = VLP_LEXEMES->get(replace_returns(yytext).c_str());return ID;} 
[\n]                    {VLP_line++;}
[ \t\r]+
"/*"                    BEGIN(comment);
<comment>[\n]           {VLP_line++;}
<comment>"*/"           BEGIN(INITIAL);
<comment>.
.                       {printf("VLP-002:%d: %s\n",VLP_line,yytext);/*yyerror("illegal token");*/}
%%

#ifndef yywrap
int yywrap() {
    return 0;
}
#endif
