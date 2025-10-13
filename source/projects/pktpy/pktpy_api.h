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

// Max patcher
#include "jpatcher_api.h"

// Max hashtab
#include "ext_hashtab.h"

// Max linklist
#include "ext_linklist.h"

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
static py_Type g_object_type = -1;
static py_Type g_patcher_type = -1;
static py_Type g_box_type = -1;
static py_Type g_hashtab_type = -1;
static py_Type g_linklist_type = -1;

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
// Object wrapper - Generic Max object wrapper

typedef struct {
    t_object* obj;
    bool owns_obj;  // Whether we should free it
} MaxObject;

static bool Object__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    MaxObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(MaxObject));
    wrapper->obj = NULL;
    wrapper->owns_obj = false;
    return true;
}

static bool Object__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    // Object is created via create() or wrap() methods, not __init__
    py_newnone(py_retval());
    return true;
}

// Destructor
static void Object__del__(void* self) {
    MaxObject* wrapper = (MaxObject*)self;
    if (wrapper->owns_obj && wrapper->obj) {
        object_free(wrapper->obj);
        wrapper->obj = NULL;
    }
}

static bool Object__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->obj) {
        t_symbol* classname = object_classname(self->obj);
        snprintf(buf, sizeof(buf), "Object(%s, %p)", classname->s_name, self->obj);
    } else {
        snprintf(buf, sizeof(buf), "Object(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Class method: create(classname, *args)
static bool Object_create(int argc, py_Ref argv) {
    if (argc < 2) {
        return TypeError("create() requires at least 1 argument (classname)");
    }

    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* classname_str = py_tostr(py_arg(1));
    t_symbol* classname = gensym(classname_str);

    // Convert remaining Python args to atoms
    int num_args = argc - 2;
    t_atom* atoms = NULL;

    if (num_args > 0) {
        atoms = (t_atom*)sysmem_newptr(num_args * sizeof(t_atom));
        if (!atoms) {
            return RuntimeError("Failed to allocate memory for arguments");
        }

        for (int i = 0; i < num_args; i++) {
            py_Ref arg = py_arg(i + 2);
            if (!py_to_atom(arg, &atoms[i])) {
                sysmem_freeptr(atoms);
                return TypeError("Argument %d cannot be converted to atom", i);
            }
        }
    }

    // Create object
    t_object* obj = (t_object*)object_new_typed(CLASS_BOX, classname, num_args, atoms);

    if (atoms) {
        sysmem_freeptr(atoms);
    }

    if (!obj) {
        return RuntimeError("Failed to create object of class '%s'", classname_str);
    }

    // Free old object if we owned it
    if (self->owns_obj && self->obj) {
        object_free(self->obj);
    }

    self->obj = obj;
    self->owns_obj = true;

    py_newnone(py_retval());
    return true;
}

// Method: wrap(pointer) - Wrap existing object pointer (no ownership)
static bool Object_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    // Free old object if we owned it
    if (self->owns_obj && self->obj) {
        object_free(self->obj);
    }

    self->obj = (t_object*)ptr;
    self->owns_obj = false;  // Don't free wrapped objects

    py_newnone(py_retval());
    return true;
}

// Method: free()
static bool Object_free_method(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    if (self->owns_obj && self->obj) {
        object_free(self->obj);
        self->obj = NULL;
        self->owns_obj = false;
    }

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Object_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->obj == NULL);
    return true;
}

// Method: classname()
static bool Object_classname(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    t_symbol* classname = object_classname(self->obj);
    py_newstr(py_retval(), classname->s_name);
    return true;
}

// Method: method(name, *args)
static bool Object_method(int argc, py_Ref argv) {
    if (argc < 2) {
        return TypeError("method() requires at least 1 argument (method name)");
    }

    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    const char* method_name = py_tostr(py_arg(1));
    t_symbol* method_sym = gensym(method_name);

    // Convert remaining Python args to atoms
    int num_args = argc - 2;
    t_atom* atoms = NULL;
    t_atom result;

    if (num_args > 0) {
        atoms = (t_atom*)sysmem_newptr(num_args * sizeof(t_atom));
        if (!atoms) {
            return RuntimeError("Failed to allocate memory for arguments");
        }

        for (int i = 0; i < num_args; i++) {
            py_Ref arg = py_arg(i + 2);
            if (!py_to_atom(arg, &atoms[i])) {
                sysmem_freeptr(atoms);
                return TypeError("Argument %d cannot be converted to atom", i);
            }
        }
    }

    // Call method
    atom_setsym(&result, gensym(""));  // Initialize result
    t_max_err err = object_method_typed(self->obj, method_sym, num_args, atoms, &result);

    if (atoms) {
        sysmem_freeptr(atoms);
    }

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Method '%s' failed with error %d", method_name, (int)err);
    }

    // Convert result to Python
    if (!atom_to_py(&result)) {
        py_newnone(py_retval());
    }

    return true;
}

// Method: getattr(name)
static bool Object_getattr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    const char* attr_name = py_tostr(py_arg(1));
    t_symbol* attr_sym = gensym(attr_name);

    long ac = 0;
    t_atom* av = NULL;

    t_max_err err = object_attr_getvalueof(self->obj, attr_sym, &ac, &av);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get attribute '%s'", attr_name);
    }

    if (ac == 0 || !av) {
        py_newnone(py_retval());
        return true;
    }

    // If single value, return it directly
    if (ac == 1) {
        if (!atom_to_py(&av[0])) {
            py_newnone(py_retval());
        }
    } else {
        // Multiple values, return as list
        py_newlistn(py_retval(), ac);
        for (long i = 0; i < ac; i++) {
            py_Ref item = py_list_getitem(py_retval(), i);
            if (!atom_to_py(&av[i])) {
                py_newnone(item);
            } else {
                py_assign(item, py_retval());
            }
        }
    }

    return true;
}

// Method: setattr(name, value)
static bool Object_setattr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    const char* attr_name = py_tostr(py_arg(1));
    t_symbol* attr_sym = gensym(attr_name);
    py_Ref value = py_arg(2);

    t_max_err err = MAX_ERR_GENERIC;

    // Handle different value types
    if (py_isint(value)) {
        err = object_attr_setlong(self->obj, attr_sym, py_toint(value));
    } else if (py_isfloat(value)) {
        err = object_attr_setfloat(self->obj, attr_sym, py_tofloat(value));
    } else if (py_isstr(value)) {
        err = object_attr_setsym(self->obj, attr_sym, gensym(py_tostr(value)));
    } else if (py_isinstance(value, tp_list)) {
        // Convert list to atoms
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

        err = object_attr_setvalueof(self->obj, attr_sym, list_len, atoms);
        sysmem_freeptr(atoms);
    } else {
        return TypeError("Unsupported value type for attribute");
    }

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set attribute '%s'", attr_name);
    }

    py_newnone(py_retval());
    return true;
}

// Method: attrnames()
static bool Object_attrnames(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    long numattrs = 0;
    t_symbol** attrnames = NULL;

    t_max_err err = object_attr_getnames(self->obj, &numattrs, &attrnames);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get attribute names");
    }

    // Create Python list
    py_newlistn(py_retval(), numattrs);

    for (long i = 0; i < numattrs; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        py_newstr(item, attrnames[i]->s_name);
    }

    // Free the array (if needed - check Max API docs)
    if (attrnames) {
        sysmem_freeptr(attrnames);
    }

    return true;
}

