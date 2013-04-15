// Verilog netlist reader
// Author: David Berthelot

#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <set>
#include <map>
#include "vlogobjects.hxx"
#include "LexemeTable.hxx"

// int isatty(int x) {
//     return 1;
// }

int          vlognetlistparse();
extern FILE *vlognetlistin;
extern int   vlognetlistdebug;
int          VLP_line;
LexemeTable *VLP_LEXEMES = 0;
VLP::Design *topdesign   = 0;

pair<bool,VLP::Design*> VLP::parse_vlog_file(const char *filename,const bool incremental)
{
    topdesign        = (incremental && topdesign) ? topdesign : new Design();
    vlognetlistin    = filename ? fopen(filename,"r") : stdin;
    VLP_LEXEMES      = VLP_LEXEMES  ? VLP_LEXEMES : new LexemeTable();
    vlognetlistdebug = 0;
    VLP_line         = 1;

    const bool isok = !vlognetlistparse();
    
    return make_pair(isok,topdesign);
}


//-----------------------------------------------------------------------------
// Class Object
//-----------------------------------------------------------------------------

VLP::Object::Object(const char *name,T_Type t):
    _name(name),_parent(0),_tobj(t)
{
}

VLP::Object::~Object()
{
}

bool VLP::Object::set_parent(Object *parent)
{
    const bool has_parent = _parent != 0;

    _parent = parent;

    return !has_parent;
}


//-----------------------------------------------------------------------------
// Class Expr
//-----------------------------------------------------------------------------

VLP::Expr::Expr(const char *name,const T_Type t):
    Object(name,O_EXPR),_t(t),_index(-1)
{
}

VLP::Expr::Expr(const char *name,const int index):
    Object(name,O_EXPR),_t(T_INDEX),_index(index)
{
}

VLP::Expr::Expr(const char *name,const Range *range):
    Object(name,O_EXPR),_t(T_RANGE),_range(range)
{
}

VLP::Expr::~Expr()
{
    if (get_type() == T_RANGE) {
        delete _range;
    }
}

VLP::Expr::T_Type VLP::Expr::get_type()  const {return _t;}
int               VLP::Expr::get_index() const {return get_type() == T_INDEX ? _index : -1;}
const VLP::Range *VLP::Expr::get_range() const {return get_type() == T_RANGE ? _range : 0;}

bool VLP::Expr::print() const 
{
    switch (get_type()) {
    case T_SIMPLE:
        printf("%s",this->get_name());
        break;
    case T_INDEX:
        printf("%s[%d]",this->get_name(),this->get_index());
        break;
    case T_RANGE:
        printf("%s[%d:%d]",this->get_name(),this->get_range()->first,this->get_range()->second);
        break;
    case T_CONSTANT:
        printf("%s",this->get_name());
        break;
    default:
        return false;
    }
    return true;
}


//-----------------------------------------------------------------------------
// Class Inst
//-----------------------------------------------------------------------------

VLP::Inst::Inst(const char *model,const char *name,InstInterfaceList *ports):
    Object(name,O_INST),_model(model),_ports(*ports)
{
    delete ports;
}

VLP::Inst::~Inst() 
{
    InstInterfaceList::const_iterator eit = _ports.begin();
    while (eit != _ports.end()) {
        delete (*eit);
        eit++;
    }
}

const VLP::Module *VLP::Inst::get_parent_module() const
{
    return dynamic_cast<Module*>(this->get_parent());
}

const VLP::Module *VLP::Inst::get_instance_module() const
{
    return get_parent_module()->get_design()->get_module(_model);
}

const char   *VLP::Inst::get_instance_module_name() const
{
    return _model;
}

const VLP::InstInterfaceList &VLP::Inst::get_ports() const
{
    return _ports;
}

bool VLP::Inst::print() const 
{
    bool isok = true;
    printf("    %-16s %s (",get_instance_module_name(),this->get_name());
    InstInterfaceList::const_iterator elit = get_ports().begin();
    
    while (elit != get_ports().end()) {
        isok = isok && (*elit)->print();
        elit++;
        if (elit != get_ports().end()) {
            printf(",");
        }
    }
    printf(");\n");
    return isok;
}


//-----------------------------------------------------------------------------
// Class InstInterface
//-----------------------------------------------------------------------------
VLP::InstInterface::InstInterface(const char *name,const Expr     *expr):
    _is_conc(false),_formal(name),_expr(expr)
{
}

