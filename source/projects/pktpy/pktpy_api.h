// pktpy_api.h

#ifndef PKTPY_API_H
#define PKTPY_API_H

// ----------------------------------------------------------------------------
// includes

// max api
#include "ext.h"
#include "ext_obex.h"

// pocketpy
#include "pocketpy.h"

// Max atomarray
#include "ext_atomarray.h"

// Max dictionary
#include "ext_dictionary.h"

// ----------------------------------------------------------------------------
// custom pktpy2 functions

bool int_add(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_int);
    py_i64 a = py_toint(py_arg(0));
    py_i64 b = py_toint(py_arg(1));
    py_newint(py_retval(), a + b);
    return true;
}

void print_to_console(const char * content)
{
    post(content);
}

t_max_err demo(void) {
    // Hello world!
    bool ok = py_exec("print('Hello world!')", "<string>", EXEC_MODE, NULL);
    if(!ok) goto __ERROR;

    // Create a list: [1, 2, 3]
    py_Ref r0 = py_getreg(0);
    py_newlistn(r0, 3);
    py_newint(py_list_getitem(r0, 0), 1);
    py_newint(py_list_getitem(r0, 1), 2);
    py_newint(py_list_getitem(r0, 2), 3);

    // Eval the sum of the list
    py_Ref f_sum = py_getbuiltin(py_name("sum"));
    py_push(f_sum);
    py_pushnil();
    py_push(r0);
    ok = py_vectorcall(1, 0);
    if(!ok) goto __ERROR;

    post("Sum of the list: %d\n", (int)py_toint(py_retval()));  // 6

    // Bind native `int_add` as a global variable
    py_newnativefunc(r0, int_add);
    py_setglobal(py_name("add"), r0);

    // Call `add` in python
    ok = py_exec("add(3, 7)", "<string>", EVAL_MODE, NULL);
    if(!ok) goto __ERROR;

    py_i64 res = py_toint(py_retval());
    post("Sum of 2 variables: %d\n", (int)res);  // 10

    return MAX_ERR_NONE;

__ERROR:
    py_printexc();
    return MAX_ERR_GENERIC;
}

// ============================================================================
// api module

// ----------------------------------------------------------------------------
// functions


static bool api_post(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* cstr = py_tostr(py_arg(0));
    post(cstr);
    return true;
}

static bool api_error(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* cstr = py_tostr(py_arg(0));
    error(cstr);
    return true;
}

// ----------------------------------------------------------------------------
// Person (demo code)


typedef struct t_person {
    int id;
    int age;
} t_person;

static void t_person__ctor(t_person* self, int id, int age) {
    self->id = id;
    self->age = age;
}

static bool Person__new__(int argc, py_Ref argv) {
    t_person* ptr = py_newobject(py_retval(), py_totype(argv), 0, sizeof(t_person));
    t_person__ctor(ptr, 0, 0); // init both to 0
    return true;
}

static bool Person__init__(int argc, py_Ref argv) {
    post("Person__init__ argc = %d", argc);
    if (argc == 1) {
        // do nothing
    }
    else if(argc == 3) {
        t_person* ptr = py_touserdata(py_arg(0));
        PY_CHECK_ARG_TYPE(1, tp_int);
        py_i64 id = py_toint(py_arg(1));
        PY_CHECK_ARG_TYPE(2, tp_int);
        py_i64 age = py_toint(py_arg(2));
        t_person__ctor(ptr, id, age);
    } else {
        return TypeError("Person__init__(): expected 0 or 2 arguments, got %d", argc - 1);
    }
    py_newnone(py_retval()); // return None
    return true;
}

static bool Person__id(int argc, py_Ref argv) {
    t_person* ptr = py_touserdata(py_arg(0));
    py_newint(py_retval(), ptr->id);
    return true;
}

static bool Person__age(int argc, py_Ref argv) {
    t_person* ptr = py_touserdata(py_arg(0));
    py_newint(py_retval(), ptr->age);
    return true;
}

static bool Person__set_id(int argc, py_Ref argv) {
    t_person* ptr = py_touserdata(py_arg(0));
    if (argc == 2) {
        ptr->id = py_toint(py_arg(1));
        return true;
    }
    return TypeError("Person__set_id(): expected 1 arguments, got %d", argc - 1);
}

static bool Person__set_age(int argc, py_Ref argv) {
    t_person* ptr = py_touserdata(py_arg(0));
    if (argc == 2) {
        ptr->age = py_toint(py_arg(1));
        return true;
    }
    return TypeError("Person__set_age(): expected 1 arguments, got %d", argc - 1);
}



// ----------------------------------------------------------------------------
// Forward declarations for global type variables

// Store type IDs globally so functions can access them
static py_Type g_symbol_type = -1;
static py_Type g_atom_type = -1;
static py_Type g_atomarray_type = -1;
static py_Type g_dictionary_type = -1;

// Forward declarations for utility functions
static bool py_to_atom(py_Ref py_val, t_atom* atom);
static bool atom_to_py(t_atom* atom);

// ----------------------------------------------------------------------------
// Symbol wrapper

