// .LIB reader
// Author: David Berthelot

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "libobjects.hxx"
#include "LexemeTable.hxx"
#include "libfile.tab.hxx"

using namespace std;

// int isatty(int x) {
//     return 1;
// }

int          DLIB_line;
extern int          libfileparse();
extern int          libexprparse();
extern int          libexpr_scan_string(const char *);
extern FILE *libfilein;
extern int   libfiledebug;
static LexemeTable LEXEMES;
LexemeTable       *DLIB_LEXEMES = &LEXEMES;
DLIB::Group       *toplib       = 0;
DLIB::Expr        *DLIB_Parsed_expr = 0;

pair<bool,DLIB::Group*> DLIB::parse_lib_file(const char *filename)
{
    libfilein    = filename ? fopen(filename,"r") : stdin;
    libfiledebug = 0;
    DLIB_line    = 1;

    const bool isok = !libfileparse();
    
    return make_pair(isok,toplib);
}

pair<bool,DLIB::Expr*> DLIB::parse_expression_string(const char *expr)
{
    DLIB_line        = 0;
    DLIB_Parsed_expr = 0;

    libexpr_scan_string(expr);

    const bool isok  = !libexprparse();

    return make_pair(isok,DLIB_Parsed_expr);
}

//-----------------------------------------------------------------------------
// Class Object
//-----------------------------------------------------------------------------
DLIB::Object::Object(const char *name):
    _name(DLIB_LEXEMES->get(name,false))
{
}

DLIB::Object::~Object()
{
}

const char *DLIB::Object::get_name() const {return _name;}


