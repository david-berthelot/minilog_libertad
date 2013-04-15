/// @mainpage  Libertad: a fast lightweight .LIB parser
/// @author    David Berthelot
/// @version   1.0
/// @date      2008
/// @attention This code is under MIT licence

#ifndef DLIB_OBJECTS_H
#define DLIB_OBJECTS_H

#include <list>
#include <vector>

/// DLIB is the namespace that contains the .LIB parser. General API information follows
/** The following rules apply in all of the API calls in this library
 *    -# Pointers returned by APIs must not be freed (unless explicitely stated otherwise in the API documentation).
 *    -# The .LIB library is modelized as nested groups (see class Group, a group is section that has a body enclosed between { })
 *      - The groups can contains other groups (referred to as subgroups), attributes (class Attr) and/or arguments (class Arg)
 *  The use of this simple decomposition allows for a small API which is easier to manipulate.
 */
namespace DLIB {
    using namespace std;

    class Object;
    class Attr;
    class Group;
    class Arg;
    class Expr;
    class BitExpr;

    /// This is the main parsing function
    /** @param filename is the path to the filename to be parsed
        @return a pair which contains the status (bool) and the top level group (typically the library).
        @attention the returned Group pointer must be freed to release the memory when you're finished using it
    */
    pair<bool,Group*>   parse_lib_file(const char *filename);
    /// Parses a string expression such as "(!(A B) | (C ^ D')'))"
    /** @param char buffer containing the expression string to be parsed
        @return a pair which contains the status (bool) and the resulting expression.
        @attention the returned expression pointer must be freed to release the memory when you're finished using it
    */
    pair<bool,Expr*>    parse_expression_string(const char *expr);

    /// This class contains a list of groups.
    /** It inherits the methods of the STL list class, so simply use STL iterators if you want to iterate through all the objects in this list */
    class GroupList : public list<Group*> {
    public:
        GroupList(){}  ///< @internal
        ~GroupList(){}

    private:
        friend class Group;
        const vector<const Group*> find_groups(const char *name) const; 
    };

    /// This class contains a list of attributes.
    /** It inherits the methods of the STL list class, so simply use STL iterators if you want to iterate through all the objects in this list */
    class AttrList : public list<Attr*> {
    public:
        AttrList(){}  ///< @internal
        ~AttrList(){}

    private:
        friend class Group;
        const Attr *find_attr(const char *name) const;
    };

    /// This class contains a list of arguments.
    /** It inherits the methods of the STL list class, so simply use STL iterators if you want to iterate through all the objects in this list */
    class ArgList : public list<Arg*> {
    public:
        ArgList(){}  ///< @internal
        ~ArgList(){}

    private:
        friend class Group;
        const Arg *get_unique_arg() const;
    };

    /// This class is the base class for named objects
    class Object {
    public:
        /// Returns the name of the object
        const char *get_name() const;

        Object(const char *name);  ///< @internal
        virtual ~Object();
    private:
        const char *_name;
    };

    /// This class represents an argument
    /** Arguments appear in group instanciation (see the arglist in the Group class) and in attributes definitions (see Attr class).
        \sa Group, Attr
    */
    class Arg {
    public:
        /// The type of the argument
        enum T_Type {T_UNKNOWN, ///< This is the type used to catch errors
                     T_NUMBER,  ///< The argument is a floating number (example: -1.23e-04)
                     T_KEYWORD, ///< The argument is a keyword (e.g. some text not included between double quotes). Example: table_lookup
                     T_TEXT,    ///< The argument is some double quoted text (example: "1mV")
                     T_COMPLEX, ///< The argument is a list of arguments (example: (1,pf))
                     T_EXPR,    ///< The argument is a simple expression (example: VDD + 0.5)
                     T_BIT_EXPR ///< The argument is a simple expression (example: bus[0] or bus[2:3])
        };
        /// Returns the type of the argument
        T_Type               get_type()      const;
        /// For a number argument, returns the float value of a number argument (else returns 0)
        const float          get_number()    const;
        /// For a keyword argument, returns the char string of a keyword argument (else returns 0)
        const char          *get_keyword()   const;
        /// For a text argument, returns the char string of the text argument with double quotes removed (else returns 0)
        const char          *get_text()      const;
        /// For a text argument, returns the vector of float represented by the text argument, only use this when the argument is the format "float, float, ..., float". An empty array is returned if invoked with a wrong argument type.
        const vector<float>  get_text_as_vector() const;
        /// For an argument list argument, returns the argument list (else returns 0)
        const ArgList       *get_complex()   const;
        /// For an expresson argument, returns the expression (else returns 0)
        const Expr          *get_expr()      const;
        /// For an bit expresson argument, returns the bit expression (else returns 0)
        const BitExpr       *get_bit_expr()      const;

        Arg(const char *name,const T_Type t);  ///< @internal
        Arg(const float number);               ///< @internal
        Arg(const ArgList *args);              ///< @internal
        Arg(const Expr    *expr);              ///< @internal
        Arg(const BitExpr *bitexpr);           ///< @internal
        virtual ~Arg();
    private:
        const T_Type _t;
        const union {
            const char    *_str;
            float          _number;
            const ArgList *_complex;
            const Expr    *_expr;
            const BitExpr *_bitexpr;
        };
    };

