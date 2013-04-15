// Lexeme Table
// Author: David Berthelot

#include <stdio.h>
#include "LexemeTable.hxx"
#include <algorithm>

LexemeTable::LexemeTable()
{
}

LexemeTable::~LexemeTable()
{
}

const char *LexemeTable::get(const char *text,const bool case_sensitive)
{
    string stext = text;

    if (!case_sensitive) {
        transform(stext.begin(),stext.end(),stext.begin(),::tolower);
    }
    set<string>::const_iterator p = _text.find(stext);

    if (p == _text.end()) {
        _text.insert(stext);
        p = _text.find(stext);
    }
    return p->c_str();
}

void LexemeTable::print() const
{
    for (set<string>::const_iterator x=_text.begin(); x!=_text.end(); ++x) {
        printf("%8p \"%s\"\n",x->c_str(),x->c_str());
    }
}

//#define LEXEME_TABLE_SELF_TEST
#ifdef  LEXEME_TABLE_SELF_TEST
#include <iostream.h>

int main() 
{
    LexemeTable lt;
    
    cout << lt.get("Hello") << "\n";
    cout << lt.get("Bye") << "\n";
    cout << lt.get("Hello") << "\n";
    cout << lt.get("Bye") << "\n";

    lt.print();

    return 0;
}

#endif
