// api_linklist.h
// Linklist wrapper for pktpy API

#ifndef API_LINKLIST_H
#define API_LINKLIST_H

#include "api_common.h"

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


#endif // API_LINKLIST_H