// api_message.h
// Message sending wrapper for pktpy API
//
// Direct message dispatch to Max objects

#ifndef API_MESSAGE_H
#define API_MESSAGE_H

#include "api_common.h"

// ----------------------------------------------------------------------------
// Message sending functions

// Send typed message to object
// typedmess(object_ptr, message_name, [arg1, arg2, ...])
static bool typedmess_func(int argc, py_Ref argv) {
    if (argc < 2) {
        return TypeError("typedmess() requires at least 2 arguments (object, message), got %d", argc);
    }

    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_str);  // message name

    py_i64 obj_ptr = py_toint(py_arg(0));
    const char* msg_name = py_tostr(py_arg(1));

    t_object* obj = (t_object*)obj_ptr;
    t_symbol* msg = gensym(msg_name);

    // Convert remaining arguments to t_atom array
    int msg_argc = argc - 2;
    t_atom* msg_argv = NULL;

    if (msg_argc > 0) {
        msg_argv = (t_atom*)getbytes(msg_argc * sizeof(t_atom));
        if (msg_argv == NULL) {
            return ValueError("Failed to allocate memory for message arguments");
        }

        for (int i = 0; i < msg_argc; i++) {
            py_Ref arg = py_arg(i + 2);

            if (py_isint(arg)) {
                atom_setlong(&msg_argv[i], py_toint(arg));
            } else if (py_isfloat(arg)) {
                atom_setfloat(&msg_argv[i], py_tofloat(arg));
            } else if (py_isstr(arg)) {
                atom_setsym(&msg_argv[i], gensym(py_tostr(arg)));
            } else {
                // Unsupported type - free and error
                freebytes(msg_argv, msg_argc * sizeof(t_atom));
                return TypeError("Unsupported argument type at index %d", i);
            }
        }
    }

    // Send message
    void* result = typedmess(obj, msg, msg_argc, msg_argv);

    // Free argument array
    if (msg_argv) {
        freebytes(msg_argv, msg_argc * sizeof(t_atom));
    }

    // Return result pointer as integer
    if (result) {
        py_newint(py_retval(), (py_i64)result);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Send message with atom array
// send_message(object_ptr, message_name, atom_array)
static bool send_message_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_str);  // message name

    py_i64 obj_ptr = py_toint(py_arg(0));
    const char* msg_name = py_tostr(py_arg(1));
    py_Ref atom_array_ref = py_arg(2);

    t_object* obj = (t_object*)obj_ptr;
    t_symbol* msg = gensym(msg_name);

    // Check if arg is an AtomArray
    if (!py_istype(atom_array_ref, g_atomarray_type)) {
        return TypeError("Third argument must be an AtomArray");
    }

    // Get AtomArray data
    AtomArrayObject* aa_obj = (AtomArrayObject*)py_touserdata(atom_array_ref);
    t_atomarray* aa = aa_obj->atomarray;

    if (aa == NULL) {
        return ValueError("AtomArray is NULL");
    }

    long ac;
    t_atom* av;
    atomarray_getatoms(aa, &ac, &av);

    // Send message
    void* result = typedmess(obj, msg, (short)ac, av);

    // Return result
    if (result) {
        py_newint(py_retval(), (py_i64)result);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Send bang to object
static bool send_bang_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer

    py_i64 obj_ptr = py_toint(py_arg(0));
    t_object* obj = (t_object*)obj_ptr;

    void* result = typedmess(obj, gensym("bang"), 0, NULL);

    if (result) {
        py_newint(py_retval(), (py_i64)result);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Send int to object
static bool send_int_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_int);  // value

    py_i64 obj_ptr = py_toint(py_arg(0));
    py_i64 val = py_toint(py_arg(1));

    t_object* obj = (t_object*)obj_ptr;

    t_atom a;
    atom_setlong(&a, val);

    void* result = typedmess(obj, gensym("int"), 1, &a);

    if (result) {
        py_newint(py_retval(), (py_i64)result);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Send float to object
static bool send_float_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer

    py_i64 obj_ptr = py_toint(py_arg(0));
    double val = py_tofloat(py_arg(1));

    t_object* obj = (t_object*)obj_ptr;

    t_atom a;
    atom_setfloat(&a, val);

    void* result = typedmess(obj, gensym("float"), 1, &a);

    if (result) {
        py_newint(py_retval(), (py_i64)result);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Send symbol to object
static bool send_symbol_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_str);  // symbol name

    py_i64 obj_ptr = py_toint(py_arg(0));
    const char* sym_name = py_tostr(py_arg(1));

    t_object* obj = (t_object*)obj_ptr;

    t_atom a;
    atom_setsym(&a, gensym(sym_name));

    void* result = typedmess(obj, gensym("symbol"), 1, &a);

    if (result) {
        py_newint(py_retval(), (py_i64)result);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Send list to object
static bool send_list_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer

    py_i64 obj_ptr = py_toint(py_arg(0));
    py_Ref list_ref = py_arg(1);

    t_object* obj = (t_object*)obj_ptr;

    if (!py_istype(list_ref, tp_list)) {
        return TypeError("Second argument must be a list");
    }

    // Convert Python list to t_atom array
    int list_len = py_list_len(list_ref);
    t_atom* atoms = (t_atom*)getbytes(list_len * sizeof(t_atom));

    if (atoms == NULL) {
        return ValueError("Failed to allocate memory for list");
    }

    for (int i = 0; i < list_len; i++) {
        py_Ref item = py_list_getitem(list_ref, i);

        if (py_isint(item)) {
            atom_setlong(&atoms[i], py_toint(item));
        } else if (py_isfloat(item)) {
            atom_setfloat(&atoms[i], py_tofloat(item));
        } else if (py_isstr(item)) {
            atom_setsym(&atoms[i], gensym(py_tostr(item)));
        } else {
            freebytes(atoms, list_len * sizeof(t_atom));
            return TypeError("Unsupported list element type at index %d", i);
        }
    }

    // Send list message
    void* result = typedmess(obj, gensym("list"), list_len, atoms);

    // Free atoms
    freebytes(atoms, list_len * sizeof(t_atom));

    if (result) {
        py_newint(py_retval(), (py_i64)result);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Send anything message
static bool send_anything_func(int argc, py_Ref argv) {
    if (argc < 2) {
        return TypeError("send_anything() requires at least 2 arguments (object, selector), got %d", argc);
    }

    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_str);  // selector

    py_i64 obj_ptr = py_toint(py_arg(0));
    const char* selector = py_tostr(py_arg(1));

    t_object* obj = (t_object*)obj_ptr;
    t_symbol* sel = gensym(selector);

    // Convert remaining arguments
    int msg_argc = argc - 2;
    t_atom* msg_argv = NULL;

    if (msg_argc > 0) {
        msg_argv = (t_atom*)getbytes(msg_argc * sizeof(t_atom));
        if (msg_argv == NULL) {
            return ValueError("Failed to allocate memory for arguments");
        }

        for (int i = 0; i < msg_argc; i++) {
            py_Ref arg = py_arg(i + 2);

            if (py_isint(arg)) {
                atom_setlong(&msg_argv[i], py_toint(arg));
            } else if (py_isfloat(arg)) {
                atom_setfloat(&msg_argv[i], py_tofloat(arg));
            } else if (py_isstr(arg)) {
                atom_setsym(&msg_argv[i], gensym(py_tostr(arg)));
            } else {
                freebytes(msg_argv, msg_argc * sizeof(t_atom));
                return TypeError("Unsupported argument type at index %d", i);
            }
        }
    }

    // Send anything message
    void* result = typedmess(obj, sel, msg_argc, msg_argv);

    if (msg_argv) {
        freebytes(msg_argv, msg_argc * sizeof(t_atom));
    }

    if (result) {
        py_newint(py_retval(), (py_i64)result);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

#endif // API_MESSAGE_H