    /// Represents a simple expression
    /** Expressions are of the form @code
a op b @endcode
    */
    class Expr {
    public:
        /// The type of the expression
        enum T_Type {T_UNKNWON, ///< This is the type used to catch errors
                     T_OR,      ///< For '|' expression
                     T_XOR,     ///< For '^' expression
                     T_AND,     ///< For '& ' expression
                     T_NOT,     ///< For '!'' expression
                     T_BUF,     ///< Just a buffer (identity)
                     T_PLUS,    ///< For '+' expression
                     T_MINUS,   ///< For '-' expression
                     T_MULT,    ///< For '*' expression
                     T_DIV      ///< For '/' expression
        };
        /// Returns the type of the expression
        T_Type      get_type()        const;
        /// Returns true if the first member of the expression is itself an expression
        bool        is_first_expr()   const;
        /// Returns the first member of the expression as an argument
        /** @return 0 if the first member of the expression is not an argument, otherwise returns the first argument */
        const Arg  *get_first_arg()   const;
        /// Returns the first member of the expression as an expression
        /** @return 0 if the first member of the expression is not an expression, otherwise returns the first expression */
        const Expr *get_first_expr()  const;
        bool        is_second_expr()  const;
        /// Returns the second member of the expression as an argument
        /** @return 0 if the second member of the expression is not an argument, otherwise returns the second argument */
        const Arg  *get_second_arg()  const;
        /// Returns the second member of the expression as an expression
        /** @return 0 if the second member of the expression is not an expression, otherwise returns the second expression */
        const Expr *get_second_expr() const;

        Expr(const Arg  *a,const T_Type op,const Arg  *b);  ///< @internal
        Expr(const Expr *a,const T_Type op,const Arg  *b);  ///< @internal
        Expr(const Expr *a,const T_Type op,const Expr *b);  ///< @internal
        ~Expr();
    private:
        const T_Type _t;
        const bool   _isaArg,_isbArg;
        union {
            const Arg   *_aa;
            const Expr  *_ae;
        };
        union {
            const Arg   *_ba;
            const Expr  *_be;
        };
    };

    /// Represents a bit expression
    /** Expressions are of the form @code
a[3] @endcode or @code
b[4:5] @encode
    */
    class BitExpr : public Object {
    public:
        /// The type of the expression
        enum T_Type {T_UNKNWON,   ///< This is the type used to catch errors
                     T_INDEX,     ///< For a[1] expression
                     T_SLICE      ///< For a[1:3] expression
        };
        /// Returns the type of the expression
        T_Type      get_type()       const;
        /// Returns the index of the selection, -1 if the bit expression is not an index expression
        int         get_index()      const;
        /// Returns the first part of a slice selection, -1 if the bit expression is not a slice expression
        int         get_from()       const;
        /// Returns the second part of a slice selection, -1 if the bit expression is not a slice expression
        int         get_to()         const;

        BitExpr(const char *name,const int index);              ///< @internal
        BitExpr(const char *name,const int from,const int to);  ///< @internal
        ~BitExpr();
    private:
        const T_Type _t;
        const union {
            int _index;
            int _from;
        };
        const int _to;
    };

    /// Represents attributes, refer to classes Object and Arg to find it's inherited methods
    /** Attributes are either of the form: @code
attribute_name : attribute_value; @endcode
        or of the form: @code
attribute_name (argument, ..., argument); @endcode
    */
    class Attr : public Object,public Arg {
    public:
        Attr(const char *name,const char *str,const Arg::T_Type t);  ///< @internal
        Attr(const char *name,const float number);                   ///< @internal
        Attr(const char *name,const ArgList *args);                  ///< @internal
        Attr(const char *name,const Expr    *expr);                  ///< @internal
        Attr(const char *name,const BitExpr *bit_expr);              ///< @internal
        ~Attr();
    };

    /// This class stores the .LIB groups informations
    /** Groups are of the form: @code
group_name (arglist) {
    group_body
} @endcode
    */
    class Group : public Object {
    public:
        /// Returns all arguments of the group
        const ArgList   *get_args()      const;

        /// Returns all the subgroups of the group
        const GroupList *get_subgroups() const;

        /// Returns all the attributes of the group
        const AttrList  *get_attrs()     const;

        /// Finds a single occuring attribute in the list (which typically is the case)
        /** Example: @code cell_group->find("area") @endcode 
                     returns the area attribute in the cell_group (cell_group being a pointer to some cell group)
            @param name to be matched, it should be a plain text string (not a regular expression).
            @return 0 if no attribute is found, otherwise the attribute is returned
        */ 
        const Attr      *find_attr(const char *name) const;

        /// Returns the single argument of a group
        /** Example: @code cell_group->get_unique_arg() @endcode 
                     returns the single argument representing the name of the cell_group 
                     (cell_group being a pointer to some cell group)
            @return 0 if there's 0 or more than 1 arguments, otherwise the sigle argument is returned
        */ 
        const Arg       *get_unique_arg()            const;

        /// Retrieves the groups that match the name argument, note name should be a plain text string (not a regular expression).
        /** Example: @code library_group->find_groups("cell") @endcode 
                     returns all the cell groups in your library (library_group being a pointer to the top level group)
        */
        const vector<const Group*> find_groups(const char *name) const;

        Group(const char *name,const ArgList *args,list<Object*> *objs);  ///< @internal
        ~Group();
    private:
        const ArgList   *_args;
        AttrList         _attrs;
        GroupList        _groups;
    };
};

#endif
