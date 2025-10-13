// api_dictionary.h
// Dictionary wrapper for pktpy API

#ifndef API_DICTIONARY_H
#define API_DICTIONARY_H

#include "api_common.h"

// ----------------------------------------------------------------------------
// Dictionary wrapper

typedef struct {
    t_dictionary* dict;
    bool owns_dict;  // Whether we should free it
} DictionaryObject;

static bool Dictionary__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    DictionaryObject* obj = py_newobject(py_retval(), cls, 0, sizeof(DictionaryObject));
    obj->dict = dictionary_new();
    obj->owns_dict = true;
    return true;
}

static bool Dictionary__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    // Dict is already created in __new__
    py_newnone(py_retval());
    return true;
}

// Destructor
static void Dictionary__del__(void* self) {
    DictionaryObject* obj = (DictionaryObject*)self;
    if (obj->owns_dict && obj->dict) {
        object_free(obj->dict);
        obj->dict = NULL;
    }
}

static bool Dictionary__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DictionaryObject* self = py_touserdata(py_arg(0));

    t_atom_long count = dictionary_getentrycount(self->dict);
    char buf[256];
    snprintf(buf, sizeof(buf), "Dictionary(entries=%ld)", count);
    py_newstr(py_retval(), buf);
    return true;
}

static bool Dictionary__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DictionaryObject* self = py_touserdata(py_arg(0));
    t_atom_long count = dictionary_getentrycount(self->dict);
    py_newint(py_retval(), count);
    return true;
}

// __getitem__ - dict[key]
static bool Dictionary__getitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    // Try to get as atom first
    t_atom a;
    t_max_err err = dictionary_getatom(self->dict, key, &a);

    if (err == MAX_ERR_NONE) {
        // Successfully got atom, convert to Python
        if (!atom_to_py(&a)) {
            return RuntimeError("Failed to convert atom to Python value");
        }
        return true;
    }

    // Check if it's a string
    if (dictionary_entryisstring(self->dict, key)) {
        const char* value = NULL;
        err = dictionary_getstring(self->dict, key, &value);
        if (err == MAX_ERR_NONE) {
            py_newstr(py_retval(), value);
            return true;
        }
    }

    // Check if it's an atomarray
    if (dictionary_entryisatomarray(self->dict, key)) {
        t_object* atomarray_obj = NULL;
        err = dictionary_getatomarray(self->dict, key, &atomarray_obj);
        if (err == MAX_ERR_NONE && g_atomarray_type >= 0) {
            AtomArrayObject* obj = py_newobject(py_retval(), g_atomarray_type, 0, sizeof(AtomArrayObject));
            obj->atomarray = (t_atomarray*)atomarray_obj;
            obj->owns_atomarray = false;  // Dictionary owns it
            return true;
        }
    }

    // Check if it's a nested dictionary
    if (dictionary_entryisdictionary(self->dict, key)) {
        t_object* sub_dict = NULL;
        err = dictionary_getdictionary(self->dict, key, &sub_dict);
        if (err == MAX_ERR_NONE && g_dictionary_type >= 0) {
            DictionaryObject* obj = py_newobject(py_retval(), g_dictionary_type, 0, sizeof(DictionaryObject));
            obj->dict = (t_dictionary*)sub_dict;
            obj->owns_dict = false;  // Parent dictionary owns it
            return true;
        }
    }

    // Create a string for KeyError
    py_Ref key_ref = py_getreg(0);
    py_newstr(key_ref, key_str);
    return KeyError(key_ref);
}

