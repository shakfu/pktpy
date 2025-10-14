// api_inlet.h
// Inlet and Proxy wrapper for pktpy API

#ifndef API_INLET_H
#define API_INLET_H

#include "api_common.h"

// ----------------------------------------------------------------------------
// Inlet wrapper - Create additional inlets for Max objects
//
// Max provides several ways to create inlets:
// 1. inlet_new() - General purpose, can receive any or specific messages
// 2. intin() - Integer-typed inlet (sends in1, in2, etc. messages)
// 3. floatin() - Float-typed inlet (sends ft1, ft2, etc. messages)
// 4. proxy_new() - Modern approach with proxy_getinlet() to identify inlet

typedef struct {
    void* inlet;
    long inlet_num;  // For tracking inlet number with proxies
    bool is_proxy;
    bool owns_inlet;
} InletObject;

static bool Inlet__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    InletObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(InletObject));
    wrapper->inlet = NULL;
    wrapper->inlet_num = 0;
    wrapper->is_proxy = false;
    wrapper->owns_inlet = false;
    return true;
}

static bool Inlet__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Inlet__del__(void* self) {
    InletObject* wrapper = (InletObject*)self;
    // Note: Inlets are typically freed by Max when object is freed
    // We don't manually free them unless we explicitly own them
    if (wrapper->owns_inlet && wrapper->inlet) {
        // Only delete if we explicitly own it
        inlet_delete(wrapper->inlet);
    }
    wrapper->inlet = NULL;
}

static bool Inlet__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    InletObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->is_proxy) {
        snprintf(buf, sizeof(buf), "Inlet(proxy, num=%ld, ptr=%p)",
                 self->inlet_num, self->inlet);
    } else {
        snprintf(buf, sizeof(buf), "Inlet(ptr=%p)", self->inlet);
    }
    py_newstr(py_retval(), buf);
    return true;
}

// ----------------------------------------------------------------------------
// Module-level inlet creation functions

// api.inlet_new(owner_ptr, message_name)
// Create a general-purpose inlet
static bool api_inlet_new(int argc, py_Ref argv) {
    if (argc < 1 || argc > 2) {
        return TypeError("inlet_new() takes 1-2 arguments, got %d", argc);
    }
    PY_CHECK_ARG_TYPE(0, tp_int);  // owner pointer

    py_i64 owner_ptr = py_toint(py_arg(0));
    const char* msg = NULL;

    if (argc >= 2 && py_isstr(py_arg(1))) {
        msg = py_tostr(py_arg(1));
    }

    void* inlet = inlet_new((void*)owner_ptr, msg);

    if (!inlet) {
        return RuntimeError("Failed to create inlet");
    }

    // Create InletObject wrapper
    InletObject* wrapper = py_newobject(py_retval(), g_inlet_type, 0, sizeof(InletObject));
    wrapper->inlet = inlet;
    wrapper->inlet_num = 0;
    wrapper->is_proxy = false;
    wrapper->owns_inlet = true;

    return true;
}

// api.intin(owner_ptr, inlet_number)
// Create integer-typed inlet (sends in1, in2, etc.)
static bool api_intin(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 owner_ptr = py_toint(py_arg(0));
    short inlet_num = (short)py_toint(py_arg(1));

    if (inlet_num < 1 || inlet_num > 9) {
        return ValueError("Inlet number must be between 1 and 9");
    }

    void* inlet = intin((void*)owner_ptr, inlet_num);

    if (!inlet) {
        return RuntimeError("Failed to create integer inlet");
    }

    InletObject* wrapper = py_newobject(py_retval(), g_inlet_type, 0, sizeof(InletObject));
    wrapper->inlet = inlet;
    wrapper->inlet_num = inlet_num;
    wrapper->is_proxy = false;
    wrapper->owns_inlet = true;

    return true;
}

