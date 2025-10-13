// api_atomarray.h
// AtomArray wrapper for pktpy API

#ifndef API_ATOMARRAY_H
#define API_ATOMARRAY_H

#include "api_common.h"
#include "api_atom.h"

// Forward declarations for utility functions
static bool py_to_atom(py_Ref py_val, t_atom* atom);
static bool atom_to_py(t_atom* atom);

// ----------------------------------------------------------------------------
// AtomArray wrapper

typedef struct {
    t_atomarray* atomarray;
    bool owns_atomarray;  // Whether we should free it
} AtomArrayObject;

static bool AtomArray__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    AtomArrayObject* obj = py_newobject(py_retval(), cls, 0, sizeof(AtomArrayObject));
    obj->atomarray = atomarray_new(0, NULL);  // Empty array
    obj->owns_atomarray = true;
    return true;
}

static bool AtomArray__init__(int argc, py_Ref argv) {
    AtomArrayObject* self = py_touserdata(py_arg(0));

    if (argc == 1) {
        // AtomArray() - empty array (already created in __new__)
        return true;
    } else if (argc == 2 && py_isinstance(py_arg(1), tp_list)) {
        // AtomArray([...]) - from Python list
        py_Ref list = py_arg(1);
        int list_len = py_list_len(list);

        // Allocate temporary atom array
        t_atom* atoms = (t_atom*)sysmem_newptr(list_len * sizeof(t_atom));
        if (!atoms) {
            return RuntimeError("Failed to allocate memory for atoms");
        }

        // Convert Python list to atoms
        for (int i = 0; i < list_len; i++) {
            py_Ref item = py_list_getitem(list, i);
            if (!py_to_atom(item, &atoms[i])) {
                sysmem_freeptr(atoms);
                return TypeError("List item %d cannot be converted to atom", i);
            }
        }

        // Set atoms in atomarray
        atomarray_setatoms(self->atomarray, list_len, atoms);
        sysmem_freeptr(atoms);

    } else {
        return TypeError("AtomArray() takes 0 or 1 argument (list)");
    }

    py_newnone(py_retval());
    return true;
}

// Destructor
static void AtomArray__del__(void* self) {
    AtomArrayObject* obj = (AtomArrayObject*)self;
    if (obj->owns_atomarray && obj->atomarray) {
        object_free(obj->atomarray);
        obj->atomarray = NULL;
    }
}

static bool AtomArray__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));

    long ac = atomarray_getsize(self->atomarray);
    char buf[256];
    snprintf(buf, sizeof(buf), "AtomArray(size=%ld)", ac);
    py_newstr(py_retval(), buf);
    return true;
}

static bool AtomArray__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    t_atom_long size = atomarray_getsize(self->atomarray);
    py_newint(py_retval(), size);
    return true;
}

static bool AtomArray__getitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 index = py_toint(py_arg(1));
    long size = atomarray_getsize(self->atomarray);

    // Handle negative indices
    if (index < 0) {
        index += size;
    }

    if (index < 0 || index >= size) {
        return IndexError("AtomArray index out of range");
    }

    // Get atom at index
    t_atom a;
    t_max_err err = atomarray_getindex(self->atomarray, index, &a);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get atom at index %lld", index);
    }

    // Create Atom object wrapper
    if (g_atom_type < 0) {
        return RuntimeError("Atom type not initialized");
    }

    AtomObject* atom_obj = py_newobject(py_retval(), g_atom_type, 0, sizeof(AtomObject));
    atom_obj->atom = a;

    return true;
}

static bool AtomArray__setitem__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    py_i64 index = py_toint(py_arg(1));
    long size = atomarray_getsize(self->atomarray);

    // Handle negative indices
    if (index < 0) {
        index += size;
    }

    if (index < 0 || index >= size) {
        return IndexError("AtomArray index out of range");
    }

    // Convert value to atom
    t_atom a;
    if (!py_to_atom(py_arg(2), &a)) {
        return TypeError("Value cannot be converted to atom");
    }

    // atomarray_setindex is not exported in Max SDK
    // We need to rebuild the array to set an item
    long ac = 0;
    t_atom* av = NULL;
    t_max_err err = atomarray_getatoms(self->atomarray, &ac, &av);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get atoms");
    }

    // Modify the atom at index
    av[index] = a;

    // Rebuild atomarray with modified atoms
    err = atomarray_setatoms(self->atomarray, ac, av);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set atoms");
    }

    py_newnone(py_retval());
    return true;
}

// Method: getsize()
static bool AtomArray_getsize(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    t_atom_long size = atomarray_getsize(self->atomarray);
    py_newint(py_retval(), size);
    return true;
}

// Method: append(atom or value)
static bool AtomArray_append(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    AtomArrayObject* self = py_touserdata(py_arg(0));

    t_atom a;
    py_Ref value = py_arg(1);

    // Check if it's an Atom object
    if (g_atom_type >= 0 && py_checktype(value, g_atom_type)) {
        AtomObject* atom_obj = py_touserdata(value);
        a = atom_obj->atom;
    } else if (!py_to_atom(value, &a)) {
        return TypeError("Value cannot be converted to atom");
    }

    atomarray_appendatom(self->atomarray, &a);
    py_newnone(py_retval());
    return true;
}