// Method: pointer() - Get raw pointer value
static bool Object_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->obj;
    py_newint(py_retval(), ptr);
    return true;
}


// ----------------------------------------------------------------------------
// Box wrapper - Wrapper for Max patcher box objects (t_jbox / t_object)

typedef struct {
    t_object* box;
    bool owns_box;
} BoxObject;

static bool Box__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    BoxObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(BoxObject));
    wrapper->box = NULL;
    wrapper->owns_box = false;
    return true;
}

static bool Box__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Box__del__(void* self) {
    BoxObject* wrapper = (BoxObject*)self;
    // Boxes are owned by the patcher, so we don't free them
    wrapper->box = NULL;
}

static bool Box__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->box) {
        t_symbol* classname = object_classname(jbox_get_object(self->box));
        snprintf(buf, sizeof(buf), "Box(%s, %p)", classname->s_name, self->box);
    } else {
        snprintf(buf, sizeof(buf), "Box(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Method: wrap(pointer)
static bool Box_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    BoxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    self->box = (t_object*)ptr;
    self->owns_box = false;

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Box_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->box == NULL);
    return true;
}

// Method: classname()
static bool Box_classname(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    if (!self->box) {
        return RuntimeError("Box is null");
    }

    t_object* obj = jbox_get_object(self->box);
    t_symbol* classname = object_classname(obj);
    py_newstr(py_retval(), classname->s_name);
    return true;
}

// Method: get_object() - Get underlying Max object
static bool Box_get_object(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    if (!self->box) {
        return RuntimeError("Box is null");
    }

    t_object* obj = jbox_get_object(self->box);

    if (g_object_type < 0) {
        return RuntimeError("Object type not initialized");
    }

    MaxObject* wrapper = py_newobject(py_retval(), g_object_type, 0, sizeof(MaxObject));
    wrapper->obj = obj;
    wrapper->owns_obj = false;  // Box owns the object

    return true;
}

// Method: get_rect() - Get box rectangle
static bool Box_get_rect(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    if (!self->box) {
        return RuntimeError("Box is null");
    }

    t_rect rect;
    t_max_err err = jbox_get_rect_for_view(self->box, NULL, &rect);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get box rectangle");
    }

    // Return as dict: {x, y, width, height}
    py_Ref result = py_retval();
    py_newlistn(result, 4);
    py_newfloat(py_list_getitem(result, 0), rect.x);
    py_newfloat(py_list_getitem(result, 1), rect.y);
    py_newfloat(py_list_getitem(result, 2), rect.width);
    py_newfloat(py_list_getitem(result, 3), rect.height);

    return true;
}

// Method: set_rect(x, y, width, height)
static bool Box_set_rect(int argc, py_Ref argv) {
    if (argc != 5) {
        return TypeError("set_rect() takes 4 arguments (x, y, width, height)");
    }

    BoxObject* self = py_touserdata(py_arg(0));

    if (!self->box) {
        return RuntimeError("Box is null");
    }

    t_rect rect;
    rect.x = py_tofloat(py_arg(1));
    rect.y = py_tofloat(py_arg(2));
    rect.width = py_tofloat(py_arg(3));
    rect.height = py_tofloat(py_arg(4));

    t_max_err err = jbox_set_rect_for_view(self->box, NULL, &rect);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set box rectangle");
    }

    py_newnone(py_retval());
    return true;
}

// Method: pointer()
static bool Box_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->box;
    py_newint(py_retval(), ptr);
    return true;
}


// ----------------------------------------------------------------------------
// Patcher wrapper - Wrapper for Max patcher objects

typedef struct {
    t_object* patcher;
    bool owns_patcher;
} PatcherObject;

static bool Patcher__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    PatcherObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(PatcherObject));
    wrapper->patcher = NULL;
    wrapper->owns_patcher = false;
    return true;
}

static bool Patcher__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Patcher__del__(void* self) {
    PatcherObject* wrapper = (PatcherObject*)self;
    // Patchers are typically not owned by Python wrappers
    wrapper->patcher = NULL;
}

static bool Patcher__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->patcher) {
        snprintf(buf, sizeof(buf), "Patcher(%p)", self->patcher);
    } else {
        snprintf(buf, sizeof(buf), "Patcher(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Method: wrap(pointer)
static bool Patcher_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PatcherObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    self->patcher = (t_object*)ptr;
    self->owns_patcher = false;

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Patcher_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->patcher == NULL);
    return true;
}

// Method: get_firstobject()
static bool Patcher_get_firstobject(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_object* box = jpatcher_get_firstobject(self->patcher);

    if (!box) {
        py_newnone(py_retval());
        return true;
    }

    if (g_box_type < 0) {
        return RuntimeError("Box type not initialized");
    }

    BoxObject* wrapper = py_newobject(py_retval(), g_box_type, 0, sizeof(BoxObject));
    wrapper->box = box;
    wrapper->owns_box = false;

    return true;
}

// Method: get_lastobject()
static bool Patcher_get_lastobject(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_object* box = jpatcher_get_lastobject(self->patcher);

    if (!box) {
        py_newnone(py_retval());
        return true;
    }

    if (g_box_type < 0) {
        return RuntimeError("Box type not initialized");
    }

    BoxObject* wrapper = py_newobject(py_retval(), g_box_type, 0, sizeof(BoxObject));
    wrapper->box = box;
    wrapper->owns_box = false;

    return true;
}

// Method: newobject(text) - Create new object from text
static bool Patcher_newobject(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PatcherObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    const char* text = py_tostr(py_arg(1));
    t_object* box = newobject_fromboxtext(self->patcher, text);

    if (!box) {
        return RuntimeError("Failed to create object from text: '%s'", text);
    }

    if (g_box_type < 0) {
        return RuntimeError("Box type not initialized");
    }

    BoxObject* wrapper = py_newobject(py_retval(), g_box_type, 0, sizeof(BoxObject));
    wrapper->box = box;
    wrapper->owns_box = false;

    return true;
}

// Method: deleteobj(box) - Delete object from patcher
static bool Patcher_deleteobj(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    // Check if argument is a Box object
    if (g_box_type >= 0 && py_checktype(py_arg(1), g_box_type)) {
        BoxObject* box_wrapper = py_touserdata(py_arg(1));
        if (!box_wrapper->box) {
            return RuntimeError("Box is null");
        }
        jpatcher_deleteobj(self->patcher, (t_jbox*)box_wrapper->box);
        box_wrapper->box = NULL;  // Invalidate the wrapper
    } else {
        return TypeError("Argument must be a Box object");
    }

    py_newnone(py_retval());
    return true;
}

// Method: set_locked(locked)
static bool Patcher_set_locked(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PatcherObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    char locked = (char)py_toint(py_arg(1));
    t_max_err err = jpatcher_set_locked(self->patcher, locked);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set locked state");
    }

    py_newnone(py_retval());
    return true;
}

// Method: get_title()
static bool Patcher_get_title(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_symbol* title = object_attr_getsym(self->patcher, gensym("title"));

    if (title) {
        py_newstr(py_retval(), title->s_name);
    } else {
        py_newstr(py_retval(), "");
    }

    return true;
}

// Method: set_title(title)
static bool Patcher_set_title(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PatcherObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    const char* title = py_tostr(py_arg(1));
    t_max_err err = jpatcher_set_title(self->patcher, gensym(title));

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set title");
    }

    py_newnone(py_retval());
    return true;
}