// api.floatin(owner_ptr, inlet_number)
// Create float-typed inlet (sends ft1, ft2, etc.)
static bool api_floatin(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 owner_ptr = py_toint(py_arg(0));
    short inlet_num = (short)py_toint(py_arg(1));

    if (inlet_num < 1 || inlet_num > 9) {
        return ValueError("Inlet number must be between 1 and 9");
    }

    void* inlet = floatin((void*)owner_ptr, inlet_num);

    if (!inlet) {
        return RuntimeError("Failed to create float inlet");
    }

    InletObject* wrapper = py_newobject(py_retval(), g_inlet_type, 0, sizeof(InletObject));
    wrapper->inlet = inlet;
    wrapper->inlet_num = inlet_num;
    wrapper->is_proxy = false;
    wrapper->owns_inlet = true;

    return true;
}

// api.proxy_new(owner_ptr, inlet_id, stuffloc_ptr)
// Create a proxy inlet (modern approach)
static bool api_proxy_new(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int);  // owner pointer
    PY_CHECK_ARG_TYPE(1, tp_int);  // inlet ID
    PY_CHECK_ARG_TYPE(2, tp_int);  // stuffloc pointer

    py_i64 owner_ptr = py_toint(py_arg(0));
    long inlet_id = (long)py_toint(py_arg(1));
    py_i64 stuffloc_ptr = py_toint(py_arg(2));

    void* proxy = proxy_new((void*)owner_ptr, inlet_id, (long*)stuffloc_ptr);

    if (!proxy) {
        return RuntimeError("Failed to create proxy inlet");
    }

    InletObject* wrapper = py_newobject(py_retval(), g_inlet_type, 0, sizeof(InletObject));
    wrapper->inlet = proxy;
    wrapper->inlet_num = inlet_id;
    wrapper->is_proxy = true;
    wrapper->owns_inlet = true;

    return true;
}

// api.proxy_getinlet(owner_ptr) -> int
// Get the inlet number where a message was received
static bool api_proxy_getinlet(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);

    py_i64 owner_ptr = py_toint(py_arg(0));
    long inlet_num = proxy_getinlet((t_object*)owner_ptr);

    py_newint(py_retval(), inlet_num);
    return true;
}

// api.inlet_count(owner_ptr) -> int
// Get the number of inlets for an object
static bool api_inlet_count(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);

    py_i64 owner_ptr = py_toint(py_arg(0));
    long count = inlet_count((t_object*)owner_ptr);

    py_newint(py_retval(), count);
    return true;
}

// api.inlet_nth(owner_ptr, index) -> Inlet or None
// Get the nth inlet (0-indexed)
static bool api_inlet_nth(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 owner_ptr = py_toint(py_arg(0));
    long index = (long)py_toint(py_arg(1));

    void* inlet = inlet_nth((t_object*)owner_ptr, index);

    if (!inlet) {
        py_newnone(py_retval());
        return true;
    }

    InletObject* wrapper = py_newobject(py_retval(), g_inlet_type, 0, sizeof(InletObject));
    wrapper->inlet = inlet;
    wrapper->inlet_num = index;
    wrapper->is_proxy = false;
    wrapper->owns_inlet = false;  // We don't own it, just wrapping existing

    return true;
}

// ----------------------------------------------------------------------------
// Inlet object methods

// Method: delete()
// Delete this inlet (if we own it)
static bool Inlet_delete(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    InletObject* self = py_touserdata(py_arg(0));

    if (!self->inlet) {
        return RuntimeError("Inlet is null");
    }

    if (!self->owns_inlet) {
        return RuntimeError("Cannot delete inlet we don't own");
    }

    inlet_delete(self->inlet);
    self->inlet = NULL;
    self->owns_inlet = false;

    py_newnone(py_retval());
    return true;
}

// Method: pointer() -> int
// Get pointer to inlet
static bool Inlet_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    InletObject* self = py_touserdata(py_arg(0));
    py_newint(py_retval(), (py_i64)self->inlet);
    return true;
}

// Method: get_num() -> int
// Get inlet number
static bool Inlet_get_num(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    InletObject* self = py_touserdata(py_arg(0));
    py_newint(py_retval(), self->inlet_num);
    return true;
}

// Method: is_proxy() -> bool
// Check if this is a proxy inlet
static bool Inlet_is_proxy(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    InletObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->is_proxy);
    return true;
}

// Method: is_null() -> bool
// Check if inlet pointer is null
static bool Inlet_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    InletObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->inlet == NULL);
    return true;
}

#endif // API_INLET_H