// Method: clear()
static bool AtomArray_clear(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));
    atomarray_clear(self->atomarray);
    py_newnone(py_retval());
    return true;
}

// Method: to_list() - Convert to Python list
static bool AtomArray_to_list(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));

    long ac = 0;
    t_atom* av = NULL;

    t_max_err err = atomarray_getatoms(self->atomarray, &ac, &av);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get atoms from atomarray");
    }

    // Create Python list
    py_newlistn(py_retval(), ac);

    for (int i = 0; i < ac; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        if (!atom_to_py(&av[i])) {
            return RuntimeError("Failed to convert atom %d to Python value", i);
        }
        // Copy retval to list item
        py_assign(item, py_retval());
    }

    return true;
}

// Method: duplicate() - Create a copy
static bool AtomArray_duplicate(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    AtomArrayObject* self = py_touserdata(py_arg(0));

    t_atomarray* dup = (t_atomarray*)atomarray_duplicate(self->atomarray);
    if (!dup) {
        return RuntimeError("Failed to duplicate atomarray");
    }

    // Create new AtomArrayObject wrapper
    if (g_atomarray_type < 0) {
        return RuntimeError("AtomArray type not initialized");
    }

    AtomArrayObject* new_obj = py_newobject(py_retval(), g_atomarray_type, 0, sizeof(AtomArrayObject));
    new_obj->atomarray = dup;
    new_obj->owns_atomarray = true;

    return true;
}


// AtomArray.to_ints() - Convert to Python list of integers
static bool AtomArray_to_ints(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    AtomArrayObject* self = (AtomArrayObject*)py_touserdata(py_arg(0));

    if (self->atomarray == NULL) {
        py_newstr(py_retval(), "AtomArray is null");
        return false;
    }

    long ac;
    t_atom* av;
    atomarray_getatoms(self->atomarray, &ac, &av);

    // Create Python list
    py_newlistn(py_retval(), ac);

    // Extract longs
    t_atom_long* vals = (t_atom_long*)sysmem_newptr(ac * sizeof(t_atom_long));
    atom_getlong_array(ac, av, ac, vals);

    for (long i = 0; i < ac; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        py_newint(item, (py_i64)vals[i]);
    }

    sysmem_freeptr(vals);
    return true;
}

// AtomArray.to_floats() - Convert to Python list of floats
static bool AtomArray_to_floats(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    AtomArrayObject* self = (AtomArrayObject*)py_touserdata(py_arg(0));

    if (self->atomarray == NULL) {
        py_newstr(py_retval(), "AtomArray is null");
        return false;
    }

    long ac;
    t_atom* av;
    atomarray_getatoms(self->atomarray, &ac, &av);

    // Create Python list
    py_newlistn(py_retval(), ac);

    // Extract doubles
    double* vals = (double*)sysmem_newptr(ac * sizeof(double));
    atom_getdouble_array(ac, av, ac, vals);

    for (long i = 0; i < ac; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        py_newfloat(item, vals[i]);
    }

    sysmem_freeptr(vals);
    return true;
}

// AtomArray.to_symbols() - Convert to Python list of symbol strings
static bool AtomArray_to_symbols(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    AtomArrayObject* self = (AtomArrayObject*)py_touserdata(py_arg(0));

    if (self->atomarray == NULL) {
        py_newstr(py_retval(), "AtomArray is null");
        return false;
    }

    long ac;
    t_atom* av;
    atomarray_getatoms(self->atomarray, &ac, &av);

    // Create Python list
    py_newlistn(py_retval(), ac);

    // Extract symbols
    t_symbol** vals = (t_symbol**)sysmem_newptr(ac * sizeof(t_symbol*));
    atom_getsym_array(ac, av, ac, vals);

    for (long i = 0; i < ac; i++) {
        py_Ref item = py_list_getitem(py_retval(), i);
        if (vals[i] != NULL) {
            py_newstr(item, vals[i]->s_name);
        } else {
            py_newstr(item, "");
        }
    }

    sysmem_freeptr(vals);
    return true;
}

// AtomArray.to_text() - Convert to text string
static bool AtomArray_to_text(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    AtomArrayObject* self = (AtomArrayObject*)py_touserdata(py_arg(0));

    if (self->atomarray == NULL) {
        py_newstr(py_retval(), "");
        return true;
    }

    long ac;
    t_atom* av;
    atomarray_getatoms(self->atomarray, &ac, &av);

    // Convert to text
    long textsize = 0;
    char* text = NULL;
    t_max_err err = atom_gettext(ac, av, &textsize, &text, 0);

    if (err != MAX_ERR_NONE || text == NULL) {
        py_newstr(py_retval(), "");
        return true;
    }

    py_newstr(py_retval(), text);

    // Free text allocated by atom_gettext
    if (text) sysmem_freeptr(text);

    return true;
}



#endif // API_ATOMARRAY_H