// Method: get_rect() - Get patcher window rectangle
static bool Patcher_get_rect(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_rect rect;
    t_max_err err = jpatcher_get_rect(self->patcher, &rect);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get patcher rectangle");
    }

    // Return as list: [x, y, width, height]
    py_Ref result = py_retval();
    py_newlistn(result, 4);
    py_newfloat(py_list_getitem(result, 0), rect.x);
    py_newfloat(py_list_getitem(result, 1), rect.y);
    py_newfloat(py_list_getitem(result, 2), rect.width);
    py_newfloat(py_list_getitem(result, 3), rect.height);

    return true;
}

// Method: set_rect(x, y, width, height)
static bool Patcher_set_rect(int argc, py_Ref argv) {
    if (argc != 5) {
        return TypeError("set_rect() takes 4 arguments (x, y, width, height)");
    }

    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_rect rect;
    rect.x = py_tofloat(py_arg(1));
    rect.y = py_tofloat(py_arg(2));
    rect.width = py_tofloat(py_arg(3));
    rect.height = py_tofloat(py_arg(4));

    t_max_err err = jpatcher_set_rect(self->patcher, &rect);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set patcher rectangle");
    }

    py_newnone(py_retval());
    return true;
}

// Method: get_parentpatcher()
static bool Patcher_get_parentpatcher(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_object* parent = jpatcher_get_parentpatcher(self->patcher);

    if (!parent) {
        py_newnone(py_retval());
        return true;
    }

    if (g_patcher_type < 0) {
        return RuntimeError("Patcher type not initialized");
    }

    PatcherObject* wrapper = py_newobject(py_retval(), g_patcher_type, 0, sizeof(PatcherObject));
    wrapper->patcher = parent;
    wrapper->owns_patcher = false;

    return true;
}

// Method: get_toppatcher()
static bool Patcher_get_toppatcher(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_object* top = jpatcher_get_toppatcher(self->patcher);

    if (!top) {
        py_newnone(py_retval());
        return true;
    }

    if (g_patcher_type < 0) {
        return RuntimeError("Patcher type not initialized");
    }

    PatcherObject* wrapper = py_newobject(py_retval(), g_patcher_type, 0, sizeof(PatcherObject));
    wrapper->patcher = top;
    wrapper->owns_patcher = false;

    return true;
}

// Method: set_dirty(dirty)
static bool Patcher_set_dirty(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PatcherObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    char dirty = (char)py_toint(py_arg(1));
    t_max_err err = jpatcher_set_dirty(self->patcher, dirty);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set dirty state");
    }

    py_newnone(py_retval());
    return true;
}

// Method: pointer()
static bool Patcher_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->patcher;
    py_newint(py_retval(), ptr);
    return true;
}

// Method: count() - Count objects in patcher
static bool Patcher_count(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    long count = 0;
    t_object* box = jpatcher_get_firstobject(self->patcher);

    while (box) {
        count++;
        object_method(box, gensym("getnextobject"), &box);
    }

    py_newint(py_retval(), count);
    return true;
}


// ----------------------------------------------------------------------------
// Hashtab wrapper - Wrapper for Max hashtable objects

typedef struct {
    t_hashtab* hashtab;
    bool owns_hashtab;
} HashtabObject;

static bool Hashtab__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    HashtabObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(HashtabObject));

    // Create new hashtab with default size (or custom size if provided)
    long slotcount = 0;  // 0 = use default
    if (argc > 1 && py_isint(py_arg(1))) {
        slotcount = py_toint(py_arg(1));
    }

    wrapper->hashtab = hashtab_new(slotcount);
    wrapper->owns_hashtab = true;

    return true;
}

static bool Hashtab__init__(int argc, py_Ref argv) {
    // Already created in __new__
    py_newnone(py_retval());
    return true;
}

static void Hashtab__del__(void* self) {
    HashtabObject* wrapper = (HashtabObject*)self;
    if (wrapper->owns_hashtab && wrapper->hashtab) {
        object_free(wrapper->hashtab);
        wrapper->hashtab = NULL;
    }
}

static bool Hashtab__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->hashtab) {
        t_atom_long size = hashtab_getsize(self->hashtab);
        snprintf(buf, sizeof(buf), "Hashtab(size=%ld)", size);
    } else {
        snprintf(buf, sizeof(buf), "Hashtab(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// __len__ - Get hashtab size
static bool Hashtab__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    t_atom_long size = hashtab_getsize(self->hashtab);
    py_newint(py_retval(), size);
    return true;
}

// __contains__ - Check if key exists
static bool Hashtab__contains__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_object* val = NULL;
    t_max_err err = hashtab_lookup(self->hashtab, key, &val);

    py_newbool(py_retval(), err == MAX_ERR_NONE);
    return true;
}

// __getitem__ - Get value by key
static bool Hashtab__getitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    // Try to get as object first
    t_object* obj_val = NULL;
    t_max_err err = hashtab_lookup(self->hashtab, key, &obj_val);

    if (err == MAX_ERR_NONE && obj_val) {
        // Try as long first
        t_atom_long long_val = 0;
        err = hashtab_lookuplong(self->hashtab, key, &long_val);
        if (err == MAX_ERR_NONE) {
            py_newint(py_retval(), long_val);
            return true;
        }

        // Try as symbol
        t_symbol* sym_val = NULL;
        err = hashtab_lookupsym(self->hashtab, key, &sym_val);
        if (err == MAX_ERR_NONE && sym_val) {
            py_newstr(py_retval(), sym_val->s_name);
            return true;
        }

        // Return object pointer as int
        py_newint(py_retval(), (py_i64)obj_val);
        return true;
    }

    // Key not found
    py_Ref key_ref = py_getreg(0);
    py_newstr(key_ref, key_str);
    return KeyError(key_ref);
}

// __setitem__ - Set value by key
static bool Hashtab__setitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);
    py_Ref value = py_arg(2);

    t_max_err err = MAX_ERR_GENERIC;

    // Store based on type
    if (py_isint(value)) {
        err = hashtab_storelong(self->hashtab, key, py_toint(value));
    } else if (py_isstr(value)) {
        err = hashtab_storesym(self->hashtab, key, gensym(py_tostr(value)));
    } else if (py_isfloat(value)) {
        // Store float as object (no direct float support in hashtab)
        // We'll just store it as a long for now
        err = hashtab_storelong(self->hashtab, key, (t_atom_long)py_tofloat(value));
    } else if (g_object_type >= 0 && py_checktype(value, g_object_type)) {
        MaxObject* obj_wrapper = py_touserdata(value);
        err = hashtab_store(self->hashtab, key, obj_wrapper->obj);
    } else {
        return TypeError("Unsupported value type for hashtab");
    }

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to store value in hashtab");
    }

    py_newnone(py_retval());
    return true;
}

// Method: wrap(pointer)
static bool Hashtab_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    // Free old hashtab if we owned it
    if (self->owns_hashtab && self->hashtab) {
        object_free(self->hashtab);
    }

    self->hashtab = (t_hashtab*)ptr;
    self->owns_hashtab = false;

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Hashtab_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->hashtab == NULL);
    return true;
}

// Method: store(key, value)
static bool Hashtab_store(int argc, py_Ref argv) {
    // Same as __setitem__
    return Hashtab__setitem__(argc, argv);
}