typedef struct {
    t_symbol* sym;
} SymbolObject;

static bool Symbol__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    SymbolObject* obj = py_newobject(py_retval(), cls, 0, sizeof(SymbolObject));
    obj->sym = gensym("");  // default empty symbol
    return true;
}

static bool Symbol__init__(int argc, py_Ref argv) {
    SymbolObject* self = py_touserdata(py_arg(0));

    if (argc == 1) {
        // Symbol() - empty symbol
        self->sym = gensym("");
    } else if (argc == 2) {
        // Symbol(str) - from string
        PY_CHECK_ARG_TYPE(1, tp_str);
        const char* str = py_tostr(py_arg(1));
        self->sym = gensym(str);
    } else {
        return TypeError("Symbol() takes 0 or 1 argument, got %d", argc - 1);
    }

    py_newnone(py_retval());
    return true;
}

static bool Symbol__str__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SymbolObject* self = py_touserdata(py_arg(0));
    py_newstr(py_retval(), self->sym->s_name);
    return true;
}

static bool Symbol__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SymbolObject* self = py_touserdata(py_arg(0));
    char buf[256];
    snprintf(buf, sizeof(buf), "Symbol('%s')", self->sym->s_name);
    py_newstr(py_retval(), buf);
    return true;
}

static bool Symbol_get_name(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SymbolObject* self = py_touserdata(py_arg(0));
    py_newstr(py_retval(), self->sym->s_name);
    return true;
}

static bool Symbol__eq__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    SymbolObject* self = py_touserdata(py_arg(0));

    // Compare with another Symbol
    if (py_checktype(py_arg(1), py_totype(py_arg(0)))) {
        SymbolObject* other = py_touserdata(py_arg(1));
        py_newbool(py_retval(), self->sym == other->sym);
        return true;
    }

    // Compare with string
    if (py_isstr(py_arg(1))) {
        const char* other_str = py_tostr(py_arg(1));
        py_newbool(py_retval(), strcmp(self->sym->s_name, other_str) == 0);
        return true;
    }

    py_newbool(py_retval(), false);
    return true;
}

// ----------------------------------------------------------------------------
// Atom wrapper

typedef struct {
    t_atom atom;
} AtomObject;

static bool Atom__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    AtomObject* obj = py_newobject(py_retval(), cls, 0, sizeof(AtomObject));
    atom_setlong(&obj->atom, 0);  // default to long with value 0
    return true;
}

static bool Atom__init__(int argc, py_Ref argv) {
    AtomObject* self = py_touserdata(py_arg(0));

    if (argc == 1) {
        // Atom() - default to long 0
        atom_setlong(&self->atom, 0);
    } else if (argc == 2) {
        py_Ref arg = py_arg(1);

        // Atom(int)
        if (py_isint(arg)) {
            atom_setlong(&self->atom, py_toint(arg));
        }
        // Atom(float)
        else if (py_isfloat(arg)) {
            atom_setfloat(&self->atom, py_tofloat(arg));
        }
        // Atom(str) or Atom(Symbol)
        else if (py_isstr(arg)) {
            atom_setsym(&self->atom, gensym(py_tostr(arg)));
        }
        // Atom(Symbol)
        else if (py_checktype(arg, py_totype(py_arg(1)))) {
            // Check if it's a SymbolObject
            SymbolObject* sym_obj = py_touserdata(arg);
            atom_setsym(&self->atom, sym_obj->sym);
        }
        else {
            return TypeError("Atom() argument must be int, float, str, or Symbol");
        }
    } else {
        return TypeError("Atom() takes 0 or 1 argument, got %d", argc - 1);
    }

    py_newnone(py_retval());
    return true;
}

static bool Atom__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    char buf[256];

    switch (atom_gettype(&self->atom)) {
        case A_LONG:
            snprintf(buf, sizeof(buf), "Atom(%ld)", atom_getlong(&self->atom));
            break;
        case A_FLOAT:
            snprintf(buf, sizeof(buf), "Atom(%f)", atom_getfloat(&self->atom));
            break;
        case A_SYM:
            snprintf(buf, sizeof(buf), "Atom('%s')", atom_getsym(&self->atom)->s_name);
            break;
        default:
            snprintf(buf, sizeof(buf), "Atom(<unknown>)");
            break;
    }

    py_newstr(py_retval(), buf);
    return true;
}

static bool Atom__str__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    t_symbol* sym = atom_getsym(&self->atom);
    py_newstr(py_retval(), sym->s_name);
    return true;
}

static bool Atom__int__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    t_atom_long val = atom_getlong(&self->atom);
    py_newint(py_retval(), val);
    return true;
}

static bool Atom__float__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    t_atom_float val = atom_getfloat(&self->atom);
    py_newfloat(py_retval(), val);
    return true;
}

static bool Atom_get_type(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));

    switch (atom_gettype(&self->atom)) {
        case A_LONG:
            py_newstr(py_retval(), "long");
            break;
        case A_FLOAT:
            py_newstr(py_retval(), "float");
            break;
        case A_SYM:
            py_newstr(py_retval(), "symbol");
            break;
        default:
            py_newstr(py_retval(), "unknown");
            break;
    }
    return true;
}

