#ifndef API_OBJECT_H
#define API_OBJECT_H

#include "api_common.h"

// ----------------------------------------------------------------------------
// Object wrapper - Generic Max object wrapper

typedef struct {
    t_object* obj;
    bool owns_obj;  // Whether we should free it
} MaxObject;

static bool Object__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    MaxObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(MaxObject));
    wrapper->obj = NULL;
    wrapper->owns_obj = false;
    return true;
}

static bool Object__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    // Object is created via create() or wrap() methods, not __init__
    py_newnone(py_retval());
    return true;
}

// Destructor
static void Object__del__(void* self) {
    MaxObject* wrapper = (MaxObject*)self;
    if (wrapper->owns_obj && wrapper->obj) {
        object_free(wrapper->obj);
        wrapper->obj = NULL;
    }
}

static bool Object__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->obj) {
        t_symbol* classname = object_classname(self->obj);
        snprintf(buf, sizeof(buf), "Object(%s, %p)", classname->s_name, self->obj);
    } else {
        snprintf(buf, sizeof(buf), "Object(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Class method: create(classname, *args)
static bool Object_create(int argc, py_Ref argv) {
    if (argc < 2) {
        return TypeError("create() requires at least 1 argument (classname)");
    }

    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* classname_str = py_tostr(py_arg(1));
    t_symbol* classname = gensym(classname_str);

    // Convert remaining Python args to atoms
    int num_args = argc - 2;
    t_atom* atoms = NULL;

    if (num_args > 0) {
        atoms = (t_atom*)sysmem_newptr(num_args * sizeof(t_atom));
        if (!atoms) {
            return RuntimeError("Failed to allocate memory for arguments");
        }

        for (int i = 0; i < num_args; i++) {
            py_Ref arg = py_arg(i + 2);
            if (!py_to_atom(arg, &atoms[i])) {
                sysmem_freeptr(atoms);
                return TypeError("Argument %d cannot be converted to atom", i);
            }
        }
    }

    // Create object
    t_object* obj = (t_object*)object_new_typed(CLASS_BOX, classname, num_args, atoms);

    if (atoms) {
        sysmem_freeptr(atoms);
    }

    if (!obj) {
        return RuntimeError("Failed to create object of class '%s'", classname_str);
    }

    // Free old object if we owned it
    if (self->owns_obj && self->obj) {
        object_free(self->obj);
    }

    self->obj = obj;
    self->owns_obj = true;

    py_newnone(py_retval());
    return true;
}

// Method: wrap(pointer) - Wrap existing object pointer (no ownership)
static bool Object_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    // Free old object if we owned it
    if (self->owns_obj && self->obj) {
        object_free(self->obj);
    }

    self->obj = (t_object*)ptr;
    self->owns_obj = false;  // Don't free wrapped objects

    py_newnone(py_retval());
    return true;
}

// Method: free()
static bool Object_free_method(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    if (self->owns_obj && self->obj) {
        object_free(self->obj);
        self->obj = NULL;
        self->owns_obj = false;
    }

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Object_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->obj == NULL);
    return true;
}

// Method: classname()
static bool Object_classname(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    t_symbol* classname = object_classname(self->obj);
    py_newstr(py_retval(), classname->s_name);
    return true;
}

// Method: method(name, *args)
static bool Object_method(int argc, py_Ref argv) {
    if (argc < 2) {
        return TypeError("method() requires at least 1 argument (method name)");
    }

    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    const char* method_name = py_tostr(py_arg(1));
    t_symbol* method_sym = gensym(method_name);

    // Convert remaining Python args to atoms
    int num_args = argc - 2;
    t_atom* atoms = NULL;
    t_atom result;

    if (num_args > 0) {
        atoms = (t_atom*)sysmem_newptr(num_args * sizeof(t_atom));
        if (!atoms) {
            return RuntimeError("Failed to allocate memory for arguments");
        }

        for (int i = 0; i < num_args; i++) {
            py_Ref arg = py_arg(i + 2);
            if (!py_to_atom(arg, &atoms[i])) {
                sysmem_freeptr(atoms);
                return TypeError("Argument %d cannot be converted to atom", i);
            }
        }
    }

    // Call method
    atom_setsym(&result, gensym(""));  // Initialize result
    t_max_err err = object_method_typed(self->obj, method_sym, num_args, atoms, &result);

    if (atoms) {
        sysmem_freeptr(atoms);
    }

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Method '%s' failed with error %d", method_name, (int)err);
    }

    // Convert result to Python
    if (!atom_to_py(&result)) {
        py_newnone(py_retval());
    }

    return true;
}

