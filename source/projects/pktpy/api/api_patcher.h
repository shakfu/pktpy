// api_patcher.h
// Patcher wrapper for pktpy API

#ifndef API_PATCHER_H
#define API_PATCHER_H

#include "api_common.h"


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

// Method: get_firstline()
static bool Patcher_get_firstline(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_object* line = jpatcher_get_firstline(self->patcher);

    if (!line) {
        py_newnone(py_retval());
        return true;
    }

    // Return as integer pointer for now
    // Could wrap in Patchline object if type is registered
    py_newint(py_retval(), (py_i64)line);
    return true;
}

// Method: get_name()
static bool Patcher_get_name(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_symbol* name = jpatcher_get_name(self->patcher);

    if (!name) {
        py_newnone(py_retval());
        return true;
    }

    py_newstr(py_retval(), name->s_name);
    return true;
}

// Method: get_filepath()
static bool Patcher_get_filepath(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_symbol* filepath = jpatcher_get_filepath(self->patcher);

    if (!filepath) {
        py_newnone(py_retval());
        return true;
    }

    py_newstr(py_retval(), filepath->s_name);
    return true;
}

// Method: get_filename()
static bool Patcher_get_filename(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    t_symbol* filename = jpatcher_get_filename(self->patcher);

    if (!filename) {
        py_newnone(py_retval());
        return true;
    }

    py_newstr(py_retval(), filename->s_name);
    return true;
}

// Method: get_boxes()
// Returns a Linklist of all boxes in the patcher
static bool Patcher_get_boxes(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    if (g_linklist_type < 0) {
        return RuntimeError("Linklist type not initialized");
    }

    // Create a new Linklist to hold the boxes
    t_linklist* list = linklist_new();
    t_object* box = jpatcher_get_firstobject(self->patcher);

    while (box) {
        linklist_append(list, box);
        object_method(box, gensym("getnextobject"), &box);
    }

    // Wrap in Linklist object
    // Note: We use manual offsetof since LinklistObject is defined later in the file
    void* wrapper = py_newobject(py_retval(), g_linklist_type, 0, sizeof(t_linklist*) + sizeof(bool));
    *((t_linklist**)wrapper) = list;  // Set linklist field
    *((bool*)((char*)wrapper + sizeof(t_linklist*))) = true;  // Set owns_linklist field

    return true;
}

// Method: get_lines()
// Returns a Linklist of all patchlines in the patcher
static bool Patcher_get_lines(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PatcherObject* self = py_touserdata(py_arg(0));

    if (!self->patcher) {
        return RuntimeError("Patcher is null");
    }

    if (g_linklist_type < 0) {
        return RuntimeError("Linklist type not initialized");
    }

    // Create a new Linklist to hold the lines
    t_linklist* list = linklist_new();
    t_object* line = jpatcher_get_firstline(self->patcher);

    while (line) {
        linklist_append(list, line);
        object_method(line, gensym("getnextline"), &line);
    }

    // Wrap in Linklist object
    // Note: We use manual offsetof since LinklistObject is defined later in the file
    void* wrapper = py_newobject(py_retval(), g_linklist_type, 0, sizeof(t_linklist*) + sizeof(bool));
    *((t_linklist**)wrapper) = list;  // Set linklist field
    *((bool*)((char*)wrapper + sizeof(t_linklist*))) = true;  // Set owns_linklist field

    return true;
}



#endif // API_PATCHER_H