static bool Atom_get_value(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));

    switch (atom_gettype(&self->atom)) {
        case A_LONG:
            py_newint(py_retval(), atom_getlong(&self->atom));
            break;
        case A_FLOAT:
            py_newfloat(py_retval(), atom_getfloat(&self->atom));
            break;
        case A_SYM:
            py_newstr(py_retval(), atom_getsym(&self->atom)->s_name);
            break;
        default:
            py_newnone(py_retval());
            break;
    }
    return true;
}

static bool Atom_set_value(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    AtomObject* self = py_touserdata(py_arg(0));
    py_Ref value = py_arg(1);

    if (py_isint(value)) {
        atom_setlong(&self->atom, py_toint(value));
    } else if (py_isfloat(value)) {
        atom_setfloat(&self->atom, py_tofloat(value));
    } else if (py_isstr(value)) {
        atom_setsym(&self->atom, gensym(py_tostr(value)));
    } else {
        return TypeError("value must be int, float, or str");
    }

    py_newnone(py_retval());
    return true;
}

static bool Atom_is_long(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), atom_gettype(&self->atom) == A_LONG);
    return true;
}

static bool Atom_is_float(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), atom_gettype(&self->atom) == A_FLOAT);
    return true;
}

static bool Atom_is_symbol(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), atom_gettype(&self->atom) == A_SYM);
    return true;
}

static bool Atom_getlong(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    t_atom_long val = atom_getlong(&self->atom);
    py_newint(py_retval(), val);
    return true;
}

static bool Atom_getfloat(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    t_atom_float val = atom_getfloat(&self->atom);
    py_newfloat(py_retval(), val);
    return true;
}

static bool Atom_getsym(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomObject* self = py_touserdata(py_arg(0));
    t_symbol* sym = atom_getsym(&self->atom);

    // Create and return a SymbolObject
    if (g_symbol_type < 0) {
        return RuntimeError("Symbol type not initialized");
    }

    SymbolObject* obj = py_newobject(py_retval(), g_symbol_type, 0, sizeof(SymbolObject));
    obj->sym = sym;

    return true;
}


// ----------------------------------------------------------------------------
// AtomArray wrapper

typedef struct {
    t_atomarray* atomarray;
    bool owns_atomarray;  // Whether we should free it
} AtomArrayObject;

static bool AtomArray__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    AtomArrayObject* obj = py_newobject(py_retval(), cls, 0, sizeof(AtomArrayObject));
    obj->atomarray = atomarray_new(0, NULL);  // Empty array
    obj->owns_atomarray = true;
    return true;
}

static bool AtomArray__init__(int argc, py_Ref argv) {
    AtomArrayObject* self = py_touserdata(py_arg(0));

    if (argc == 1) {
        // AtomArray() - empty array (already created in __new__)
        return true;
    } else if (argc == 2 && py_isinstance(py_arg(1), tp_list)) {
        // AtomArray([...]) - from Python list
        py_Ref list = py_arg(1);
        int list_len = py_list_len(list);

        // Allocate temporary atom array
        t_atom* atoms = (t_atom*)sysmem_newptr(list_len * sizeof(t_atom));
        if (!atoms) {
            return RuntimeError("Failed to allocate memory for atoms");
        }

        // Convert Python list to atoms
        for (int i = 0; i < list_len; i++) {
            py_Ref item = py_list_getitem(list, i);
            if (!py_to_atom(item, &atoms[i])) {
                sysmem_freeptr(atoms);
                return TypeError("List item %d cannot be converted to atom", i);
            }
        }

        // Set atoms in atomarray
        atomarray_setatoms(self->atomarray, list_len, atoms);
        sysmem_freeptr(atoms);

    } else {
        return TypeError("AtomArray() takes 0 or 1 argument (list)");
    }

    py_newnone(py_retval());
    return true;
}

// Destructor
static void AtomArray__del__(void* self) {
    AtomArrayObject* obj = (AtomArrayObject*)self;
    if (obj->owns_atomarray && obj->atomarray) {
        object_free(obj->atomarray);
        obj->atomarray = NULL;
    }
}

static bool AtomArray__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));

    long ac = atomarray_getsize(self->atomarray);
    char buf[256];
    snprintf(buf, sizeof(buf), "AtomArray(size=%ld)", ac);
    py_newstr(py_retval(), buf);
    return true;
}

static bool AtomArray__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    t_atom_long size = atomarray_getsize(self->atomarray);
    py_newint(py_retval(), size);
    return true;
}

static bool AtomArray__getitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 index = py_toint(py_arg(1));
    long size = atomarray_getsize(self->atomarray);

    // Handle negative indices
    if (index < 0) {
        index += size;
    }

    if (index < 0 || index >= size) {
        return IndexError("AtomArray index out of range");
    }

    // Get atom at index
    t_atom a;
    t_max_err err = atomarray_getindex(self->atomarray, index, &a);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get atom at index %lld", index);
    }

    // Create Atom object wrapper
    if (g_atom_type < 0) {
        return RuntimeError("Atom type not initialized");
    }

    AtomObject* atom_obj = py_newobject(py_retval(), g_atom_type, 0, sizeof(AtomObject));
    atom_obj->atom = a;

    return true;
}

