// api_outlet.h
// Outlet wrapper for pktpy API

#ifndef API_OUTLET_H
#define API_OUTLET_H

#include "api_common.h"


// ----------------------------------------------------------------------------
// Outlet wrapper

typedef struct {
    void* outlet;
    bool owns_outlet;
} OutletObject;

// Global type
py_Type g_outlet_type;

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


#endif // API_OUTLET_H