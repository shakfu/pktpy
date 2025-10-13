// api_symbol.h
// Symbol wrapper for pktpy API

#ifndef API_SYMBOL_H
#define API_SYMBOL_H

#include "api_common.h"

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

#endif // API_SYMBOL_H