static bool AtomArray__setitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 index = py_toint(py_arg(1));
    long size = atomarray_getsize(self->atomarray);

    // Handle negative indices
    if (index < 0) {
        index += size;
    }

    if (index < 0 || index >= size) {
        return IndexError("AtomArray index out of range");
    }

    // Convert value to atom
    t_atom a;
    if (!py_to_atom(py_arg(2), &a)) {
        return TypeError("Value cannot be converted to atom");
    }

    // atomarray_setindex is not exported in Max SDK
    // We need to rebuild the array to set an item
    long ac = 0;
    t_atom* av = NULL;
    t_max_err err = atomarray_getatoms(self->atomarray, &ac, &av);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get atoms");
    }

    // Modify the atom at index
    av[index] = a;

    // Rebuild atomarray with modified atoms
    err = atomarray_setatoms(self->atomarray, ac, av);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set atoms");
    }

    py_newnone(py_retval());
    return true;
}

// Method: getsize()
static bool AtomArray_getsize(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    t_atom_long size = atomarray_getsize(self->atomarray);
    py_newint(py_retval(), size);
    return true;
}

// Method: append(atom or value)
static bool AtomArray_append(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    AtomArrayObject* self = py_touserdata(py_arg(0));

    t_atom a;
    py_Ref value = py_arg(1);

    // Check if it's an Atom object
    if (g_atom_type >= 0 && py_checktype(value, g_atom_type)) {
        AtomObject* atom_obj = py_touserdata(value);
        a = atom_obj->atom;
    } else if (!py_to_atom(value, &a)) {
        return TypeError("Value cannot be converted to atom");
    }

    atomarray_appendatom(self->atomarray, &a);
    py_newnone(py_retval());
    return true;
}

// Method: clear()
static bool AtomArray_clear(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    atomarray_clear(self->atomarray);
    py_newnone(py_retval());
    return true;
}

// Method: to_list() - Convert to Python list
static bool AtomArray_to_list(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));

    long ac = 0;
    t_atom* av = NULL;

    t_max_err err = atomarray_getatoms(self->atomarray, &ac, &av);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get atoms from atomarray");
    }

    // Create Python list
    py_newlistn(py_retval(), ac);

    for (int i = 0; i < ac; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        if (!atom_to_py(&av[i])) {
            return RuntimeError("Failed to convert atom %d to Python value", i);
        }
        // Copy retval to list item
        py_assign(item, py_retval());
    }

    return true;
}

// Method: duplicate() - Create a copy
static bool AtomArray_duplicate(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));

    t_atomarray* dup = (t_atomarray*)atomarray_duplicate(self->atomarray);
    if (!dup) {
        return RuntimeError("Failed to duplicate atomarray");
    }

    // Create new AtomArrayObject wrapper
    if (g_atomarray_type < 0) {
        return RuntimeError("AtomArray type not initialized");
    }

    AtomArrayObject* new_obj = py_newobject(py_retval(), g_atomarray_type, 0, sizeof(AtomArrayObject));
    new_obj->atomarray = dup;
    new_obj->owns_atomarray = true;

    return true;
}


// ----------------------------------------------------------------------------
// Dictionary wrapper

typedef struct {
    t_dictionary* dict;
    bool owns_dict;  // Whether we should free it
} DictionaryObject;

static bool Dictionary__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    DictionaryObject* obj = py_newobject(py_retval(), cls, 0, sizeof(DictionaryObject));
    obj->dict = dictionary_new();
    obj->owns_dict = true;
    return true;
}

static bool Dictionary__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    // Dict is already created in __new__
    py_newnone(py_retval());
    return true;
}

// Destructor
static void Dictionary__del__(void* self) {
    DictionaryObject* obj = (DictionaryObject*)self;
    if (obj->owns_dict && obj->dict) {
        object_free(obj->dict);
        obj->dict = NULL;
    }
}

static bool Dictionary__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DictionaryObject* self = py_touserdata(py_arg(0));

    t_atom_long count = dictionary_getentrycount(self->dict);
    char buf[256];
    snprintf(buf, sizeof(buf), "Dictionary(entries=%ld)", count);
    py_newstr(py_retval(), buf);
    return true;
}

static bool Dictionary__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DictionaryObject* self = py_touserdata(py_arg(0));
    t_atom_long count = dictionary_getentrycount(self->dict);
    py_newint(py_retval(), count);
    return true;
}