VLP::InstInterface::InstInterface(const char *name,const ExprList *lexpr):
    _is_conc(true),_formal(name),_lexpr(lexpr)
{
}

VLP::InstInterface::~InstInterface()
{
    if (_is_conc) {
        for (ExprList::const_iterator x=_lexpr->begin(); x!=_lexpr->end(); ++x) {
            delete *x;
        }
        delete _lexpr;
    } else if (_expr) {
        delete _expr;
    }
}

bool                 VLP::InstInterface::is_actual_conc()  const {return _is_conc;}
const char          *VLP::InstInterface::get_formal()      const {return _formal;}
const VLP::Expr     *VLP::InstInterface::get_actual_expr() const {return _is_conc ? 0 : _expr;}
const VLP::ExprList *VLP::InstInterface::get_actual_conc() const {return _is_conc ? _lexpr : 0;}

bool VLP::InstInterface::print() const 
{
    bool isok = true;
    
    if (get_formal()) {
        printf(".%s(",get_formal());
    }
    if (is_actual_conc()) {
        ExprList::const_iterator x = get_actual_conc()->begin();
    
        printf("{");
        while (x != get_actual_conc()->end()) {
            isok = isok && (*x)->print();
            x++;
            if (x != get_actual_conc()->end()) {
                printf(",");
            }
        }
        printf("}");
    } else if (get_actual_expr()) {
        isok = isok && get_actual_expr()->print();
    }
    if (get_formal()) {
        printf(")");
    }
    return isok;
}


//-----------------------------------------------------------------------------
// Class Wire
//-----------------------------------------------------------------------------

VLP::Wire::Wire(const char *name,T_Wire t,Range *r):
    Object(name,O_WIRE),_twire(t),_range(r)
{
}

VLP::Wire::~Wire()
{
    delete _range;
}

const VLP::Module *VLP::Wire::get_parent_module() const
{
    return dynamic_cast<Module*>(this->get_parent());
}

VLP::Wire::T_Wire  VLP::Wire::get_type()  const
{
    return _twire;
}

const VLP::Range  *VLP::Wire::get_range() const
{
    return _range;
}

bool VLP::Wire::print() const 
{
    static const char *types[] = {"*ERROR_WIRE*","input","inout","output",
                                  "wire","wand"  ,"wor"  ,"supply0","supply1",
                                  "tri" ,"triand","trior","tri0"   ,"tri1"};
    if (get_range()) {
        printf("    %-7s [%d:%d] %s;\n",types[_twire],get_range()->first,get_range()->second,this->get_name());
    } else {
        printf("    %-7s       %s;\n",types[_twire],this->get_name());
    }
    return true;
}


//-----------------------------------------------------------------------------
// Class Assign
//-----------------------------------------------------------------------------

VLP::Assign::Assign(const Expr *lhs,const Expr *rhs):
    Object(0,Object::O_ASSIGN),_lhs(lhs),_rhs(rhs)
{
}

VLP::Assign::~Assign()
{
    if (_lhs) delete _lhs;
    if (_rhs) delete _rhs;
}

const VLP::Expr   *VLP::Assign::get_lhs() const {return _lhs;}
const VLP::Expr   *VLP::Assign::get_rhs() const {return _rhs;}

const VLP::Module *VLP::Assign::get_parent_module() const
{
    return dynamic_cast<Module*>(this->get_parent());
}

bool VLP::Assign::print() const
{
    bool isok = true;
    printf("    assign ");
    isok = isok && get_lhs()->print();
    printf(" = ");
    isok = isok && get_rhs()->print();
    printf(";\n");
    return isok;
}


//-----------------------------------------------------------------------------
// Class Module
//-----------------------------------------------------------------------------

VLP::Module::Module(const char *name,NameList *nl):
    Object(name,O_MODULE),_namelist(*nl)
{
    delete nl;
}

VLP::Module::~Module() 
{
    WireList::const_iterator wit = _wirelist.begin();
    while (wit != _wirelist.end()) {
        delete (*wit);
        wit++;
    }
    AssignList::const_iterator ait = _assignlist.begin();
    while (ait != _assignlist.end()) {
        delete (*ait);
        ait++;
    }
    InstList::const_iterator iit = _instlist.begin();
    while (iit != _instlist.end()) {
        delete (*iit);
        iit++;
    }
}

bool VLP::Module::add_wire(Wire *w)
{
    w->set_parent(this);
    _wirelist.push_back(w);
    return true;
}

