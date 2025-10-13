// api_atom.h
// Atom wrapper for pktpy API

#ifndef API_ATOM_H
#define API_ATOM_H

#include "api_common.h"
#include "api_symbol.h"

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

#endif // API_ATOM_H