// __setitem__ - dict[key] = value
static bool Dictionary__setitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);
    py_Ref value = py_arg(2);

    t_max_err err = MAX_ERR_GENERIC;

    // Check type and use appropriate append function
    if (py_isint(value)) {
        err = dictionary_appendlong(self->dict, key, py_toint(value));
    } else if (py_isfloat(value)) {
        err = dictionary_appendfloat(self->dict, key, py_tofloat(value));
    } else if (py_isstr(value)) {
        err = dictionary_appendstring(self->dict, key, py_tostr(value));
    } else if (py_isinstance(value, tp_list)) {
        // Convert list to atoms and append as atomarray
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

        err = dictionary_appendatoms(self->dict, key, list_len, atoms);
        sysmem_freeptr(atoms);
    } else if (g_atomarray_type >= 0 && py_checktype(value, g_atomarray_type)) {
        // AtomArray object
        AtomArrayObject* aa_obj = py_touserdata(value);
        err = dictionary_appendatomarray(self->dict, key, (t_object*)aa_obj->atomarray);
        // Note: dictionary now owns the atomarray, so mark it
        aa_obj->owns_atomarray = false;
    } else if (g_dictionary_type >= 0 && py_checktype(value, g_dictionary_type)) {
        // Dictionary object
        DictionaryObject* dict_obj = py_touserdata(value);
        err = dictionary_appenddictionary(self->dict, key, (t_object*)dict_obj->dict);
        // Note: parent dictionary now owns the child
        dict_obj->owns_dict = false;
    } else if (g_atom_type >= 0 && py_checktype(value, g_atom_type)) {
        // Atom object
        AtomObject* atom_obj = py_touserdata(value);
        err = dictionary_appendatom(self->dict, key, &atom_obj->atom);
    } else if (g_symbol_type >= 0 && py_checktype(value, g_symbol_type)) {
        // Symbol object
        SymbolObject* sym_obj = py_touserdata(value);
        err = dictionary_appendsym(self->dict, key, sym_obj->sym);
    } else {
        return TypeError("Unsupported value type for dictionary");
    }

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set dictionary value");
    }

    py_newnone(py_retval());
    return true;
}

// __contains__ - key in dict
static bool Dictionary__contains__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    long has = dictionary_hasentry(self->dict, key);
    py_newbool(py_retval(), has != 0);
    return true;
}

// Method: get(key, default=None)
static bool Dictionary_get(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("get() takes 1 or 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    // Check if key exists
    if (!dictionary_hasentry(self->dict, key)) {
        if (argc == 3) {
            // Return default value
            py_assign(py_retval(), py_arg(2));
        } else {
            // Return None
            py_newnone(py_retval());
        }
        return true;
    }

    // Key exists, use __getitem__ logic
    t_atom a;
    t_max_err err = dictionary_getatom(self->dict, key, &a);

    if (err == MAX_ERR_NONE) {
        if (!atom_to_py(&a)) {
            return RuntimeError("Failed to convert atom to Python value");
        }
        return true;
    }

    // Check other types as in __getitem__
    if (dictionary_entryisstring(self->dict, key)) {
        const char* value = NULL;
        err = dictionary_getstring(self->dict, key, &value);
        if (err == MAX_ERR_NONE) {
            py_newstr(py_retval(), value);
            return true;
        }
    }

    py_newnone(py_retval());
    return true;
}

// Method: keys()
static bool Dictionary_keys(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DictionaryObject* self = py_touserdata(py_arg(0));

    long numkeys = 0;
    t_symbol** keys = NULL;

    t_max_err err = dictionary_getkeys(self->dict, &numkeys, &keys);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get dictionary keys");
    }

    // Create Python list
    py_newlistn(py_retval(), numkeys);

    for (long i = 0; i < numkeys; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        py_newstr(item, keys[i]->s_name);
    }

    if (keys) {
        dictionary_freekeys(self->dict, numkeys, keys);
    }

    return true;
}

// Method: has_key(key)
static bool Dictionary_has_key(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    long has = dictionary_hasentry(self->dict, key);
    py_newbool(py_retval(), has != 0);
    return true;
}

// Method: clear()
static bool Dictionary_clear(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DictionaryObject* self = py_touserdata(py_arg(0));

    t_max_err err = dictionary_clear(self->dict);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to clear dictionary");
    }

    py_newnone(py_retval());
    return true;
}

// Method: delete(key)
static bool Dictionary_delete(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_max_err err = dictionary_deleteentry(self->dict, key);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to delete key '%s'", key_str);
    }

    py_newnone(py_retval());
    return true;
}