bool VLP::Module::add_assign(Assign *a)
{
    _assignlist.push_back(a);
    return a->set_parent(this);
}

bool VLP::Module::add_inst(Inst *i)
{
    _instlist.push_back(i);
    return i->set_parent(this);
}

// Frees *ol
bool VLP::Module::add_objects(ObjectList *ol)
{
    bool res = true;
    ObjectList::const_iterator it = ol->begin();
    
    while (res && (it != ol->end())) {
        switch((*it)->get_type()) {
        case O_WIRE:
            res = res && add_wire(dynamic_cast<Wire*>(*it));
            break;
        case O_ASSIGN:
            res = res && add_assign(dynamic_cast<Assign*>(*it));
            break;
        case O_INST:
            res = res && add_inst(dynamic_cast<Inst*>(*it));
            break;
        default:
            assert(0);
            break;
        }
        it++;
    }
    delete ol;
    return res;
}

const VLP::Design *VLP::Module::get_design() const
{
    return dynamic_cast<Design*>(this->get_parent());
}

bool VLP::Module::print() const 
{
    bool isok = true;

    printf("module %s(",this->get_name());
    NameList::const_iterator nit = get_port_names().begin();
    while (nit != get_port_names().end()) {
        printf("%s",*nit);
        nit++;
        if (nit != get_port_names().end()) {
            printf(",");
        }
    }
    printf(");\n");
    WireList::const_iterator wit = get_wire_list().begin();
    while (wit != get_wire_list().end()) {
        isok = isok && (*wit)->print();
        wit++;
    }
    printf("\n");
    AssignList::const_iterator ait = get_assign_list().begin();
    while (ait != get_assign_list().end()) {
        isok = isok && (*ait)->print();
        ait++;
    }
    printf("\n");
    InstList::const_iterator iit = get_instance_list().begin();
    while (iit != get_instance_list().end()) {
        isok = isok && (*iit)->print();
        iit++;
    }
    printf("endmodule // %s\n",this->get_name());
    return isok;
}


//-----------------------------------------------------------------------------
// Class Design
//-----------------------------------------------------------------------------

struct VLP::Design::data {
    LexemeTable               t;
    ModuleList                ml;
    map<const char*,Module*>  mm;
};

const char *root_design = "/work";

VLP::Design::Design():Object(root_design,Object::O_DESIGN),_data(new data())
{
    topdesign   = this;
    VLP_LEXEMES = &_data->t;
}

VLP::Design::~Design()
{
    for (map<const char*,Module*>::const_iterator x=_data->mm.begin(); x!=_data->mm.end(); ++x) {
        delete x->second;
    }
    if (VLP_LEXEMES == &_data->t) {
        VLP_LEXEMES = 0;
        assert(topdesign == this);
        topdesign   = 0;
    }
    delete _data;
}

const VLP::ModuleList &VLP::Design::get_modules() const
{
    return _data->ml;
}

const VLP::Module     *VLP::Design::get_module(const char *name) const
{
    const char *lname = _data->t.get(name);
    map<const char*,Module*>::const_iterator f = _data->mm.find(lname);

    if (f == _data->mm.end()) {
        return 0;
    } else {
        return f->second;
    }
}

bool VLP::Design::add_module(VLP::Module *m) 
{
    if (_data->mm.find(m->get_name()) == _data->mm.end()) {
        _data->mm.insert(make_pair(m->get_name(),m));
        _data->ml.push_back(m);
        return m->set_parent(this);
    } else {
        return false;
    }
}

bool VLP::Design::print() const 
{
    bool                       isok = true;
    for (ModuleList::const_iterator mit  = get_modules().begin(); mit != get_modules().end(); ++mit) {
        isok = isok && (*mit)->print();
        printf("\n");
    }
    return isok;
}

const VLP::NameList VLP::Design::find_top_modules() const
{
    NameList         top;
    set<const char*> mnames,instmnames;

    for (ModuleList::const_iterator mit = get_modules().begin(); mit != get_modules().end(); ++mit) {
        mnames.insert((*mit)->get_name());
        
        for (InstList::const_iterator iit = (*mit)->get_instance_list().begin(); iit != (*mit)->get_instance_list().end(); ++iit) {
            instmnames.insert((*iit)->get_instance_module_name());
        }
    }
    set_difference(mnames.begin(),mnames.end(),instmnames.begin(),instmnames.end(),back_inserter(top));

    return top;
}
