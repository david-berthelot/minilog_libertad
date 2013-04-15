/// @mainpage  MiniLog: a fast lightweight Verilog netlist parser
/// @author    David Berthelot
/// @version   1.0
/// @date      2007-2008
/// @attention MIT licence

#ifndef VLOGNETLIST_OBJECTS
#define VLOGNETLIST_OBJECTS

#include <list>
#include <vector>

using namespace std;

/// The namespace encapsulating verilog parser to avoid naming conflict with your code. Details follow
/** The following rules apply in all of the API calls in this library
 *    -# Pointers returned by APIs must not be freed (unless explicitely stated otherwise in the API documentation).
 */
namespace VLP {
    class Object;
    class Expr;
    class Wire;
    class Assign;
    class Inst;
    class InstInterface;
    class Module;
    class Design;

    /// This is the main parsing function
    /** @param filename is the path to the verilog filename to be parsed
        @param incremental when set to true, multiple call to this function keep adding to the prevously generated design, when false, the function creates a new design
        @return a pair which contains the status (bool) and the design
        @attention the returned Design pointer must be freed to release the memory when you're finished using it
    */
    pair<bool,Design*> parse_vlog_file(const char *filename,const bool incremental=true);

    typedef list<const char *> NameList;   ///< A list of names
    typedef pair<int,int>      Range;      ///< A range, typically used for describing wire and port ranges [from,to], from is referred to as first, to is referred to as second
    typedef list<InstInterface*>    InstInterfaceList; ///< A list of instance interfaces
    typedef list<Expr*>        ExprList;   ///< A list of expressions
    typedef list<Inst*>        InstList;   ///< A list of instances
    typedef list<Wire*>        WireList;   ///< A list of wires
    typedef list<Assign*>      AssignList; ///< A list of assignments
    typedef list<Object*>      ObjectList; ///< A list of objects
    typedef list<Module*>      ModuleList; ///< A list of modules

    /// This class provides common methods to all verilog objects, this class is always inherited.
    class Object {
    public:
        /// The type of the object, representing a verilog object
        enum T_Type {O_UNKNOWN, ///< This is the type used to catch errors
                     O_WIRE,    ///< For INPUT, INOUT, OUTPUT, WIRE, WAND, WOR, TRI, TRIAND, TRIOR, SUPPLY0, SUPPLY1, TRI0, TRI1
                     O_ASSIGN,  ///< For assignments
                     O_INST,    ///< For cell instance
                     O_MODULE,  ///< For module definition
                     O_EXPR,    ///< For slice expression  (ex: a[1])
                     O_DESIGN   ///< For design (typically referred to as /work library)
        };
        T_Type       get_type()   const {return _tobj;}   ///< Returns the type of the object
        const char  *get_name()   const {return _name;}   ///< Returns the name of the object
        Object      *get_parent() const {return _parent;} ///< Returns the parent object of the object

        virtual bool print() const = 0;

        Object(const char *name,T_Type t); ///< @internal
        virtual ~Object();

    protected:
        const char  *_name;

    private:
        Object      *_parent;
        T_Type       _tobj;
    private:
        friend class Module;
        friend class Design;
        bool         set_parent(Object *parent);
    };

    /// This class provides simple bitselection expression support (example: mywire[3]) and constant support (example: 1'b0)
    class Expr : public Object {
    public:
        /// The type of the expression
        enum T_Type {T_UNKNOWN, ///< This is the type used to catch errors
                     T_SIMPLE,  ///< For simple wires (example: mywire)
                     T_INDEX,   ///< For indexed wires (example: mywire[3])
                     T_RANGE,   ///< For sliced wires (example: mywire[3:5])
                     T_CONSTANT ///< For constants (example: 3'b0_1x)
        };
        T_Type       get_type()  const; ///< Returns the type of the expression
        int          get_index() const; ///< Returns the index if the expression is indexed, else -1
        const Range *get_range() const; ///< Returns the range if the expression is sliced, else 0
        bool         print()     const; ///< Prints the content of this object for debugging purposes

        Expr(const char *name,const T_Type t);     ///< @internal
        Expr(const char *name,const int    index); ///< @internal
        Expr(const char *name,const Range *range); ///< @internal
        ~Expr();
        
    private:
        T_Type _t;
        const union {
            int          _index;
            const Range *_range;
        };
    };

