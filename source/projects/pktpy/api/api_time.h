// api_time.h
// Time (ITM) wrapper for pktpy API
//
// Transport and timing beyond Clock - tempo-aware scheduling

#ifndef API_TIME_H
#define API_TIME_H

#include "api_common.h"
#include "ext_itm.h"

// ----------------------------------------------------------------------------
// ITM wrapper

typedef struct {
    t_itm* itm;
    bool owns_itm;
} ITMObject;

// ----------------------------------------------------------------------------
// Constructor/Destructor

static bool ITM__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    ITMObject* obj = py_newobject(py_retval(), cls, 0, sizeof(ITMObject));
    obj->itm = NULL;
    obj->owns_itm = false;
    return true;
}

static bool ITM__init__(int argc, py_Ref argv) {
    ITMObject* self = py_touserdata(py_arg(0));

    if (argc == 1) {
        // ITM() - get global ITM
        self->itm = (t_itm*)itm_getglobal();
        self->owns_itm = false;
    } else if (argc == 2) {
        PY_CHECK_ARG_TYPE(1, tp_str);
        // ITM(name) - get named ITM
        const char* name = py_tostr(py_arg(1));
        t_symbol* s = gensym(name);
        self->itm = (t_itm*)itm_getnamed(s, NULL, NULL, 1);
        self->owns_itm = true;
    } else if (argc == 3) {
        PY_CHECK_ARG_TYPE(1, tp_int);  // pointer to ITM
        // ITM(ptr) - wrap existing ITM
        py_i64 ptr_val = py_toint(py_arg(1));
        self->itm = (t_itm*)ptr_val;
        self->owns_itm = false;
    } else {
        return TypeError("ITM() takes 0, 1, or 2 arguments, got %d", argc - 1);
    }

    py_newnone(py_retval());
    return true;
}

static void ITM__del__(void* self) {
    ITMObject* obj = (ITMObject*)self;

    if (obj->itm && obj->owns_itm) {
        itm_dereference(obj->itm);
        obj->itm = NULL;
    }
}

// ----------------------------------------------------------------------------
// String representation

static bool ITM__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->itm) {
        snprintf(buf, sizeof(buf), "ITM(%p)", (void*)self->itm);
    } else {
        snprintf(buf, sizeof(buf), "ITM(None)");
    }

    py_newstr(py_retval(), buf);
    return true;
}

// ----------------------------------------------------------------------------
// Time query methods

// Get current time in ticks
static bool ITM_getticks(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    double ticks = itm_getticks(self->itm);
    py_newfloat(py_retval(), ticks);
    return true;
}

// Get current time (in ms)
static bool ITM_gettime(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    double time = itm_gettime(self->itm);
    py_newfloat(py_retval(), time);
    return true;
}

// Get state (playing, paused, etc.)
static bool ITM_getstate(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    long state = itm_getstate(self->itm);
    py_newint(py_retval(), state);
    return true;
}

// ----------------------------------------------------------------------------
// Time conversion methods

// Convert ticks to milliseconds
static bool ITM_tickstoms(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    double ticks = py_tofloat(py_arg(1));
    double ms = itm_tickstoms(self->itm, ticks);
    py_newfloat(py_retval(), ms);
    return true;
}

// Convert milliseconds to ticks
static bool ITM_mstoticks(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    double ms = py_tofloat(py_arg(1));
    double ticks = itm_mstoticks(self->itm, ms);
    py_newfloat(py_retval(), ticks);
    return true;
}

// Convert milliseconds to samples
static bool ITM_mstosamps(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    double ms = py_tofloat(py_arg(1));
    double samps = itm_mstosamps(self->itm, ms);
    py_newfloat(py_retval(), samps);
    return true;
}

// Convert samples to milliseconds
static bool ITM_sampstoms(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    double samps = py_tofloat(py_arg(1));
    double ms = itm_sampstoms(self->itm, samps);
    py_newfloat(py_retval(), ms);
    return true;
}