// __getitem__ - dict[key]
static bool Dictionary__getitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    // Try to get as atom first
    t_atom a;
    t_max_err err = dictionary_getatom(self->dict, key, &a);

    if (err == MAX_ERR_NONE) {
        // Successfully got atom, convert to Python
        if (!atom_to_py(&a)) {
            return RuntimeError("Failed to convert atom to Python value");
        }
        return true;
    }

    // Check if it's a string
    if (dictionary_entryisstring(self->dict, key)) {
        const char* value = NULL;
        err = dictionary_getstring(self->dict, key, &value);
        if (err == MAX_ERR_NONE) {
            py_newstr(py_retval(), value);
            return true;
        }
    }

    // Check if it's an atomarray
    if (dictionary_entryisatomarray(self->dict, key)) {
        t_object* atomarray_obj = NULL;
        err = dictionary_getatomarray(self->dict, key, &atomarray_obj);
        if (err == MAX_ERR_NONE && g_atomarray_type >= 0) {
            AtomArrayObject* obj = py_newobject(py_retval(), g_atomarray_type, 0, sizeof(AtomArrayObject));
            obj->atomarray = (t_atomarray*)atomarray_obj;
            obj->owns_atomarray = false;  // Dictionary owns it
            return true;
        }
    }

    // Check if it's a nested dictionary
    if (dictionary_entryisdictionary(self->dict, key)) {
        t_object* sub_dict = NULL;
        err = dictionary_getdictionary(self->dict, key, &sub_dict);
        if (err == MAX_ERR_NONE && g_dictionary_type >= 0) {
            DictionaryObject* obj = py_newobject(py_retval(), g_dictionary_type, 0, sizeof(DictionaryObject));
            obj->dict = (t_dictionary*)sub_dict;
            obj->owns_dict = false;  // Parent dictionary owns it
            return true;
        }
    }

    // Create a string for KeyError
    py_Ref key_ref = py_getreg(0);
    py_newstr(key_ref, key_str);
    return KeyError(key_ref);
}

// __setitem__ - dict[key] = value
static bool Dictionary__setitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);
    py_Ref value = py_arg(2);

    t_max_err err = MAX_ERR_GENERIC;

    // Check type and use appropriate append function
    if (py_isint(value)) {
        err = dictionary_appendlong(self->dict, key, py_toint(value));
    } else if (py_isfloat(value)) {
        err = dictionary_appendfloat(self->dict, key, py_tofloat(value));
    } else if (py_isstr(value)) {
        err = dictionary_appendstring(self->dict, key, py_tostr(value));
    } else if (py_isinstance(value, tp_list)) {
        // Convert list to atoms and append as atomarray
        int list_len = py_list_len(value);
        t_atom* atoms = (t_atom*)sysmem_newptr(list_len * sizeof(t_atom));
        if (!atoms) {
            return RuntimeError("Failed to allocate memory");
        }

        for (int i = 0; i < list_len; i++) {
            py_Ref item = py_list_getitem(value, i);
            if (!py_to_atom(item, &atoms[i])) {
                sysmem_freeptr(atoms);
                return TypeError("List item %d cannot be converted to atom", i);
            }
        }

        err = dictionary_appendatoms(self->dict, key, list_len, atoms);
        sysmem_freeptr(atoms);
    } else if (g_atomarray_type >= 0 && py_checktype(value, g_atomarray_type)) {
        // AtomArray object
        AtomArrayObject* aa_obj = py_touserdata(value);
        err = dictionary_appendatomarray(self->dict, key, (t_object*)aa_obj->atomarray);
        // Note: dictionary now owns the atomarray, so mark it
        aa_obj->owns_atomarray = false;
    } else if (g_dictionary_type >= 0 && py_checktype(value, g_dictionary_type)) {
        // Dictionary object
        DictionaryObject* dict_obj = py_touserdata(value);
        err = dictionary_appenddictionary(self->dict, key, (t_object*)dict_obj->dict);
        // Note: parent dictionary now owns the child
        dict_obj->owns_dict = false;
    } else if (g_atom_type >= 0 && py_checktype(value, g_atom_type)) {
        // Atom object
        AtomObject* atom_obj = py_touserdata(value);
        err = dictionary_appendatom(self->dict, key, &atom_obj->atom);
    } else if (g_symbol_type >= 0 && py_checktype(value, g_symbol_type)) {
        // Symbol object
        SymbolObject* sym_obj = py_touserdata(value);
        err = dictionary_appendsym(self->dict, key, sym_obj->sym);
    } else {
        return TypeError("Unsupported value type for dictionary");
    }

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set dictionary value");
    }

    py_newnone(py_retval());
    return true;
}

// __contains__ - key in dict
static bool Dictionary__contains__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    long has = dictionary_hasentry(self->dict, key);
    py_newbool(py_retval(), has != 0);
    return true;
}

