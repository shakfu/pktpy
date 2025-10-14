// api_external.h
// External wrapper for pktpy API
//
// Provides access to the pktpy Max external object

#ifndef API_EXTERNAL_H
#define API_EXTERNAL_H

#include "api_common.h"

// Forward declaration
struct t_pktpy;

// ----------------------------------------------------------------------------
// External wrapper

typedef struct {
    struct t_pktpy* external;  // Pointer to the pktpy external instance
} ExternalObject;

// ----------------------------------------------------------------------------
// Constructor/Destructor

static bool External__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    ExternalObject* obj = py_newobject(py_retval(), cls, 0, sizeof(ExternalObject));
    obj->external = NULL;  // Must be set externally
    return true;
}

static bool External__init__(int argc, py_Ref argv) {
    ExternalObject* self = py_touserdata(py_arg(0));

    if (argc == 1) {
        // External() - default empty
        self->external = NULL;
    } else if (argc == 2) {
        // External(ptr) - from pointer (integer)
        PY_CHECK_ARG_TYPE(1, tp_int);
        py_i64 ptr_val = py_toint(py_arg(1));
        self->external = (struct t_pktpy*)ptr_val;
    } else {
        return TypeError("External() takes 0 or 1 argument, got %d", argc - 1);
    }

    py_newnone(py_retval());
    return true;
}

// ----------------------------------------------------------------------------
// String representation

static bool External__str__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        py_newstr(py_retval(), "External(None)");
    } else {
        char buf[256];
        snprintf(buf, sizeof(buf), "External(%p)", (void*)self->external);
        py_newstr(py_retval(), buf);
    }
    return true;
}

static bool External__repr__(int argc, py_Ref argv) {
    return External__str__(argc, argv);
}

// ----------------------------------------------------------------------------
// Methods

// Get pointer to external (for internal use)
static bool External_get_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    // Return as integer (pointer value)
    py_newint(py_retval(), (py_i64)self->external);
    return true;
}

// Check if external is valid (not NULL)
static bool External_is_valid(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    py_newbool(py_retval(), self->external != NULL);
    return true;
}

// Get left outlet (wrapper will be created elsewhere)
static bool External_get_outlet_left(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        return ValueError("External: external is NULL");
    }

    // Return outlet pointer as integer
    // The calling code will wrap this in an Outlet object
    py_newint(py_retval(), (py_i64)((t_object*)self->external)->o_outlet);
    return true;
}

// Get middle outlet
static bool External_get_outlet_middle(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        return ValueError("External: external is NULL");
    }

    // Cast to access p_outlet_middle
    // Note: This requires including the full t_pktpy definition
    py_newint(py_retval(), (py_i64)self->external);
    return true;
}

// Get right outlet
static bool External_get_outlet_right(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        return ValueError("External: external is NULL");
    }

    py_newint(py_retval(), (py_i64)self->external);
    return true;
}

// Get owner object (the external itself as t_object*)
static bool External_get_owner(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        return ValueError("External: external is NULL");
    }

    // Return as Object wrapper
    py_newint(py_retval(), (py_i64)((t_object*)self->external));
    return true;
}

// Get patcher containing this external
static bool External_get_patcher(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        return ValueError("External: external is NULL");
    }

    t_object* owner = (t_object*)self->external;
    t_object* patcher = NULL;
    object_obex_lookup(owner, gensym("#P"), &patcher);

    if (patcher == NULL) {
        py_newnone(py_retval());
        return true;
    }

    // Return patcher pointer as integer (will be wrapped in Patcher object)
    py_newint(py_retval(), (py_i64)patcher);
    return true;
}

// Get external name
static bool External_get_name(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        return ValueError("External: external is NULL");
    }

    // Access name field from t_pktpy struct
    // Note: This requires the full struct definition
    py_newstr(py_retval(), "pktpy");  // For now, return static name
    return true;
}

// Post message to Max console
static bool External_post(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* msg = py_tostr(py_arg(1));
    post(msg);

    py_newnone(py_retval());
    return true;
}

// Send bang to left outlet
static bool External_bang_left(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        return ValueError("External: external is NULL");
    }

    t_object* owner = (t_object*)self->external;
    if (owner->o_outlet) {
        outlet_bang(owner->o_outlet);
    }

    py_newnone(py_retval());
    return true;
}

// Output value to left outlet
static bool External_out(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    ExternalObject* self = py_touserdata(py_arg(0));

    if (self->external == NULL) {
        return ValueError("External: external is NULL");
    }

    t_object* owner = (t_object*)self->external;
    void* outlet = owner->o_outlet;

    if (!outlet) {
        return ValueError("External: no outlet available");
    }

    py_Ref value = py_arg(1);

    // Handle different Python types
    if (py_isint(value)) {
        t_atom a;
        atom_setlong(&a, py_toint(value));
        outlet_anything(outlet, gensym("int"), 1, &a);
    } else if (py_isfloat(value)) {
        t_atom a;
        atom_setfloat(&a, py_tofloat(value));
        outlet_anything(outlet, gensym("float"), 1, &a);
    } else if (py_isstr(value)) {
        const char* str = py_tostr(value);
        outlet_anything(outlet, gensym(str), 0, NULL);
    } else {
        return TypeError("External.out(): unsupported type");
    }

    py_newnone(py_retval());
    return true;
}

#endif // API_EXTERNAL_H