// Convert BBU (bars, beats, units) to ticks
static bool ITM_bbutoticsk(int argc, py_Ref argv) {
    PY_CHECK_ARGC(4);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    long bars = py_toint(py_arg(1));
    long beats = py_toint(py_arg(2));
    double units = py_tofloat(py_arg(3));

    double ticks = 0;
    itm_barbeatunitstoticks(self->itm, bars, beats, units, &ticks, 0);
    py_newfloat(py_retval(), ticks);
    return true;
}

// Convert ticks to BBU (returns tuple)
static bool ITM_tickstobbu(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    double ticks = py_tofloat(py_arg(1));
    long bars, beats;
    double units;

    itm_tickstobarbeatunits(self->itm, ticks, &bars, &beats, &units, 0);

    // Return tuple (bars, beats, units)
    py_newtuple(py_retval(), 3);
    py_Ref tuple = py_retval();

    py_newint(py_arg(-1), bars);
    py_tuple_setitem(tuple, 0, py_arg(-1));

    py_newint(py_arg(-1), beats);
    py_tuple_setitem(tuple, 1, py_arg(-1));

    py_newfloat(py_arg(-1), units);
    py_tuple_setitem(tuple, 2, py_arg(-1));

    return true;
}

// ----------------------------------------------------------------------------
// Transport control methods

// Pause transport
static bool ITM_pause(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    itm_pause(self->itm);
    py_newnone(py_retval());
    return true;
}

// Resume transport
static bool ITM_resume(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    itm_resume(self->itm);
    py_newnone(py_retval());
    return true;
}

// Seek to position
static bool ITM_seek(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    double oldticks = py_tofloat(py_arg(1));
    double newticks = py_tofloat(py_arg(2));
    long chase = (argc >= 4) ? py_toint(py_arg(3)) : 1;

    itm_seek(self->itm, oldticks, newticks, chase);
    py_newnone(py_retval());
    return true;
}

// ----------------------------------------------------------------------------
// Time signature methods

// Set time signature
static bool ITM_settimesignature(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    long num = py_toint(py_arg(1));
    long denom = py_toint(py_arg(2));
    long flags = (argc >= 4) ? py_toint(py_arg(3)) : 0;

    itm_settimesignature(self->itm, num, denom, flags);
    py_newnone(py_retval());
    return true;
}

// Get time signature (returns tuple)
static bool ITM_gettimesignature(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    long num, denom;
    itm_gettimesignature(self->itm, &num, &denom);

    // Return tuple (numerator, denominator)
    py_newtuple(py_retval(), 2);
    py_Ref tuple = py_retval();

    py_newint(py_arg(-1), num);
    py_tuple_setitem(tuple, 0, py_arg(-1));

    py_newint(py_arg(-1), denom);
    py_tuple_setitem(tuple, 1, py_arg(-1));

    return true;
}

// ----------------------------------------------------------------------------
// Utility methods

// Dump ITM info to Max console
static bool ITM_dump(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    itm_dump(self->itm);
    py_newnone(py_retval());
    return true;
}

// Sync ITM
static bool ITM_sync(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    if (self->itm == NULL) {
        return ValueError("ITM: itm is NULL");
    }

    itm_sync(self->itm);
    py_newnone(py_retval());
    return true;
}

// Get pointer
static bool ITM_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    py_newint(py_retval(), (py_i64)self->itm);
    return true;
}

// Check if valid
static bool ITM_is_valid(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    ITMObject* self = py_touserdata(py_arg(0));

    py_newbool(py_retval(), self->itm != NULL);
    return true;
}

// ----------------------------------------------------------------------------
// Module-level functions

// Get global ITM
static bool itm_getglobal_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);

    void* itm = itm_getglobal();
    py_newint(py_retval(), (py_i64)itm);
    return true;
}

// Set resolution
static bool itm_setresolution_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);

    double res = py_tofloat(py_arg(0));
    itm_setresolution(res);

    py_newnone(py_retval());
    return true;
}

// Get resolution
static bool itm_getresolution_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);

    double res = itm_getresolution();
    py_newfloat(py_retval(), res);
    return true;
}

#endif // API_TIME_H