// Method: getattr(name)
static bool Object_getattr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    const char* attr_name = py_tostr(py_arg(1));
    t_symbol* attr_sym = gensym(attr_name);

    long ac = 0;
    t_atom* av = NULL;

    t_max_err err = object_attr_getvalueof(self->obj, attr_sym, &ac, &av);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get attribute '%s'", attr_name);
    }

    if (ac == 0 || !av) {
        py_newnone(py_retval());
        return true;
    }

    // If single value, return it directly
    if (ac == 1) {
        if (!atom_to_py(&av[0])) {
            py_newnone(py_retval());
        }
    } else {
        // Multiple values, return as list
        py_newlistn(py_retval(), ac);
        for (long i = 0; i < ac; i++) {
            py_Ref item = py_list_getitem(py_retval(), i);
            if (!atom_to_py(&av[i])) {
                py_newnone(item);
            } else {
                py_assign(item, py_retval());
            }
        }
    }

    return true;
}

// Method: setattr(name, value)
static bool Object_setattr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    MaxObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    const char* attr_name = py_tostr(py_arg(1));
    t_symbol* attr_sym = gensym(attr_name);
    py_Ref value = py_arg(2);

    t_max_err err = MAX_ERR_GENERIC;

    // Handle different value types
    if (py_isint(value)) {
        err = object_attr_setlong(self->obj, attr_sym, py_toint(value));
    } else if (py_isfloat(value)) {
        err = object_attr_setfloat(self->obj, attr_sym, py_tofloat(value));
    } else if (py_isstr(value)) {
        err = object_attr_setsym(self->obj, attr_sym, gensym(py_tostr(value)));
    } else if (py_isinstance(value, tp_list)) {
        // Convert list to atoms
        int list_len = py_list_len(value);
        t_atom* atoms = (t_atom*)sysmem_newptr(list_len * sizeof(t_atom));
        if (!atoms) {
            return RuntimeError("Failed to allocate memory");
        }

        for (int i = 0; i < list_len; i++) {
            py_Ref item = py_list_getitem(value, i);
            if (!py_to_atom(item, &atoms[i])) {
                sysmem_freeptr(atoms);
                return TypeError("List item %d cannot be converted to atom", i);
            }
        }

        err = object_attr_setvalueof(self->obj, attr_sym, list_len, atoms);
        sysmem_freeptr(atoms);
    } else {
        return TypeError("Unsupported value type for attribute");
    }

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set attribute '%s'", attr_name);
    }

    py_newnone(py_retval());
    return true;
}

// Method: attrnames()
static bool Object_attrnames(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    if (!self->obj) {
        return RuntimeError("Object is null");
    }

    long numattrs = 0;
    t_symbol** attrnames = NULL;

    t_max_err err = object_attr_getnames(self->obj, &numattrs, &attrnames);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get attribute names");
    }

    // Create Python list
    py_newlistn(py_retval(), numattrs);

    for (long i = 0; i < numattrs; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        py_newstr(item, attrnames[i]->s_name);
    }

    // Free the array (if needed - check Max API docs)
    if (attrnames) {
        sysmem_freeptr(attrnames);
    }

    return true;
}

// Method: pointer() - Get raw pointer value
static bool Object_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    MaxObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->obj;
    py_newint(py_retval(), ptr);
    return true;
}

#endif // API_OBJECT_H