//-----------------------------------------------------------------------------
// Class ArgList
//-----------------------------------------------------------------------------
const DLIB::Arg *DLIB::ArgList::get_unique_arg() const
{
    if (this->size() == 1) {
        return *this->begin();
    } else {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Class GroupList
//-----------------------------------------------------------------------------
const vector<const DLIB::Group*> DLIB::GroupList::find_groups(const char *name) const
{
    const char           *lname = DLIB_LEXEMES->get(name,false);
    vector<const Group*>  v;
    
    for (const_iterator x = begin(); x != end(); ++x) {
        if ((*x)->get_name() == lname) {
            v.push_back(*x);
        }
    }
    return v;
}


//-----------------------------------------------------------------------------
// Class AttrList
//-----------------------------------------------------------------------------
const DLIB::Attr *DLIB::AttrList::find_attr(const char *name) const
{
    const char *lname = DLIB_LEXEMES->get(name,false);
    
    for (const_iterator x = begin(); x != end(); ++x) {
        if ((*x)->get_name() == lname) {
            return *x;
        }
    }
    return 0;
}


//-----------------------------------------------------------------------------
// Class Attr
//-----------------------------------------------------------------------------
        
DLIB::Attr::Attr(const char *name,const char *str,const Arg::T_Type t):
    Object(name),Arg(str,t)
{
}

DLIB::Attr::Attr(const char *name,const float number):
    Object(name),DLIB::Arg(number)
{
}

DLIB::Attr::Attr(const char *name,const ArgList *args):
    Object(name),DLIB::Arg(args)
{
}

DLIB::Attr::Attr(const char *name,const Expr *expr):
    Object(name),DLIB::Arg(expr)
{
}

DLIB::Attr::Attr(const char *name,const BitExpr *bit_expr):
    Object(name),DLIB::Arg(bit_expr)
{
}

DLIB::Attr::~Attr()
{
}


//-----------------------------------------------------------------------------
// Class Group
//-----------------------------------------------------------------------------

// objlist is freed by the constructor
DLIB::Group::Group(const char *name,const ArgList *args,list<Object*> *objs):
    Object(name),_args(args)
{
    if (objs) {
        for (list<Object*>::const_iterator x=objs->begin(); x!=objs->end(); ++x) {
            Group *g = dynamic_cast<Group*>(*x);
            Attr  *a = dynamic_cast<Attr*>(*x);

            if (g) {
                _groups.push_back(g);
            } else if (a) {
                _attrs.push_back(a);
            } else {
                assert(0);
            }
        }
        delete objs;
    }
}

DLIB::Group::~Group()
{
    if (_args) {
        for (ArgList::const_iterator x=_args->begin(); x!=_args->end(); ++x) {
            delete *x;
        }
        delete _args;
    }
    for (AttrList::const_iterator x=_attrs.begin(); x!=_attrs.end(); ++x) {
        delete *x;
    }
    for (GroupList::const_iterator x=_groups.begin(); x!=_groups.end(); ++x) {
        delete *x;
    }
}

const DLIB::ArgList   *DLIB::Group::get_args()      const {return _args;}
const DLIB::GroupList *DLIB::Group::get_subgroups() const {return &_groups;}
const DLIB::AttrList  *DLIB::Group::get_attrs()     const {return &_attrs;}
const DLIB::Attr      *DLIB::Group::find_attr(const char *name) const {return _attrs.find_attr(name);}
const DLIB::Arg       *DLIB::Group::get_unique_arg()            const {return _args ? _args->get_unique_arg() : 0;}
const vector<const DLIB::Group*> DLIB::Group::find_groups(const char *name) const {return _groups.find_groups(name);}


//-----------------------------------------------------------------------------
// Class Arg
//-----------------------------------------------------------------------------
DLIB::Arg::Arg(const char *name,const T_Type t):
    _t(t),_str(name)
{
}

DLIB::Arg::Arg(float number):
    _t(T_NUMBER),_number(number)
{
}

DLIB::Arg::Arg(const ArgList *args):
    _t(T_COMPLEX),_complex(args)
{
}

DLIB::Arg::Arg(const Expr *expr):
    _t(T_EXPR),_expr(expr)
{
}

DLIB::Arg::Arg(const BitExpr *bitexpr):
    _t(T_BIT_EXPR),_bitexpr(bitexpr)
{
}

DLIB::Arg::~Arg()
{
    switch(get_type()) {
    case T_COMPLEX:
        for (ArgList::const_iterator x=_complex->begin(); x!=_complex->end(); ++x) {
            delete *x;
        }
        delete _complex;
        break;
    case T_EXPR:
        delete _expr;
        break;
    case T_BIT_EXPR:
        delete _bitexpr;
        break;
    default:
        break;
    }
}

DLIB::Arg::T_Type DLIB::Arg::get_type() const
{
    return _t;
}

const float DLIB::Arg::get_number()  const
{
    if (get_type() == T_NUMBER) {
        return _number;
    } else {
        return 0;
    }
}

const char  *DLIB::Arg::get_keyword() const
{
    if (get_type() == T_KEYWORD) {
        return _str;
    } else {
        return 0;
    }
}

const char  *DLIB::Arg::get_text()    const
{
    if (get_type() == T_TEXT) {
        return _str;
    } else {
        return 0;
    }
}

const vector<float> DLIB::Arg::get_text_as_vector() const
{
    vector<float> v;

    if (get_type() != T_TEXT) {
        return v;
    }
    string s(this->get_text());

    int pos1 = 0;
    int pos2 = s.find(',',pos1);

    while (pos2 > 0) {
        v.push_back(atof(s.substr(pos1,pos2).c_str()));
        pos1 = pos2+1;
        pos2 = s.find(',',pos1);
    }
    v.push_back(atof(s.substr(pos1).c_str()));

    return v;
}

const DLIB::ArgList *DLIB::Arg::get_complex() const
{
    if (get_type() != T_COMPLEX) {
        return 0;
    } else {
        return _complex;
    }
}

const DLIB::Expr *DLIB::Arg::get_expr() const
{
    if (get_type() != T_EXPR) {
        return 0;
    } else {
        return _expr;
    }
}

const DLIB::BitExpr *DLIB::Arg::get_bit_expr() const
{
    if (get_type() != T_BIT_EXPR) {
        return 0;
    } else {
        return _bitexpr;
    }
}


//-----------------------------------------------------------------------------
// Class Expr
//-----------------------------------------------------------------------------
DLIB::Expr::Expr(const Arg  *a,const T_Type op,const Arg *b):
    _t(op),_isaArg(true),_isbArg(true),_aa(a),_ba(b)
{
}

DLIB::Expr::Expr(const Expr *a,const T_Type op,const Arg *b):
    _t(op),_isaArg(false),_isbArg(true),_ae(a),_ba(b)
{
}

DLIB::Expr::Expr(const Expr *a,const T_Type op,const Expr *b):
    _t(op),_isaArg(false),_isbArg(false),_ae(a),_be(b)
{
}

DLIB::Expr::~Expr()
{
    if (_isaArg) {
        if (_aa) delete _aa;
    } else {
        if (_ae) delete _ae;
    }
    if (_isbArg) {
        if (_ba) delete _ba;
    } else {
        if (_be) delete _be;
    }
}

DLIB::Expr::T_Type  DLIB::Expr::get_type()        const {return _t;}
bool                DLIB::Expr::is_first_expr()   const {return !_isaArg;}
const DLIB::Arg    *DLIB::Expr::get_first_arg()   const {return _isaArg ? _aa : 0;}
const DLIB::Expr   *DLIB::Expr::get_first_expr()  const {return _isaArg ? 0 : _ae;}
bool                DLIB::Expr::is_second_expr()  const {return !_isbArg;}
const DLIB::Arg    *DLIB::Expr::get_second_arg()  const {return _isbArg ? _ba : 0;}
const DLIB::Expr   *DLIB::Expr::get_second_expr() const {return _isbArg ? 0 : _be;}


//-----------------------------------------------------------------------------
// Class Expr
//-----------------------------------------------------------------------------
DLIB::BitExpr::BitExpr(const char *name,const int index):
    Object(name),_t(T_INDEX),_index(index),_to(-1)
{
}

DLIB::BitExpr::BitExpr(const char *name,const int from,const int to):
    Object(name),_t(T_SLICE),_from(from),_to(to)
{
}

DLIB::BitExpr::~BitExpr()
{
}

DLIB::BitExpr::T_Type DLIB::BitExpr::get_type() const {return _t;}
int         DLIB::BitExpr::get_index() const {return (get_type() == T_INDEX) ? _index : -1;}
int         DLIB::BitExpr::get_from()  const {return (get_type() == T_SLICE) ? _from : -1;}
int         DLIB::BitExpr::get_to()    const {return (get_type() == T_SLICE) ? _to   : -1;}
