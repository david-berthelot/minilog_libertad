// Lexeme Table
// Author: David Berthelot

#ifndef  LEXEME_TABLE
#define  LEXEME_TABLE

#include <set>
#include <string>

using namespace std;

class LexemeTable 
{
public:
    LexemeTable();
    ~LexemeTable();

    const char *get(const char *text,const bool case_sensitive=true);
    void        print() const;

private:
    set<string> _text;
};

#endif