// Method: lookup(key, default=None)
static bool Hashtab_lookup(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("lookup() takes 1 or 2 arguments");
    }

    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_object* obj_val = NULL;
    t_max_err err = hashtab_lookup(self->hashtab, key, &obj_val);

    if (err != MAX_ERR_NONE) {
        // Key not found, return default
        if (argc == 3) {
            py_assign(py_retval(), py_arg(2));
        } else {
            py_newnone(py_retval());
        }
        return true;
    }

    // Found - try different types
    t_atom_long long_val = 0;
    if (hashtab_lookuplong(self->hashtab, key, &long_val) == MAX_ERR_NONE) {
        py_newint(py_retval(), long_val);
        return true;
    }

    t_symbol* sym_val = NULL;
    if (hashtab_lookupsym(self->hashtab, key, &sym_val) == MAX_ERR_NONE && sym_val) {
        py_newstr(py_retval(), sym_val->s_name);
        return true;
    }

    // Return object pointer as int
    py_newint(py_retval(), (py_i64)obj_val);
    return true;
}

// Method: delete(key)
static bool Hashtab_delete(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_max_err err = hashtab_delete(self->hashtab, key);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to delete key '%s'", key_str);
    }

    py_newnone(py_retval());
    return true;
}

// Method: clear()
static bool Hashtab_clear(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    t_max_err err = hashtab_clear(self->hashtab);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to clear hashtab");
    }

    py_newnone(py_retval());
    return true;
}

// Method: keys()
static bool Hashtab_keys(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    long keycount = 0;
    t_symbol** keys = NULL;

    t_max_err err = hashtab_getkeys(self->hashtab, &keycount, &keys);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get hashtab keys");
    }

    // Create Python list
    py_newlistn(py_retval(), keycount);

    for (long i = 0; i < keycount; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        if (keys[i]) {
            py_newstr(item, keys[i]->s_name);
        } else {
            py_newstr(item, "");
        }
    }

    // Note: keys array is owned by hashtab, don't free

    return true;
}

// Method: has_key(key)
static bool Hashtab_has_key(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_object* val = NULL;
    t_max_err err = hashtab_lookup(self->hashtab, key, &val);

    py_newbool(py_retval(), err == MAX_ERR_NONE);
    return true;
}

// Method: getsize()
static bool Hashtab_getsize(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    t_atom_long size = hashtab_getsize(self->hashtab);
    py_newint(py_retval(), size);
    return true;
}

// Method: pointer()
static bool Hashtab_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->hashtab;
    py_newint(py_retval(), ptr);
    return true;
}


// ----------------------------------------------------------------------------
// Linklist wrapper - Wrapper for Max linked list objects

typedef struct {
    t_linklist* linklist;
    bool owns_linklist;
} LinklistObject;

static bool Linklist__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    LinklistObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(LinklistObject));
    wrapper->linklist = linklist_new();
    wrapper->owns_linklist = true;
    return true;
}

static bool Linklist__init__(int argc, py_Ref argv) {
    // Already created in __new__
    py_newnone(py_retval());
    return true;
}

static void Linklist__del__(void* self) {
    LinklistObject* wrapper = (LinklistObject*)self;
    if (wrapper->owns_linklist && wrapper->linklist) {
        object_free(wrapper->linklist);
        wrapper->linklist = NULL;
    }
}

static bool Linklist__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    LinklistObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->linklist) {
        t_atom_long size = linklist_getsize(self->linklist);
        snprintf(buf, sizeof(buf), "Linklist(size=%ld)", size);
    } else {
        snprintf(buf, sizeof(buf), "Linklist(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// __len__ - Get linklist size
static bool Linklist__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    LinklistObject* self = py_touserdata(py_arg(0));

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    t_atom_long size = linklist_getsize(self->linklist);
    py_newint(py_retval(), size);
    return true;
}

// __getitem__ - Get item by index
static bool Linklist__getitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    LinklistObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    py_i64 index = py_toint(py_arg(1));
    t_atom_long size = linklist_getsize(self->linklist);

    // Handle negative indices
    if (index < 0) {
        index = size + index;
    }

    if (index < 0 || index >= size) {
        return IndexError("List index out of range");
    }

    void* item = linklist_getindex(self->linklist, (long)index);

    if (!item) {
        py_newnone(py_retval());
        return true;
    }

    // Return as integer pointer (user can wrap with Object if needed)
    py_newint(py_retval(), (py_i64)item);
    return true;
}

// Method: wrap(pointer)
static bool Linklist_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    LinklistObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    // Free old linklist if we owned it
    if (self->owns_linklist && self->linklist) {
        object_free(self->linklist);
    }

    self->linklist = (t_linklist*)ptr;
    self->owns_linklist = false;

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Linklist_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    LinklistObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->linklist == NULL);
    return true;
}

// Method: append(item)
static bool Linklist_append(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    LinklistObject* self = py_touserdata(py_arg(0));

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    void* item = NULL;

    // Accept Object wrapper or raw pointer
    if (g_object_type >= 0 && py_checktype(py_arg(1), g_object_type)) {
        MaxObject* obj_wrapper = py_touserdata(py_arg(1));
        item = obj_wrapper->obj;
    } else if (py_isint(py_arg(1))) {
        item = (void*)py_toint(py_arg(1));
    } else {
        return TypeError("Append requires Object or integer pointer");
    }

    t_atom_long index = linklist_append(self->linklist, item);
    py_newint(py_retval(), index);
    return true;
}

// Method: insertindex(item, index)
static bool Linklist_insertindex(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    LinklistObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(2, tp_int);

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    void* item = NULL;

    // Accept Object wrapper or raw pointer
    if (g_object_type >= 0 && py_checktype(py_arg(1), g_object_type)) {
        MaxObject* obj_wrapper = py_touserdata(py_arg(1));
        item = obj_wrapper->obj;
    } else if (py_isint(py_arg(1))) {
        item = (void*)py_toint(py_arg(1));
    } else {
        return TypeError("Insert requires Object or integer pointer");
    }

    long index = (long)py_toint(py_arg(2));
    t_atom_long result_index = linklist_insertindex(self->linklist, item, index);
    py_newint(py_retval(), result_index);
    return true;
}

// Method: getindex(index)
static bool Linklist_getindex(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    LinklistObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    long index = (long)py_toint(py_arg(1));
    void* item = linklist_getindex(self->linklist, index);

    if (!item) {
        py_newnone(py_retval());
        return true;
    }

    py_newint(py_retval(), (py_i64)item);
    return true;
}

// Method: chuckindex(index)
static bool Linklist_chuckindex(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    LinklistObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    long index = (long)py_toint(py_arg(1));
    long result = linklist_chuckindex(self->linklist, index);
    py_newint(py_retval(), result);
    return true;
}

// Method: deleteindex(index)
static bool Linklist_deleteindex(int argc, py_Ref argv) {
    // Alias for chuckindex
    return Linklist_chuckindex(argc, argv);
}

// Method: clear()
static bool Linklist_clear(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    LinklistObject* self = py_touserdata(py_arg(0));

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    linklist_clear(self->linklist);
    py_newnone(py_retval());
    return true;
}

// Method: getsize()
static bool Linklist_getsize(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    LinklistObject* self = py_touserdata(py_arg(0));

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    t_atom_long size = linklist_getsize(self->linklist);
    py_newint(py_retval(), size);
    return true;
}