// Method: get(key, default=None)
static bool Dictionary_get(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("get() takes 1 or 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    // Check if key exists
    if (!dictionary_hasentry(self->dict, key)) {
        if (argc == 3) {
            // Return default value
            py_assign(py_retval(), py_arg(2));
        } else {
            // Return None
            py_newnone(py_retval());
        }
        return true;
    }

    // Key exists, use __getitem__ logic
    t_atom a;
    t_max_err err = dictionary_getatom(self->dict, key, &a);

    if (err == MAX_ERR_NONE) {
        if (!atom_to_py(&a)) {
            return RuntimeError("Failed to convert atom to Python value");
        }
        return true;
    }

    // Check other types as in __getitem__
    if (dictionary_entryisstring(self->dict, key)) {
        const char* value = NULL;
        err = dictionary_getstring(self->dict, key, &value);
        if (err == MAX_ERR_NONE) {
            py_newstr(py_retval(), value);
            return true;
        }
    }

    py_newnone(py_retval());
    return true;
}

// Method: keys()
static bool Dictionary_keys(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DictionaryObject* self = py_touserdata(py_arg(0));

    long numkeys = 0;
    t_symbol** keys = NULL;

    t_max_err err = dictionary_getkeys(self->dict, &numkeys, &keys);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get dictionary keys");
    }

    // Create Python list
    py_newlistn(py_retval(), numkeys);

    for (long i = 0; i < numkeys; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        py_newstr(item, keys[i]->s_name);
    }

    if (keys) {
        dictionary_freekeys(self->dict, numkeys, keys);
    }

    return true;
}

// Method: has_key(key)
static bool Dictionary_has_key(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    long has = dictionary_hasentry(self->dict, key);
    py_newbool(py_retval(), has != 0);
    return true;
}

// Method: clear()
static bool Dictionary_clear(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DictionaryObject* self = py_touserdata(py_arg(0));

    t_max_err err = dictionary_clear(self->dict);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to clear dictionary");
    }

    py_newnone(py_retval());
    return true;
}

// Method: delete(key)
static bool Dictionary_delete(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_max_err err = dictionary_deleteentry(self->dict, key);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to delete key '%s'", key_str);
    }

    py_newnone(py_retval());
    return true;
}

