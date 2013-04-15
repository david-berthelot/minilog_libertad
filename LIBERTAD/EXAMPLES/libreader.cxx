// .LIB reader
// Author: David Berthelot

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <string>
#include "libobjects.hxx"

using namespace std;

const char *default_indent = "    ";

static void display_arglist(const DLIB::ArgList *l);
static void display_arg(const DLIB::Arg *a);

static void display_bitexpr(const DLIB::BitExpr *e) {
    printf("%s[",e->get_name());

    switch (e->get_type()) {
    case DLIB::BitExpr::T_INDEX:
        printf("%d",e->get_index());
        break;
    case DLIB::BitExpr::T_SLICE:
        printf("%d:%d",e->get_from(),e->get_to());
        break;
    default:
        assert(0);
        break;
    }
    printf("]");
}

static void display_expr(const DLIB::Expr *e) {
    const char table[] = "X|^&! +-*/";

    if (e->is_first_expr()) {
        display_expr(e->get_first_expr());
    } else {
        display_arg(e->get_first_arg());
    }
    printf(" %c ",table[e->get_type()]);
    if (e->is_second_expr()) {
        display_expr(e->get_second_expr());
    } else if (e->get_second_arg()) {
        display_arg(e->get_second_arg());
    }
}

static void display_arg(const DLIB::Arg *a) {
    switch (a->get_type()) {
    case DLIB::Arg::T_TEXT:
        printf("\"%s\"",a->get_text());
        break;
    case DLIB::Arg::T_KEYWORD:
        printf("%s",a->get_keyword());
        break;
    case DLIB::Arg::T_NUMBER:
        if (a->get_number() == int(a->get_number())) {
            printf("%d",int(a->get_number()));
        } else {
            printf("%f",a->get_number());
        }
        break;
    case DLIB::Arg::T_COMPLEX:
        display_arglist(a->get_complex());
        break;
    case DLIB::Arg::T_EXPR:
        display_expr(a->get_expr());
        break;
    case DLIB::Arg::T_BIT_EXPR:
        display_bitexpr(a->get_bit_expr());
        break;
    default:
        assert(0);
    }
}

static void display_arglist(const DLIB::ArgList *l) {
    printf("(");
    if (l) {
        const size_t len   = l->size() ;
        size_t       count = 0;

        for (DLIB::ArgList::const_iterator x = l->begin(); x != l->end(); ++x) {
            display_arg(*x);
            if (++count != len) {
                printf(", ");
            }
        }
    }
    printf(")");
}

static void display_group(const DLIB::Group *g,const string &indent) {
    const string newindent = indent + default_indent;

    printf("%s%s ",indent.c_str(),g->get_name());
    display_arglist(g->get_args());
    printf(" {\n");
    
    for (DLIB::AttrList::const_iterator x = g->get_attrs()->begin(); x != g->get_attrs()->end(); ++x) {
        printf("%s%s ",newindent.c_str(),(*x)->get_name());
        if ((*x)->get_type() != DLIB::Arg::T_COMPLEX) {
            printf(": ");
        }
        display_arg(*x);
        printf(";\n");
    }
    for (DLIB::GroupList::const_iterator x = g->get_subgroups()->begin(); x != g->get_subgroups()->end(); ++x) {
        display_group((*x),newindent);
    }
    printf("%s}\n",indent.c_str());
}

int main(int argc,char **argv)
{
    pair<bool,DLIB::Group*> g = make_pair(true,static_cast<DLIB::Group*>(0));
    const bool print = (argc > 1) ? strcmp(argv[1],"-p") == 0 : false;

    if ( argc > 1 ) {
        if ((argc > 2) && (!strcmp("-e",argv[1]))) {
            pair<bool,DLIB::Expr*>  e = DLIB::parse_expression_string(argv[argc-1]);
            display_expr(e.second);
            return e.first ? 0 : 1;
        }
        printf("Reading library %s\n",argv[argc-1]);
        g = DLIB::parse_lib_file(argv[argc-1]);
    } else {
        g = DLIB::parse_lib_file(0);
    }
    printf("Finished\n");
    fflush(stdin);

    if (0) {
        printf("---------------------------------------- Lexeme Table\n");
        //LEXEMES->print();
        printf("----------------------------------------\n");
    }    
    if (print) {
        string indent;
        display_group(g.second,indent);
    }

    // Display all cells
    if (0) {
        printf("Cells:\n");
        const vector<const DLIB::Group*> v = g.second->find_groups("CEll");
    
        for (size_t x=0; x<v.size(); ++x) {
            printf("%s ",v[x]->get_unique_arg()->get_keyword());
        }
        printf("\n");
    }
    delete g.second;
    return g.first ? 0 : 1;
}

