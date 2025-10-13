// api_clock.h
// Clock wrapper for pktpy API

#ifndef API_CLOCK_H
#define API_CLOCK_H

#include "api_common.h"


// ----------------------------------------------------------------------------
// Clock wrapper

typedef struct {
    t_clock* clock;
    bool owns_clock;
    py_Ref callback;  // Store Python callback
    void* owner;      // Store owner object pointer
} ClockObject;

// Global type
py_Type g_clock_type;

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


#endif // API_CLOCK_H