// Method: getlong(key, default=0)
static bool Dictionary_getlong(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("getlong() takes 1 or 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_atom_long value = 0;
    t_max_err err;

    if (argc == 3 && py_isint(py_arg(2))) {
        t_atom_long def = py_toint(py_arg(2));
        err = dictionary_getdeflong(self->dict, key, &value, def);
    } else {
        err = dictionary_getlong(self->dict, key, &value);
    }

    if (err != MAX_ERR_NONE && argc < 3) {
        py_Ref key_ref = py_getreg(0);
        py_newstr(key_ref, key_str);
        return KeyError(key_ref);
    }

    py_newint(py_retval(), value);
    return true;
}

// Method: getfloat(key, default=0.0)
static bool Dictionary_getfloat(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("getfloat() takes 1 or 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    double value = 0.0;
    t_max_err err;

    if (argc == 3 && py_isfloat(py_arg(2))) {
        double def = py_tofloat(py_arg(2));
        err = dictionary_getdeffloat(self->dict, key, &value, def);
    } else {
        err = dictionary_getfloat(self->dict, key, &value);
    }

    if (err != MAX_ERR_NONE && argc < 3) {
        py_Ref key_ref = py_getreg(0);
        py_newstr(key_ref, key_str);
        return KeyError(key_ref);
    }

    py_newfloat(py_retval(), value);
    return true;
}

// Method: getstring(key, default="")
static bool Dictionary_getstring(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("getstring() takes 1 or 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    const char* value = NULL;
    t_max_err err = dictionary_getstring(self->dict, key, &value);

    if (err != MAX_ERR_NONE) {
        if (argc == 3 && py_isstr(py_arg(2))) {
            value = py_tostr(py_arg(2));
        } else {
            py_Ref key_ref = py_getreg(0);
            py_newstr(key_ref, key_str);
            return KeyError(key_ref);
        }
    }

    py_newstr(py_retval(), value ? value : "");
    return true;
}

// Method: read(filename, path)
static bool Dictionary_read(int argc, py_Ref argv) {
    if (argc != 3) {
        return TypeError("read() takes 2 arguments: filename and path");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);
    PY_CHECK_ARG_TYPE(2, tp_int);

    const char* filename = py_tostr(py_arg(1));
    short path = (short)py_toint(py_arg(2));

    // Clear existing dictionary and read new one
    if (self->owns_dict && self->dict) {
        object_free(self->dict);
    }

    t_dictionary* new_dict = NULL;
    t_max_err err = dictionary_read(filename, path, &new_dict);

    if (err != MAX_ERR_NONE || !new_dict) {
        return RuntimeError("Failed to read dictionary from file '%s'", filename);
    }

    self->dict = new_dict;
    self->owns_dict = true;

    py_newnone(py_retval());
    return true;
}

// Method: write(filename, path)
static bool Dictionary_write(int argc, py_Ref argv) {
    if (argc != 3) {
        return TypeError("write() takes 2 arguments: filename and path");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);
    PY_CHECK_ARG_TYPE(2, tp_int);

    const char* filename = py_tostr(py_arg(1));
    short path = (short)py_toint(py_arg(2));

    t_max_err err = dictionary_write(self->dict, filename, path);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to write dictionary to file '%s'", filename);
    }

    py_newnone(py_retval());
    return true;
}

// Method: dump(recurse=True, console=False)
static bool Dictionary_dump(int argc, py_Ref argv) {
    if (argc < 1 || argc > 3) {
        return TypeError("dump() takes 0 to 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));

    long recurse = 1;
    long console = 0;

    if (argc >= 2) {
        recurse = py_isint(py_arg(1)) ? py_toint(py_arg(1)) : 1;
    }
    if (argc >= 3) {
        console = py_isint(py_arg(2)) ? py_toint(py_arg(2)) : 0;
    }

    t_max_err err = dictionary_dump(self->dict, recurse, console);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to dump dictionary");
    }

    py_newnone(py_retval());
    return true;
}


// ----------------------------------------------------------------------------
// Utility Functions - Python to Max conversions

// Convert Python value to Atom
static bool py_to_atom(py_Ref py_val, t_atom* atom) {
    if (py_isint(py_val)) {
        atom_setlong(atom, py_toint(py_val));
        return true;
    } else if (py_isfloat(py_val)) {
        atom_setfloat(atom, py_tofloat(py_val));
        return true;
    } else if (py_isstr(py_val)) {
        atom_setsym(atom, gensym(py_tostr(py_val)));
        return true;
    }
    return false;
}

// Convert Atom to Python value (returns via py_retval)
static bool atom_to_py(t_atom* atom) {
    switch (atom_gettype(atom)) {
        case A_LONG:
            py_newint(py_retval(), atom_getlong(atom));
            return true;
        case A_FLOAT:
            py_newfloat(py_retval(), atom_getfloat(atom));
            return true;
        case A_SYM:
            py_newstr(py_retval(), atom_getsym(atom)->s_name);
            return true;
        default:
            return false;
    }
}

// ----------------------------------------------------------------------------
// Max API Function Wrappers

static bool api_gensym(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    const char* str = py_tostr(py_arg(0));
    t_symbol* sym = gensym(str);

    // Create and return a SymbolObject using cached type
    if (g_symbol_type < 0) {
        return RuntimeError("Symbol type not initialized");
    }

    SymbolObject* obj = py_newobject(py_retval(), g_symbol_type, 0, sizeof(SymbolObject));
    obj->sym = sym;

    return true;
}

// Module-level atom conversion functions that take Atom objects
static bool api_atom_getlong(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);

    // Check if argument is an Atom
    if (g_atom_type < 0 || !py_checktype(py_arg(0), g_atom_type)) {
        return TypeError("atom_getlong() requires an Atom object");
    }

    AtomObject* atom_obj = py_touserdata(py_arg(0));
    t_atom_long val = atom_getlong(&atom_obj->atom);
    py_newint(py_retval(), val);
    return true;
}

static bool api_atom_getfloat(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);

    // Check if argument is an Atom
    if (g_atom_type < 0 || !py_checktype(py_arg(0), g_atom_type)) {
        return TypeError("atom_getfloat() requires an Atom object");
    }

    AtomObject* atom_obj = py_touserdata(py_arg(0));
    t_atom_float val = atom_getfloat(&atom_obj->atom);
    py_newfloat(py_retval(), val);
    return true;
}

static bool api_atom_getsym(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);

    // Check if argument is an Atom
    if (g_atom_type < 0 || !py_checktype(py_arg(0), g_atom_type)) {
        return TypeError("atom_getsym() requires an Atom object");
    }

    AtomObject* atom_obj = py_touserdata(py_arg(0));
    t_symbol* sym = atom_getsym(&atom_obj->atom);

    // Create and return a SymbolObject
    if (g_symbol_type < 0) {
        return RuntimeError("Symbol type not initialized");
    }

    SymbolObject* obj = py_newobject(py_retval(), g_symbol_type, 0, sizeof(SymbolObject));
    obj->sym = sym;

    return true;
}

// ----------------------------------------------------------------------------
// utils

static bool print_args(int argc, py_Ref argv) {
    py_ObjectRef tuple_item;
    py_i64 long_item;
    py_TValue* args = py_tuple_data(argv);
    int tuple_len = py_tuple_len(argv);
    post("tuple_len: %d", tuple_len);
    PY_CHECK_ARGC(1); // 1 arg: `*args` tuple
    PY_CHECK_ARG_TYPE(0, tp_tuple);
    for(int i = 0; i < tuple_len; i++) {
        tuple_item = py_tuple_getitem(py_arg(0), i);
        long_item = py_toint(tuple_item);
        post("%d: %d", i, (int)long_item);
    }
    py_newnone(py_retval());
    return true;
}



// ----------------------------------------------------------------------------
// initialize