    /// This class represents verilog wires as well as ports (which are special types of wires)
    class Wire : public Object {
    public:
        /// The type of the wire
        enum T_Wire {W_UNKNOWN, ///< This is the type used to catch errors
                     W_IN,      ///< For input ports
                     W_INOUT,   ///< For inout ports
                     W_OUT,     ///< For output ports
                     W_WIRE,    ///< For simple wires
                     W_WAND,    ///< For multidriven and-resolved wires
                     W_WOR,     ///< For multidriven or-resolved wires
                     W_SUPPLY0, ///< For constant 0 wires
                     W_SUPPLY1, ///< For constant 1 wires
                     W_TRI,     ///< For tristate wires
                     W_TRIAND,  ///< For tristate and-resolved wires
                     W_TRIOR,   ///< For tristate or-resolved wires
                     W_TRI0,    ///< For tristate constant 0 wires
                     W_TRI1     ///< For tristate constant 1 wires
        };
        T_Wire        get_type()  const;         ///< Returns the type of the wire
        const Range  *get_range() const;         ///< Returns the declared range of the wire [from,to], else 0 if there's no declared range
        const Module *get_parent_module() const; ///< Returns the module to which this wire belongs

        bool print() const; ///< Prints the content of this object for debugging purposes

        Wire(const char *name,T_Wire t,Range *r=0); ///< @internal
        ~Wire();
    private:
        T_Wire  _twire;
        Range  *_range;
    };

    /// Describes a verilog assignment, note that the name of an assignment is always NULL
    class Assign : public Object {
    public:
        const Expr   *get_lhs() const; ///< Returns the left hand side (lhs) of the assignment
        const Expr   *get_rhs() const; ///< Returns the right hand side (rhs) of the assignment
        const Module *get_parent_module() const; ///< Returns the module to which this assignment belongs

        bool print() const; ///< Prints the content of this object for debugging purposes

        Assign(const Expr *lhs,const Expr *rhs);   ///< @internal
        ~Assign();
    private:
        const Expr *_lhs,*_rhs;
    };

    /// Describes a verilog cell instance
    class Inst : public Object {
    public:
        const Module *get_parent_module()        const; ///< Returns the module to which this instance belongs to
        const Module *get_instance_module()      const; ///< Returns the module of which this instance is from (user module only), returns 0 in case of library cell instance
        const char   *get_instance_module_name() const; ///< Returns the module name which this instance is from (user module or libcell)
        const InstInterfaceList &get_ports()     const; ///< Returns the list of port interfaces for that instance
        bool print() const; ///< Prints the content of this object for debugging purposes

        Inst(const char *model,const char *name,InstInterfaceList *ports); ///< @internal
        ~Inst();
    private:
        const char        *_model;
        InstInterfaceList  _ports;
    };

    /// Describes an instance interface. This defines wire assignments of the forms .a(b) and .a({b,c})
    class InstInterface {
    public:
        bool            is_actual_conc()  const; ///< Returns true if actual part is a concatenation of wires (Example: {a,b,c})
        const char     *get_formal()      const; ///< Returns the formal, note that formal is NULL when ports are position mapped
        const Expr     *get_actual_expr() const; ///< Returns the actual expression, returns 0 if the actual expression is a concatenation
        const ExprList *get_actual_conc() const; ///< Returns the actual list of concatenated wires, returns 0 if the actual is an expression
        bool            print()           const; ///< Prints the content of this object for debugging purposes

        InstInterface(const char *name,const Expr     *expr);  ///< @internal
        InstInterface(const char *name,const ExprList *lexpr); ///< @internal
        ~InstInterface();
    private:
        bool        _is_conc;
        const char *_formal;
        union {
            const Expr     *_expr;
            const ExprList *_lexpr;
        };
    };

    /// Describes a verilog module 
    class Module : public Object {
    public:
        const NameList   &get_port_names()    const {return _namelist;}   ///< Returns the list of port names
        const WireList   &get_wire_list()     const {return _wirelist;}   ///< Returns the list of wires in the module
        const AssignList &get_assign_list()   const {return _assignlist;} ///< Returns the list of wires in the module
        const InstList   &get_instance_list() const {return _instlist;}   ///< Returns the list of instances in the module
        const Design     *get_design()        const;                      ///< Returns the design to which this module belongs

        bool print() const; ///< Prints the content of this object for debugging purposes

        Module(const char *name,NameList *nl);  ///< @internal
        ~Module();

        bool add_objects(ObjectList *ol); ///< @internal
        bool add_wire(Wire *w);           ///< @internal
        bool add_assign(Assign *a);       ///< @internal
        bool add_inst(Inst *i);           ///< @internal

    private:
        NameList     _namelist;
        WireList     _wirelist;
        AssignList   _assignlist;
        InstList     _instlist;
    };

    /// This class is a container that stores all the modules that are part of a design
    class Design : public Object {
    public:
        const ModuleList &get_modules() const;                 ///< Returns the list of modules in the design
        const Module     *get_module(const char *name) const;  ///< Finds the module with the name specified as argument
        const NameList    find_top_modules() const;            ///< Heuristically determines the top module(s)

        bool              print() const; ///< Prints the content of this object for debugging purposes

        Design();                                              ///< @internal
        ~Design();
        bool              add_module(Module *m);               ///< @internal

    private:
        struct data;
        data  *_data;
    };
};

#endif
