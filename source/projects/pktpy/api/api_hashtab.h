// api_hashtab.h
// Hashtab wrapper for pktpy API

#ifndef API_HASHTAB_H
#define API_HASHTAB_H

#include "api_common.h"


// ----------------------------------------------------------------------------
// Hashtab wrapper - Wrapper for Max hashtable objects

typedef struct {
    t_hashtab* hashtab;
    bool owns_hashtab;
} HashtabObject;

static bool Hashtab__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    HashtabObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(HashtabObject));

    // Create new hashtab with default size (or custom size if provided)
    long slotcount = 0;  // 0 = use default
    if (argc > 1 && py_isint(py_arg(1))) {
        slotcount = py_toint(py_arg(1));
    }

    wrapper->hashtab = hashtab_new(slotcount);
    wrapper->owns_hashtab = true;

    return true;
}

static bool Hashtab__init__(int argc, py_Ref argv) {
    // Already created in __new__
    py_newnone(py_retval());
    return true;
}

static void Hashtab__del__(void* self) {
    HashtabObject* wrapper = (HashtabObject*)self;
    if (wrapper->owns_hashtab && wrapper->hashtab) {
        object_free(wrapper->hashtab);
        wrapper->hashtab = NULL;
    }
}

static bool Hashtab__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->hashtab) {
        t_atom_long size = hashtab_getsize(self->hashtab);
        snprintf(buf, sizeof(buf), "Hashtab(size=%ld)", size);
    } else {
        snprintf(buf, sizeof(buf), "Hashtab(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// __len__ - Get hashtab size
static bool Hashtab__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    t_atom_long size = hashtab_getsize(self->hashtab);
    py_newint(py_retval(), size);
    return true;
}

// __contains__ - Check if key exists
static bool Hashtab__contains__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_object* val = NULL;
    t_max_err err = hashtab_lookup(self->hashtab, key, &val);

    py_newbool(py_retval(), err == MAX_ERR_NONE);
    return true;
}

// __getitem__ - Get value by key
static bool Hashtab__getitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    // Try to get as object first
    t_object* obj_val = NULL;
    t_max_err err = hashtab_lookup(self->hashtab, key, &obj_val);

    if (err == MAX_ERR_NONE && obj_val) {
        // Try as long first
        t_atom_long long_val = 0;
        err = hashtab_lookuplong(self->hashtab, key, &long_val);
        if (err == MAX_ERR_NONE) {
            py_newint(py_retval(), long_val);
            return true;
        }

        // Try as symbol
        t_symbol* sym_val = NULL;
        err = hashtab_lookupsym(self->hashtab, key, &sym_val);
        if (err == MAX_ERR_NONE && sym_val) {
            py_newstr(py_retval(), sym_val->s_name);
            return true;
        }

        // Return object pointer as int
        py_newint(py_retval(), (py_i64)obj_val);
        return true;
    }

    // Key not found
    py_Ref key_ref = py_getreg(0);
    py_newstr(key_ref, key_str);
    return KeyError(key_ref);
}

// __setitem__ - Set value by key
static bool Hashtab__setitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);
    py_Ref value = py_arg(2);

    t_max_err err = MAX_ERR_GENERIC;

    // Store based on type
    if (py_isint(value)) {
        err = hashtab_storelong(self->hashtab, key, py_toint(value));
    } else if (py_isstr(value)) {
        err = hashtab_storesym(self->hashtab, key, gensym(py_tostr(value)));
    } else if (py_isfloat(value)) {
        // Store float as object (no direct float support in hashtab)
        // We'll just store it as a long for now
        err = hashtab_storelong(self->hashtab, key, (t_atom_long)py_tofloat(value));
    } else if (g_object_type >= 0 && py_checktype(value, g_object_type)) {
        MaxObject* obj_wrapper = py_touserdata(value);
        err = hashtab_store(self->hashtab, key, obj_wrapper->obj);
    } else {
        return TypeError("Unsupported value type for hashtab");
    }

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to store value in hashtab");
    }

    py_newnone(py_retval());
    return true;
}

// Method: wrap(pointer)
static bool Hashtab_wrap(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 ptr = py_toint(py_arg(1));
    if (ptr == 0) {
        return ValueError("Cannot wrap null pointer");
    }

    // Free old hashtab if we owned it
    if (self->owns_hashtab && self->hashtab) {
        object_free(self->hashtab);
    }

    self->hashtab = (t_hashtab*)ptr;
    self->owns_hashtab = false;

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Hashtab_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->hashtab == NULL);
    return true;
}

// Method: store(key, value)
static bool Hashtab_store(int argc, py_Ref argv) {
    // Same as __setitem__
    return Hashtab__setitem__(argc, argv);
}

// Method: lookup(key, default=None)
static bool Hashtab_lookup(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("lookup() takes 1 or 2 arguments");
    }

    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_object* obj_val = NULL;
    t_max_err err = hashtab_lookup(self->hashtab, key, &obj_val);

    if (err != MAX_ERR_NONE) {
        // Key not found, return default
        if (argc == 3) {
            py_assign(py_retval(), py_arg(2));
        } else {
            py_newnone(py_retval());
        }
        return true;
    }

    // Found - try different types
    t_atom_long long_val = 0;
    if (hashtab_lookuplong(self->hashtab, key, &long_val) == MAX_ERR_NONE) {
        py_newint(py_retval(), long_val);
        return true;
    }

    t_symbol* sym_val = NULL;
    if (hashtab_lookupsym(self->hashtab, key, &sym_val) == MAX_ERR_NONE && sym_val) {
        py_newstr(py_retval(), sym_val->s_name);
        return true;
    }

    // Return object pointer as int
    py_newint(py_retval(), (py_i64)obj_val);
    return true;
}

// Method: delete(key)
static bool Hashtab_delete(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_max_err err = hashtab_delete(self->hashtab, key);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to delete key '%s'", key_str);
    }

    py_newnone(py_retval());
    return true;
}

// Method: clear()
static bool Hashtab_clear(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    t_max_err err = hashtab_clear(self->hashtab);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to clear hashtab");
    }

    py_newnone(py_retval());
    return true;
}

// Method: keys()
static bool Hashtab_keys(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    long keycount = 0;
    t_symbol** keys = NULL;

    t_max_err err = hashtab_getkeys(self->hashtab, &keycount, &keys);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get hashtab keys");
    }

    // Create Python list
    py_newlistn(py_retval(), keycount);

    for (long i = 0; i < keycount; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        if (keys[i]) {
            py_newstr(item, keys[i]->s_name);
        } else {
            py_newstr(item, "");
        }
    }

    // Note: keys array is owned by hashtab, don't free

    return true;
}

// Method: has_key(key)
static bool Hashtab_has_key(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    HashtabObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_object* val = NULL;
    t_max_err err = hashtab_lookup(self->hashtab, key, &val);

    py_newbool(py_retval(), err == MAX_ERR_NONE);
    return true;
}

// Method: getsize()
static bool Hashtab_getsize(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    if (!self->hashtab) {
        return RuntimeError("Hashtab is null");
    }

    t_atom_long size = hashtab_getsize(self->hashtab);
    py_newint(py_retval(), size);
    return true;
}

// Method: pointer()
static bool Hashtab_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    HashtabObject* self = py_touserdata(py_arg(0));

    py_i64 ptr = (py_i64)self->hashtab;
    py_newint(py_retval(), ptr);
    return true;
}

#endif // API_HASHTAB_H