bool api_module_initialize(void) {
    py_GlobalRef mod = py_newmodule("api");

    // Basic Max console functions
    py_bindfunc(mod, "post", api_post);
    py_bindfunc(mod, "error", api_error);

    // Utility functions
    py_bind(mod, "print_args(*args)", print_args);

    // Symbol type
    g_symbol_type = py_newtype("Symbol", tp_object, mod, NULL);
    py_bindmethod(g_symbol_type, "__new__", Symbol__new__);
    py_bindmethod(g_symbol_type, "__init__", Symbol__init__);
    py_bindmethod(g_symbol_type, "__str__", Symbol__str__);
    py_bindmethod(g_symbol_type, "__repr__", Symbol__repr__);
    py_bindmethod(g_symbol_type, "__eq__", Symbol__eq__);
    py_bindproperty(g_symbol_type, "name", Symbol_get_name, NULL);

    // Now bind gensym after Symbol type is created
    py_bindfunc(mod, "gensym", api_gensym);

    // Atom type
    g_atom_type = py_newtype("Atom", tp_object, mod, NULL);
    py_bindmethod(g_atom_type, "__new__", Atom__new__);
    py_bindmethod(g_atom_type, "__init__", Atom__init__);
    py_bindmethod(g_atom_type, "__repr__", Atom__repr__);
    py_bindmethod(g_atom_type, "__str__", Atom__str__);
    py_bindmethod(g_atom_type, "__int__", Atom__int__);
    py_bindmethod(g_atom_type, "__float__", Atom__float__);
    py_bindproperty(g_atom_type, "type", Atom_get_type, NULL);
    py_bindproperty(g_atom_type, "value", Atom_get_value, Atom_set_value);
    py_bindmethod(g_atom_type, "is_long", Atom_is_long);
    py_bindmethod(g_atom_type, "is_float", Atom_is_float);
    py_bindmethod(g_atom_type, "is_symbol", Atom_is_symbol);
    // Max atom conversion methods
    py_bindmethod(g_atom_type, "getlong", Atom_getlong);
    py_bindmethod(g_atom_type, "getfloat", Atom_getfloat);
    py_bindmethod(g_atom_type, "getsym", Atom_getsym);

    // Max atom conversion functions (module-level)
    py_bindfunc(mod, "atom_getlong", api_atom_getlong);
    py_bindfunc(mod, "atom_getfloat", api_atom_getfloat);
    py_bindfunc(mod, "atom_getsym", api_atom_getsym);

    // AtomArray type
    g_atomarray_type = py_newtype("AtomArray", tp_object, mod, (py_Dtor)AtomArray__del__);
    py_bindmethod(g_atomarray_type, "__new__", AtomArray__new__);
    py_bindmethod(g_atomarray_type, "__init__", AtomArray__init__);
    py_bindmethod(g_atomarray_type, "__repr__", AtomArray__repr__);
    py_bindmethod(g_atomarray_type, "__len__", AtomArray__len__);
    py_bindmethod(g_atomarray_type, "__getitem__", AtomArray__getitem__);
    py_bindmethod(g_atomarray_type, "__setitem__", AtomArray__setitem__);
    py_bindmethod(g_atomarray_type, "getsize", AtomArray_getsize);
    py_bindmethod(g_atomarray_type, "append", AtomArray_append);
    py_bindmethod(g_atomarray_type, "clear", AtomArray_clear);
    py_bindmethod(g_atomarray_type, "to_list", AtomArray_to_list);
    py_bindmethod(g_atomarray_type, "duplicate", AtomArray_duplicate);

    // Dictionary type
    g_dictionary_type = py_newtype("Dictionary", tp_object, mod, (py_Dtor)Dictionary__del__);
    py_bindmethod(g_dictionary_type, "__new__", Dictionary__new__);
    py_bindmethod(g_dictionary_type, "__init__", Dictionary__init__);
    py_bindmethod(g_dictionary_type, "__repr__", Dictionary__repr__);
    py_bindmethod(g_dictionary_type, "__len__", Dictionary__len__);
    py_bindmethod(g_dictionary_type, "__getitem__", Dictionary__getitem__);
    py_bindmethod(g_dictionary_type, "__setitem__", Dictionary__setitem__);
    py_bindmethod(g_dictionary_type, "__contains__", Dictionary__contains__);
    py_bindmethod(g_dictionary_type, "get", Dictionary_get);
    py_bindmethod(g_dictionary_type, "keys", Dictionary_keys);
    py_bindmethod(g_dictionary_type, "has_key", Dictionary_has_key);
    py_bindmethod(g_dictionary_type, "clear", Dictionary_clear);
    py_bindmethod(g_dictionary_type, "delete", Dictionary_delete);
    py_bindmethod(g_dictionary_type, "getlong", Dictionary_getlong);
    py_bindmethod(g_dictionary_type, "getfloat", Dictionary_getfloat);
    py_bindmethod(g_dictionary_type, "getstring", Dictionary_getstring);
    py_bindmethod(g_dictionary_type, "read", Dictionary_read);
    py_bindmethod(g_dictionary_type, "write", Dictionary_write);
    py_bindmethod(g_dictionary_type, "dump", Dictionary_dump);

    // Person type (demo code)
    py_Type person_type = py_newtype("Person", tp_object, mod, NULL);
    py_bindmethod(person_type, "__new__", Person__new__);
    py_bindmethod(person_type, "__init__", Person__init__);
    py_bindproperty(person_type, "id", Person__id, Person__set_id);
    py_bindproperty(person_type, "age", Person__age, Person__set_age);

    return true;
}




// ----------------------------------------------------------------------------

#endif // PKTPY_API_H