// Method: getlong(key, default=0)
static bool Dictionary_getlong(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("getlong() takes 1 or 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    t_atom_long value = 0;
    t_max_err err;

    if (argc == 3 && py_isint(py_arg(2))) {
        t_atom_long def = py_toint(py_arg(2));
        err = dictionary_getdeflong(self->dict, key, &value, def);
    } else {
        err = dictionary_getlong(self->dict, key, &value);
    }

    if (err != MAX_ERR_NONE && argc < 3) {
        py_Ref key_ref = py_getreg(0);
        py_newstr(key_ref, key_str);
        return KeyError(key_ref);
    }

    py_newint(py_retval(), value);
    return true;
}

// Method: getfloat(key, default=0.0)
static bool Dictionary_getfloat(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("getfloat() takes 1 or 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    double value = 0.0;
    t_max_err err;

    if (argc == 3 && py_isfloat(py_arg(2))) {
        double def = py_tofloat(py_arg(2));
        err = dictionary_getdeffloat(self->dict, key, &value, def);
    } else {
        err = dictionary_getfloat(self->dict, key, &value);
    }

    if (err != MAX_ERR_NONE && argc < 3) {
        py_Ref key_ref = py_getreg(0);
        py_newstr(key_ref, key_str);
        return KeyError(key_ref);
    }

    py_newfloat(py_retval(), value);
    return true;
}

// Method: getstring(key, default="")
static bool Dictionary_getstring(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("getstring() takes 1 or 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* key_str = py_tostr(py_arg(1));
    t_symbol* key = gensym(key_str);

    const char* value = NULL;
    t_max_err err = dictionary_getstring(self->dict, key, &value);

    if (err != MAX_ERR_NONE) {
        if (argc == 3 && py_isstr(py_arg(2))) {
            value = py_tostr(py_arg(2));
        } else {
            py_Ref key_ref = py_getreg(0);
            py_newstr(key_ref, key_str);
            return KeyError(key_ref);
        }
    }

    py_newstr(py_retval(), value ? value : "");
    return true;
}

// Method: read(filename, path)
static bool Dictionary_read(int argc, py_Ref argv) {
    if (argc != 3) {
        return TypeError("read() takes 2 arguments: filename and path");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);
    PY_CHECK_ARG_TYPE(2, tp_int);

    const char* filename = py_tostr(py_arg(1));
    short path = (short)py_toint(py_arg(2));

    // Clear existing dictionary and read new one
    if (self->owns_dict && self->dict) {
        object_free(self->dict);
    }

    t_dictionary* new_dict = NULL;
    t_max_err err = dictionary_read(filename, path, &new_dict);

    if (err != MAX_ERR_NONE || !new_dict) {
        return RuntimeError("Failed to read dictionary from file '%s'", filename);
    }

    self->dict = new_dict;
    self->owns_dict = true;

    py_newnone(py_retval());
    return true;
}

// Method: write(filename, path)
static bool Dictionary_write(int argc, py_Ref argv) {
    if (argc != 3) {
        return TypeError("write() takes 2 arguments: filename and path");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);
    PY_CHECK_ARG_TYPE(2, tp_int);

    const char* filename = py_tostr(py_arg(1));
    short path = (short)py_toint(py_arg(2));

    t_max_err err = dictionary_write(self->dict, filename, path);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to write dictionary to file '%s'", filename);
    }

    py_newnone(py_retval());
    return true;
}

// Method: dump(recurse=True, console=False)
static bool Dictionary_dump(int argc, py_Ref argv) {
    if (argc < 1 || argc > 3) {
        return TypeError("dump() takes 0 to 2 arguments");
    }

    DictionaryObject* self = py_touserdata(py_arg(0));

    long recurse = 1;
    long console = 0;

    if (argc >= 2) {
        recurse = py_isint(py_arg(1)) ? py_toint(py_arg(1)) : 1;
    }
    if (argc >= 3) {
        console = py_isint(py_arg(2)) ? py_toint(py_arg(2)) : 0;
    }

    t_max_err err = dictionary_dump(self->dict, recurse, console);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to dump dictionary");
    }

    py_newnone(py_retval());
    return true;
}

#endif // API_DICTIONARY_H