// Method: reverse()
static bool Linklist_reverse(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    LinklistObject* self = py_touserdata(py_arg(0));

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    linklist_reverse(self->linklist);
    py_newnone(py_retval());
    return true;
}

// Method: rotate(n)
static bool Linklist_rotate(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    LinklistObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    long n = (long)py_toint(py_arg(1));
    linklist_rotate(self->linklist, n);
    py_newnone(py_retval());
    return true;
}

// Method: shuffle()
static bool Linklist_shuffle(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    LinklistObject* self = py_touserdata(py_arg(0));

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    linklist_shuffle(self->linklist);
    py_newnone(py_retval());
    return true;
}

// Method: swap(a, b)
static bool Linklist_swap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    LinklistObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_int);

    if (!self->linklist) {
        return RuntimeError("Linklist is null");
    }

    long a = (long)py_toint(py_arg(1));
    long b = (long)py_toint(py_arg(2));
    linklist_swap(self->linklist, a, b);
    py_newnone(py_retval());
    return true;
}

// Method: pointer()
static bool Linklist_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    LinklistObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->linklist;
    py_newint(py_retval(), ptr);
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
// Atom parsing functions

// Module-level parse function
static bool api_parse(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    const char* parsestr = py_tostr(py_arg(0));

    // Parse string into atoms
    t_atom* av = NULL;
    long ac = 0;
    t_max_err err = atom_setparse(&ac, &av, parsestr);

    if (err != MAX_ERR_NONE) {
        if (av) sysmem_freeptr(av);
        py_newstr(py_retval(), "Failed to parse string");
        return false;
    }

    // Create AtomArray to hold result
    py_newobject(py_retval(), g_atomarray_type, 0, sizeof(AtomArrayObject));
    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(py_retval());

    // Create atomarray and copy atoms
    arr_obj->atomarray = atomarray_new(ac, av);
    arr_obj->owns_atomarray = true;

    // Free temporary atoms
    if (av) sysmem_freeptr(av);

    return true;
}

// AtomArray.from_parse() class method
static bool AtomArray_from_parse(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    const char* parsestr = py_tostr(py_arg(0));

    // Parse string into atoms
    t_atom* av = NULL;
    long ac = 0;
    t_max_err err = atom_setparse(&ac, &av, parsestr);

    if (err != MAX_ERR_NONE) {
        if (av) sysmem_freeptr(av);
        py_newstr(py_retval(), "Failed to parse string");
        return false;
    }

    // Create AtomArray to hold result
    py_newobject(py_retval(), g_atomarray_type, 0, sizeof(AtomArrayObject));
    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(py_retval());

    // Create atomarray and copy atoms
    arr_obj->atomarray = atomarray_new(ac, av);
    arr_obj->owns_atomarray = true;

    // Free temporary atoms
    if (av) sysmem_freeptr(av);

    return true;
}


// ----------------------------------------------------------------------------
// Object registration and notification functions

// object_register() - Register object in namespace
static bool api_object_register(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);  // namespace
    PY_CHECK_ARG_TYPE(1, tp_str);  // name
    PY_CHECK_ARG_TYPE(2, tp_int);  // object pointer

    const char* ns_str = py_tostr(py_arg(0));
    const char* name_str = py_tostr(py_arg(1));
    py_i64 obj_ptr = py_toint(py_arg(2));

    t_symbol* ns = gensym(ns_str);
    t_symbol* name = gensym(name_str);
    void* obj = (void*)obj_ptr;

    void* registered = object_register(ns, name, obj);

    // Return pointer to registered object (may be different from input)
    py_newint(py_retval(), (py_i64)registered);
    return true;
}

// object_unregister() - Unregister object
static bool api_object_unregister(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer

    py_i64 obj_ptr = py_toint(py_arg(0));
    void* obj = (void*)obj_ptr;

    t_max_err err = object_unregister(obj);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to unregister object");
        return false;
    }

    py_newnone(py_retval());
    return true;
}

// object_findregistered() - Find registered object by namespace and name
static bool api_object_findregistered(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str);  // namespace
    PY_CHECK_ARG_TYPE(1, tp_str);  // name

    const char* ns_str = py_tostr(py_arg(0));
    const char* name_str = py_tostr(py_arg(1));

    t_symbol* ns = gensym(ns_str);
    t_symbol* name = gensym(name_str);

    void* obj = object_findregistered(ns, name);

    if (obj == NULL) {
        py_newnone(py_retval());
    } else {
        py_newint(py_retval(), (py_i64)obj);
    }

    return true;
}

// object_findregisteredbyptr() - Find namespace and name by object pointer
static bool api_object_findregisteredbyptr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer

    py_i64 obj_ptr = py_toint(py_arg(0));
    void* obj = (void*)obj_ptr;

    t_symbol* ns = NULL;
    t_symbol* name = NULL;

    t_max_err err = object_findregisteredbyptr(&ns, &name, obj);

    if (err != MAX_ERR_NONE || ns == NULL || name == NULL) {
        py_newnone(py_retval());
        return true;
    }

    // Return tuple (namespace, name)
    py_newlistn(py_retval(), 2);
    py_Ref item0 = py_list_getitem(py_retval(), 0);
    py_Ref item1 = py_list_getitem(py_retval(), 1);
    py_newstr(item0, ns->s_name);
    py_newstr(item1, name->s_name);

    return true;
}

// object_attach() - Attach client to registered object
static bool api_object_attach(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);  // namespace
    PY_CHECK_ARG_TYPE(1, tp_str);  // name
    PY_CHECK_ARG_TYPE(2, tp_int);  // client object pointer

    const char* ns_str = py_tostr(py_arg(0));
    const char* name_str = py_tostr(py_arg(1));
    py_i64 client_ptr = py_toint(py_arg(2));

    t_symbol* ns = gensym(ns_str);
    t_symbol* name = gensym(name_str);
    void* client = (void*)client_ptr;

    void* registered_obj = object_attach(ns, name, client);

    if (registered_obj == NULL) {
        py_newnone(py_retval());
    } else {
        py_newint(py_retval(), (py_i64)registered_obj);
    }

    return true;
}

// object_detach() - Detach client from registered object
static bool api_object_detach(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);  // namespace
    PY_CHECK_ARG_TYPE(1, tp_str);  // name
    PY_CHECK_ARG_TYPE(2, tp_int);  // client object pointer

    const char* ns_str = py_tostr(py_arg(0));
    const char* name_str = py_tostr(py_arg(1));
    py_i64 client_ptr = py_toint(py_arg(2));

    t_symbol* ns = gensym(ns_str);
    t_symbol* name = gensym(name_str);
    void* client = (void*)client_ptr;

    t_max_err err = object_detach(ns, name, client);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to detach from object");
        return false;
    }

    py_newnone(py_retval());
    return true;
}

// object_attach_byptr() - Attach to registered object by pointer
static bool api_object_attach_byptr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // client object pointer
    PY_CHECK_ARG_TYPE(1, tp_int);  // registered object pointer

    py_i64 client_ptr = py_toint(py_arg(0));
    py_i64 registered_ptr = py_toint(py_arg(1));

    void* client = (void*)client_ptr;
    void* registered_obj = (void*)registered_ptr;

    t_max_err err = object_attach_byptr(client, registered_obj);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to attach by pointer");
        return false;
    }

    py_newnone(py_retval());
    return true;
}

