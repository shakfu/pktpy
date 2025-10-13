// api_patchline.h
// Patchline wrapper for pktpy API

#ifndef API_PATCHLINE_H
#define API_PATCHLINE_H

#include "api_common.h"


// ----------------------------------------------------------------------------
// Patchline wrapper - Wrapper for Max patchline (patch cord) objects

typedef struct {
    t_object* patchline;
    bool owns_patchline;
} PatchlineObject;

// py_Type g_patchline_type = -1;

static bool Patchline__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    PatchlineObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(PatchlineObject));
    wrapper->patchline = NULL;
    wrapper->owns_patchline = false;
    return true;
}

static bool Patchline__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Patchline__del__(void* self) {
    PatchlineObject* wrapper = (PatchlineObject*)self;
    // Patchlines are typically not owned by Python wrappers
    wrapper->patchline = NULL;
}

static bool Patchline__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->patchline) {
        snprintf(buf, sizeof(buf), "Patchline(%p)", self->patchline);
    } else {
        snprintf(buf, sizeof(buf), "Patchline(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Method: wrap(pointer)
static bool Patchline_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PatchlineObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    self->patchline = (t_object*)ptr;
    self->owns_patchline = false;

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Patchline_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->patchline == NULL);
    return true;
}

// Method: get_box1()
// Returns the source box (where the patchline originates)
static bool Patchline_get_box1(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    if (g_box_type < 0) {
        return RuntimeError("Box type not initialized");
    }

    t_object* box = jpatchline_get_box1(self->patchline);

    if (!box) {
        py_newnone(py_retval());
        return true;
    }

    // Note: We use manual offsetof since BoxObject is defined earlier in the file
    void* wrapper = py_newobject(py_retval(), g_box_type, 0, sizeof(t_object*) + sizeof(bool));
    *((t_object**)wrapper) = box;  // Set box field
    *((bool*)((char*)wrapper + sizeof(t_object*))) = false;  // Set owns_box field

    return true;
}

// Method: get_box2()
// Returns the destination box (where the patchline connects to)
static bool Patchline_get_box2(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    if (g_box_type < 0) {
        return RuntimeError("Box type not initialized");
    }

    t_object* box = jpatchline_get_box2(self->patchline);

    if (!box) {
        py_newnone(py_retval());
        return true;
    }

    // Note: We use manual offsetof since BoxObject is defined earlier in the file
    void* wrapper = py_newobject(py_retval(), g_box_type, 0, sizeof(t_object*) + sizeof(bool));
    *((t_object**)wrapper) = box;  // Set box field
    *((bool*)((char*)wrapper + sizeof(t_object*))) = false;  // Set owns_box field

    return true;
}

// Method: get_outletnum()
// Returns the outlet number of the source box
static bool Patchline_get_outletnum(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    long outletnum = jpatchline_get_outletnum(self->patchline);
    py_newint(py_retval(), outletnum);
    return true;
}

// Method: get_inletnum()
// Returns the inlet number of the destination box
static bool Patchline_get_inletnum(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    long inletnum = jpatchline_get_inletnum(self->patchline);
    py_newint(py_retval(), inletnum);
    return true;
}

// Method: get_startpoint()
// Returns [x, y] of the patchline start point
static bool Patchline_get_startpoint(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    double x, y;
    t_max_err err = jpatchline_get_startpoint(self->patchline, &x, &y);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get startpoint");
    }

    py_newlistn(py_retval(), 2);
    py_Ref item0 = py_list_getitem(py_retval(), 0);
    py_Ref item1 = py_list_getitem(py_retval(), 1);
    py_newfloat(item0, x);
    py_newfloat(item1, y);

    return true;
}

// Method: get_endpoint()
// Returns [x, y] of the patchline end point
static bool Patchline_get_endpoint(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    double x, y;
    t_max_err err = jpatchline_get_endpoint(self->patchline, &x, &y);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get endpoint");
    }

    py_newlistn(py_retval(), 2);
    py_Ref item0 = py_list_getitem(py_retval(), 0);
    py_Ref item1 = py_list_getitem(py_retval(), 1);
    py_newfloat(item0, x);
    py_newfloat(item1, y);

    return true;
}

// Method: get_hidden()
// Returns whether the patchline is hidden
static bool Patchline_get_hidden(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    char hidden = jpatchline_get_hidden(self->patchline);
    py_newbool(py_retval(), hidden != 0);
    return true;
}

// Method: set_hidden(hidden)
// Set whether the patchline is hidden
static bool Patchline_set_hidden(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PatchlineObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_bool);

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    char hidden = py_tobool(py_arg(1)) ? 1 : 0;
    t_max_err err = jpatchline_set_hidden(self->patchline, hidden);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set hidden");
    }

    py_newnone(py_retval());
    return true;
}

// Method: get_nextline()
// Get the next patchline in the linked list
static bool Patchline_get_nextline(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    if (!self->patchline) {
        return RuntimeError("Patchline is null");
    }

    t_object* nextline = jpatchline_get_nextline(self->patchline);

    if (!nextline) {
        py_newnone(py_retval());
        return true;
    }

    if (g_patchline_type < 0) {
        return RuntimeError("Patchline type not initialized");
    }

    // Note: We use manual offsetof since PatchlineObject is the current struct
    void* wrapper = py_newobject(py_retval(), g_patchline_type, 0, sizeof(t_object*) + sizeof(bool));
    *((t_object**)wrapper) = nextline;  // Set patchline field
    *((bool*)((char*)wrapper + sizeof(t_object*))) = false;  // Set owns_patchline field

    return true;
}

// Method: pointer()
static bool Patchline_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatchlineObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->patchline;
    py_newint(py_retval(), ptr);
    return true;
}


#endif // API_PATCHLINE_H

