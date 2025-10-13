// api_box.h
// Box wrapper - Wrapper for Max patcher box objects (t_jbox / t_object)

#ifndef API_BOX_H
#define API_BOX_H

#include "api_common.h"
#include "api_object.h"

// ----------------------------------------------------------------------------
// Box wrapper

typedef struct {
    t_object* box;
    bool owns_box;
} BoxObject;

static bool Box__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    BoxObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(BoxObject));
    wrapper->box = NULL;
    wrapper->owns_box = false;
    return true;
}

static bool Box__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Box__del__(void* self) {
    BoxObject* wrapper = (BoxObject*)self;
    // Boxes are owned by the patcher, so we don't free them
    wrapper->box = NULL;
}

static bool Box__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->box) {
        t_symbol* classname = object_classname(jbox_get_object(self->box));
        snprintf(buf, sizeof(buf), "Box(%s, %p)", classname->s_name, self->box);
    } else {
        snprintf(buf, sizeof(buf), "Box(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Method: wrap(pointer)
static bool Box_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    BoxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    self->box = (t_object*)ptr;
    self->owns_box = false;

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Box_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->box == NULL);
    return true;
}

// Method: classname()
static bool Box_classname(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    if (!self->box) {
        return RuntimeError("Box is null");
    }

    t_object* obj = jbox_get_object(self->box);
    t_symbol* classname = object_classname(obj);
    py_newstr(py_retval(), classname->s_name);
    return true;
}

// Method: get_object() - Get underlying Max object
static bool Box_get_object(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    if (!self->box) {
        return RuntimeError("Box is null");
    }

    t_object* obj = jbox_get_object(self->box);

    if (g_object_type < 0) {
        return RuntimeError("Object type not initialized");
    }

    MaxObject* wrapper = py_newobject(py_retval(), g_object_type, 0, sizeof(MaxObject));
    wrapper->obj = obj;
    wrapper->owns_obj = false;  // Box owns the object

    return true;
}

// Method: get_rect() - Get box rectangle
static bool Box_get_rect(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    if (!self->box) {
        return RuntimeError("Box is null");
    }

    t_rect rect;
    t_max_err err = jbox_get_rect_for_view(self->box, NULL, &rect);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get box rectangle");
    }

    // Return as dict: {x, y, width, height}
    py_Ref result = py_retval();
    py_newlistn(result, 4);
    py_newfloat(py_list_getitem(result, 0), rect.x);
    py_newfloat(py_list_getitem(result, 1), rect.y);
    py_newfloat(py_list_getitem(result, 2), rect.width);
    py_newfloat(py_list_getitem(result, 3), rect.height);

    return true;
}

// Method: set_rect(x, y, width, height)
static bool Box_set_rect(int argc, py_Ref argv) {
    if (argc != 5) {
        return TypeError("set_rect() takes 4 arguments (x, y, width, height)");
    }

    BoxObject* self = py_touserdata(py_arg(0));

    if (!self->box) {
        return RuntimeError("Box is null");
    }

    t_rect rect;
    rect.x = py_tofloat(py_arg(1));
    rect.y = py_tofloat(py_arg(2));
    rect.width = py_tofloat(py_arg(3));
    rect.height = py_tofloat(py_arg(4));

    t_max_err err = jbox_set_rect_for_view(self->box, NULL, &rect);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set box rectangle");
    }

    py_newnone(py_retval());
    return true;
}

// Method: pointer()
static bool Box_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BoxObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->box;
    py_newint(py_retval(), ptr);
    return true;
}

#endif // API_BOX_H