// object_detach_byptr() - Detach from registered object by pointer
static bool api_object_detach_byptr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // client object pointer
    PY_CHECK_ARG_TYPE(1, tp_int);  // registered object pointer

    py_i64 client_ptr = py_toint(py_arg(0));
    py_i64 registered_ptr = py_toint(py_arg(1));

    void* client = (void*)client_ptr;
    void* registered_obj = (void*)registered_ptr;

    t_max_err err = object_detach_byptr(client, registered_obj);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to detach by pointer");
        return false;
    }

    py_newnone(py_retval());
    return true;
}

// object_notify() - Send notification to attached clients
static bool api_object_notify(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_str);  // message symbol
    PY_CHECK_ARG_TYPE(2, tp_int);  // data pointer (can be NULL/0)

    py_i64 obj_ptr = py_toint(py_arg(0));
    const char* msg_str = py_tostr(py_arg(1));
    py_i64 data_ptr = py_toint(py_arg(2));

    void* obj = (void*)obj_ptr;
    t_symbol* msg = gensym(msg_str);
    void* data = (void*)data_ptr;

    t_max_err err = object_notify(obj, msg, data);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to notify");
        return false;
    }

    py_newnone(py_retval());
    return true;
}


// ----------------------------------------------------------------------------
// Clock wrapper

typedef struct {
    t_clock* clock;
    bool owns_clock;
    py_Ref callback;  // Store Python callback
    void* owner;      // Store owner object pointer
} ClockObject;

// Global type
static py_Type g_clock_type;

// Clock callback function - bridges C to Python
static void clock_callback_bridge(ClockObject* clock_obj) {
    if (clock_obj == NULL || clock_obj->callback == NULL) return;

    // Call Python callback
    py_push(clock_obj->callback);
    py_pushnil();
    bool ok = py_vectorcall(0, 0);

    if (!ok) {
        py_printexc();
    }
}

// Clock.__new__
static bool Clock__new__(int argc, py_Ref argv) {
    py_newobject(py_retval(), g_clock_type, 0, sizeof(ClockObject));
    return true;
}

// Clock.__init__(owner_ptr, callback)
static bool Clock__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);     // owner pointer
    // argv[1] is callback function

    ClockObject* self = (ClockObject*)py_touserdata(py_arg(-1));
    py_i64 owner_ptr = py_toint(py_arg(0));
    self->owner = (void*)owner_ptr;

    // Store callback
    self->callback = py_arg(1);
    py_setslot(py_arg(-1), 0, self->callback);  // Keep reference

    // Create clock with bridge function
    self->clock = clock_new(self->owner, (method)clock_callback_bridge);
    self->owns_clock = true;

    py_newnone(py_retval());
    return true;
}

// Clock.__del__
static void Clock__del__(py_Ref self) {
    ClockObject* clock_obj = (ClockObject*)py_touserdata(self);
    if (clock_obj->clock && clock_obj->owns_clock) {
        clock_unset(clock_obj->clock);
        freeobject((t_object*)clock_obj->clock);
        clock_obj->clock = NULL;
    }
}

// Clock.__repr__
static bool Clock__repr__(int argc, py_Ref argv) {
    ClockObject* self = (ClockObject*)py_touserdata(py_arg(0));
    char buf[128];
    snprintf(buf, sizeof(buf), "Clock(active=%s)",
             self->clock ? "True" : "False");
    py_newstr(py_retval(), buf);
    return true;
}

// Clock.delay(milliseconds)
static bool Clock_delay(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ClockObject* self = (ClockObject*)py_touserdata(py_arg(0));

    if (self->clock == NULL) {
        py_newstr(py_retval(), "Clock is null");
        return false;
    }

    py_i64 ms = py_toint(py_arg(1));
    clock_delay(self->clock, (long)ms);

    py_newnone(py_retval());
    return true;
}

// Clock.unset()
static bool Clock_unset(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    ClockObject* self = (ClockObject*)py_touserdata(py_arg(0));

    if (self->clock == NULL) {
        py_newstr(py_retval(), "Clock is null");
        return false;
    }

    clock_unset(self->clock);

    py_newnone(py_retval());
    return true;
}

// Clock.fdelay(milliseconds_float)
static bool Clock_fdelay(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ClockObject* self = (ClockObject*)py_touserdata(py_arg(0));

    if (self->clock == NULL) {
        py_newstr(py_retval(), "Clock is null");
        return false;
    }

    double ms = py_tofloat(py_arg(1));
    clock_fdelay(self->clock, ms);

    py_newnone(py_retval());
    return true;
}

// Clock.pointer()
static bool Clock_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    ClockObject* self = (ClockObject*)py_touserdata(py_arg(0));
    py_newint(py_retval(), (py_i64)self->clock);
    return true;
}


// ----------------------------------------------------------------------------
// Outlet wrapper

typedef struct {
    void* outlet;
    bool owns_outlet;
} OutletObject;

// Global type
static py_Type g_outlet_type;

// Outlet.__new__
static bool Outlet__new__(int argc, py_Ref argv) {
    py_newobject(py_retval(), g_outlet_type, 0, sizeof(OutletObject));
    return true;
}

// Outlet.__init__(owner_ptr, type_string)
static bool Outlet__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);   // owner pointer
    PY_CHECK_ARG_TYPE(1, tp_str);   // outlet type

    OutletObject* self = (OutletObject*)py_touserdata(py_arg(-1));
    py_i64 owner_ptr = py_toint(py_arg(0));
    const char* type_str = py_tostr(py_arg(1));

    // Create outlet
    self->outlet = outlet_new((void*)owner_ptr, type_str);
    self->owns_outlet = true;

    py_newnone(py_retval());
    return true;
}

// Outlet.__del__
static void Outlet__del__(py_Ref self) {
    OutletObject* outlet_obj = (OutletObject*)py_touserdata(self);
    // Note: Outlets are typically freed by Max when object is freed
    // We don't manually free them
    outlet_obj->outlet = NULL;
}

// Outlet.__repr__
static bool Outlet__repr__(int argc, py_Ref argv) {
    OutletObject* self = (OutletObject*)py_touserdata(py_arg(0));
    char buf[128];
    snprintf(buf, sizeof(buf), "Outlet(ptr=0x%llx)", (unsigned long long)self->outlet);
    py_newstr(py_retval(), buf);
    return true;
}

// Outlet.bang()
static bool Outlet_bang(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    OutletObject* self = (OutletObject*)py_touserdata(py_arg(0));

    if (self->outlet == NULL) {
        py_newstr(py_retval(), "Outlet is null");
        return false;
    }

    outlet_bang((t_outlet*)self->outlet);

    py_newnone(py_retval());
    return true;
}

// Outlet.int(value)
static bool Outlet_int(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    OutletObject* self = (OutletObject*)py_touserdata(py_arg(0));

    if (self->outlet == NULL) {
        py_newstr(py_retval(), "Outlet is null");
        return false;
    }

    py_i64 value = py_toint(py_arg(1));
    outlet_int((t_outlet*)self->outlet, (t_atom_long)value);

    py_newnone(py_retval());
    return true;
}

// Outlet.float(value)
static bool Outlet_float(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    OutletObject* self = (OutletObject*)py_touserdata(py_arg(0));

    if (self->outlet == NULL) {
        py_newstr(py_retval(), "Outlet is null");
        return false;
    }

    double value = py_tofloat(py_arg(1));
    outlet_float((t_outlet*)self->outlet, value);

    py_newnone(py_retval());
    return true;
}

// Outlet.list(atom_array)
static bool Outlet_list(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    OutletObject* self = (OutletObject*)py_touserdata(py_arg(0));

    if (self->outlet == NULL) {
        py_newstr(py_retval(), "Outlet is null");
        return false;
    }

    // Get AtomArray
    py_Ref arr_ref = py_arg(1);
    if (py_typeof(arr_ref) != g_atomarray_type) {
        py_newstr(py_retval(), "Argument must be AtomArray");
        return false;
    }

    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(arr_ref);
    if (arr_obj->atomarray == NULL) {
        py_newstr(py_retval(), "AtomArray is null");
        return false;
    }

    long ac;
    t_atom* av;
    atomarray_getatoms(arr_obj->atomarray, &ac, &av);

    outlet_list((t_outlet*)self->outlet, NULL, (short)ac, av);

    py_newnone(py_retval());
    return true;
}

// Outlet.anything(symbol, atom_array)
static bool Outlet_anything(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    OutletObject* self = (OutletObject*)py_touserdata(py_arg(0));

    if (self->outlet == NULL) {
        py_newstr(py_retval(), "Outlet is null");
        return false;
    }

    const char* sym_str = py_tostr(py_arg(1));
    t_symbol* sym = gensym(sym_str);

    // Get AtomArray
    py_Ref arr_ref = py_arg(2);
    if (py_typeof(arr_ref) != g_atomarray_type) {
        py_newstr(py_retval(), "Argument must be AtomArray");
        return false;
    }

    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(arr_ref);
    if (arr_obj->atomarray == NULL) {
        py_newstr(py_retval(), "AtomArray is null");
        return false;
    }

    long ac;
    t_atom* av;
    atomarray_getatoms(arr_obj->atomarray, &ac, &av);

    outlet_anything((t_outlet*)self->outlet, sym, (short)ac, av);

    py_newnone(py_retval());
    return true;
}

// Outlet.pointer()
static bool Outlet_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    OutletObject* self = (OutletObject*)py_touserdata(py_arg(0));
    py_newint(py_retval(), (py_i64)self->outlet);
    return true;
}


// ----------------------------------------------------------------------------
// Defer functions (module-level)

// Defer callback storage
typedef struct {
    py_Ref callback;
    py_Ref symbol;
    py_Ref atomarray;
} DeferData;

// Bridge function for defer
static void defer_callback_bridge(DeferData* data, t_symbol* s, short argc, t_atom* argv) {
    if (data == NULL || data->callback == NULL) return;

    // Call Python callback with symbol and atomarray
    py_push(data->callback);
    py_pushnil();
    py_push(data->symbol);
    py_push(data->atomarray);
    bool ok = py_vectorcall(2, 0);

    if (!ok) {
        py_printexc();
    }

    // Free defer data
    sysmem_freeptr(data);
}

// api.defer(owner_ptr, callback, symbol, atomarray)
static bool api_defer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_int);   // owner pointer
    // argv[1] is callback
    PY_CHECK_ARG_TYPE(2, tp_str);   // symbol
    // argv[3] is atomarray

    py_i64 owner_ptr = py_toint(py_arg(0));
    const char* sym_str = py_tostr(py_arg(2));
    t_symbol* sym = gensym(sym_str);

    // Get atomarray
    py_Ref arr_ref = py_arg(3);
    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(arr_ref);
    long ac;
    t_atom* av;
    atomarray_getatoms(arr_obj->atomarray, &ac, &av);

    // Create defer data (will be freed in bridge)
    DeferData* data = (DeferData*)sysmem_newptr(sizeof(DeferData));
    data->callback = py_arg(1);
    data->symbol = py_arg(2);
    data->atomarray = py_arg(3);

    // Keep references
    py_setslot(py_arg(0), 0, data->callback);
    py_setslot(py_arg(0), 1, data->symbol);
    py_setslot(py_arg(0), 2, data->atomarray);

    defer((void*)owner_ptr, (method)defer_callback_bridge, sym, (short)ac, av);

    py_newnone(py_retval());
    return true;
}

// api.defer_low(owner_ptr, callback, symbol, atomarray)
static bool api_defer_low(int argc, py_Ref argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_int);   // owner pointer
    // argv[1] is callback
    PY_CHECK_ARG_TYPE(2, tp_str);   // symbol
    // argv[3] is atomarray

    py_i64 owner_ptr = py_toint(py_arg(0));
    const char* sym_str = py_tostr(py_arg(2));
    t_symbol* sym = gensym(sym_str);

    // Get atomarray
    py_Ref arr_ref = py_arg(3);
    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(arr_ref);
    long ac;
    t_atom* av;
    atomarray_getatoms(arr_obj->atomarray, &ac, &av);

    // Create defer data (will be freed in bridge)
    DeferData* data = (DeferData*)sysmem_newptr(sizeof(DeferData));
    data->callback = py_arg(1);
    data->symbol = py_arg(2);
    data->atomarray = py_arg(3);

    // Keep references
    py_setslot(py_arg(0), 0, data->callback);
    py_setslot(py_arg(0), 1, data->symbol);
    py_setslot(py_arg(0), 2, data->atomarray);

    defer_low((void*)owner_ptr, (method)defer_callback_bridge, sym, (short)ac, av);

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

    // Atom parsing functions (module-level)
    py_bindfunc(mod, "parse", api_parse);

    // Object registration and notification functions (module-level)
    py_bindfunc(mod, "object_register", api_object_register);
    py_bindfunc(mod, "object_unregister", api_object_unregister);
    py_bindfunc(mod, "object_findregistered", api_object_findregistered);
    py_bindfunc(mod, "object_findregisteredbyptr", api_object_findregisteredbyptr);
    py_bindfunc(mod, "object_attach", api_object_attach);
    py_bindfunc(mod, "object_detach", api_object_detach);
    py_bindfunc(mod, "object_attach_byptr", api_object_attach_byptr);
    py_bindfunc(mod, "object_detach_byptr", api_object_detach_byptr);
    py_bindfunc(mod, "object_notify", api_object_notify);

    // Scheduling functions (module-level)
    py_bindfunc(mod, "defer", api_defer);
    py_bindfunc(mod, "defer_low", api_defer_low);

    // Clock type
    g_clock_type = py_newtype("Clock", tp_object, mod, (py_Dtor)Clock__del__);
    py_bindmethod(g_clock_type, "__new__", Clock__new__);
    py_bindmethod(g_clock_type, "__init__", Clock__init__);
    py_bindmethod(g_clock_type, "__repr__", Clock__repr__);
    py_bindmethod(g_clock_type, "delay", Clock_delay);
    py_bindmethod(g_clock_type, "fdelay", Clock_fdelay);
    py_bindmethod(g_clock_type, "unset", Clock_unset);
    py_bindmethod(g_clock_type, "pointer", Clock_pointer);

    // Outlet type
    g_outlet_type = py_newtype("Outlet", tp_object, mod, (py_Dtor)Outlet__del__);
    py_bindmethod(g_outlet_type, "__new__", Outlet__new__);
    py_bindmethod(g_outlet_type, "__init__", Outlet__init__);
    py_bindmethod(g_outlet_type, "__repr__", Outlet__repr__);
    py_bindmethod(g_outlet_type, "bang", Outlet_bang);
    py_bindmethod(g_outlet_type, "int", Outlet_int);
    py_bindmethod(g_outlet_type, "float", Outlet_float);
    py_bindmethod(g_outlet_type, "list", Outlet_list);
    py_bindmethod(g_outlet_type, "anything", Outlet_anything);
    py_bindmethod(g_outlet_type, "pointer", Outlet_pointer);

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
    py_bindmethod(g_atomarray_type, "from_parse", AtomArray_from_parse);

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

    // Object type
    g_object_type = py_newtype("Object", tp_object, mod, (py_Dtor)Object__del__);
    py_bindmethod(g_object_type, "__new__", Object__new__);
    py_bindmethod(g_object_type, "__init__", Object__init__);
    py_bindmethod(g_object_type, "__repr__", Object__repr__);
    py_bindmethod(g_object_type, "create", Object_create);
    py_bindmethod(g_object_type, "wrap", Object_wrap);
    py_bindmethod(g_object_type, "free", Object_free_method);
    py_bindmethod(g_object_type, "is_null", Object_is_null);
    py_bindmethod(g_object_type, "classname", Object_classname);
    py_bindmethod(g_object_type, "method", Object_method);
    py_bindmethod(g_object_type, "getattr", Object_getattr);
    py_bindmethod(g_object_type, "setattr", Object_setattr);
    py_bindmethod(g_object_type, "attrnames", Object_attrnames);
    py_bindmethod(g_object_type, "pointer", Object_pointer);

    // Box type
    g_box_type = py_newtype("Box", tp_object, mod, (py_Dtor)Box__del__);
    py_bindmethod(g_box_type, "__new__", Box__new__);
    py_bindmethod(g_box_type, "__init__", Box__init__);
    py_bindmethod(g_box_type, "__repr__", Box__repr__);
    py_bindmethod(g_box_type, "wrap", Box_wrap);
    py_bindmethod(g_box_type, "is_null", Box_is_null);
    py_bindmethod(g_box_type, "classname", Box_classname);
    py_bindmethod(g_box_type, "get_object", Box_get_object);
    py_bindmethod(g_box_type, "get_rect", Box_get_rect);
    py_bindmethod(g_box_type, "set_rect", Box_set_rect);
    py_bindmethod(g_box_type, "pointer", Box_pointer);

    // Patcher type
    g_patcher_type = py_newtype("Patcher", tp_object, mod, (py_Dtor)Patcher__del__);
    py_bindmethod(g_patcher_type, "__new__", Patcher__new__);
    py_bindmethod(g_patcher_type, "__init__", Patcher__init__);
    py_bindmethod(g_patcher_type, "__repr__", Patcher__repr__);
    py_bindmethod(g_patcher_type, "wrap", Patcher_wrap);
    py_bindmethod(g_patcher_type, "is_null", Patcher_is_null);
    py_bindmethod(g_patcher_type, "get_firstobject", Patcher_get_firstobject);
    py_bindmethod(g_patcher_type, "get_lastobject", Patcher_get_lastobject);
    py_bindmethod(g_patcher_type, "newobject", Patcher_newobject);
    py_bindmethod(g_patcher_type, "deleteobj", Patcher_deleteobj);
    py_bindmethod(g_patcher_type, "set_locked", Patcher_set_locked);
    py_bindmethod(g_patcher_type, "get_title", Patcher_get_title);
    py_bindmethod(g_patcher_type, "set_title", Patcher_set_title);
    py_bindmethod(g_patcher_type, "get_rect", Patcher_get_rect);
    py_bindmethod(g_patcher_type, "set_rect", Patcher_set_rect);
    py_bindmethod(g_patcher_type, "get_parentpatcher", Patcher_get_parentpatcher);
    py_bindmethod(g_patcher_type, "get_toppatcher", Patcher_get_toppatcher);
    py_bindmethod(g_patcher_type, "set_dirty", Patcher_set_dirty);
    py_bindmethod(g_patcher_type, "count", Patcher_count);
    py_bindmethod(g_patcher_type, "pointer", Patcher_pointer);

    // Hashtab type
    g_hashtab_type = py_newtype("Hashtab", tp_object, mod, (py_Dtor)Hashtab__del__);
    py_bindmethod(g_hashtab_type, "__new__", Hashtab__new__);
    py_bindmethod(g_hashtab_type, "__init__", Hashtab__init__);
    py_bindmethod(g_hashtab_type, "__repr__", Hashtab__repr__);
    py_bindmethod(g_hashtab_type, "__len__", Hashtab__len__);
    py_bindmethod(g_hashtab_type, "__contains__", Hashtab__contains__);
    py_bindmethod(g_hashtab_type, "__getitem__", Hashtab__getitem__);
    py_bindmethod(g_hashtab_type, "__setitem__", Hashtab__setitem__);
    py_bindmethod(g_hashtab_type, "wrap", Hashtab_wrap);
    py_bindmethod(g_hashtab_type, "is_null", Hashtab_is_null);
    py_bindmethod(g_hashtab_type, "store", Hashtab_store);
    py_bindmethod(g_hashtab_type, "lookup", Hashtab_lookup);
    py_bindmethod(g_hashtab_type, "delete", Hashtab_delete);
    py_bindmethod(g_hashtab_type, "clear", Hashtab_clear);
    py_bindmethod(g_hashtab_type, "keys", Hashtab_keys);
    py_bindmethod(g_hashtab_type, "has_key", Hashtab_has_key);
    py_bindmethod(g_hashtab_type, "getsize", Hashtab_getsize);
    py_bindmethod(g_hashtab_type, "pointer", Hashtab_pointer);

    // Linklist type
    g_linklist_type = py_newtype("Linklist", tp_object, mod, (py_Dtor)Linklist__del__);
    py_bindmethod(g_linklist_type, "__new__", Linklist__new__);
    py_bindmethod(g_linklist_type, "__init__", Linklist__init__);
    py_bindmethod(g_linklist_type, "__repr__", Linklist__repr__);
    py_bindmethod(g_linklist_type, "__len__", Linklist__len__);
    py_bindmethod(g_linklist_type, "__getitem__", Linklist__getitem__);
    py_bindmethod(g_linklist_type, "wrap", Linklist_wrap);
    py_bindmethod(g_linklist_type, "is_null", Linklist_is_null);
    py_bindmethod(g_linklist_type, "append", Linklist_append);
    py_bindmethod(g_linklist_type, "insertindex", Linklist_insertindex);
    py_bindmethod(g_linklist_type, "getindex", Linklist_getindex);
    py_bindmethod(g_linklist_type, "chuckindex", Linklist_chuckindex);
    py_bindmethod(g_linklist_type, "deleteindex", Linklist_deleteindex);
    py_bindmethod(g_linklist_type, "clear", Linklist_clear);
    py_bindmethod(g_linklist_type, "getsize", Linklist_getsize);
    py_bindmethod(g_linklist_type, "reverse", Linklist_reverse);
    py_bindmethod(g_linklist_type, "rotate", Linklist_rotate);
    py_bindmethod(g_linklist_type, "shuffle", Linklist_shuffle);
    py_bindmethod(g_linklist_type, "swap", Linklist_swap);
    py_bindmethod(g_linklist_type, "pointer", Linklist_pointer);

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
