// api_table.h
// Table wrapper for pktpy API

#ifndef API_TABLE_H
#define API_TABLE_H

#include "api_common.h"

// ----------------------------------------------------------------------------
// Table wrapper - Wrapper for Max table objects (named integer arrays)
//
// Max tables are named global arrays of long integers, lighter than buffers.
// They're commonly used with [table] objects for lookup tables, breakpoints, etc.

typedef struct {
    t_symbol* name;
    long** handle;      // Pointer to pointer to array of longs
    long size;          // Number of elements
    bool is_bound;      // Whether we successfully retrieved the table
} TableObject;

static bool Table__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    TableObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(TableObject));
    wrapper->name = NULL;
    wrapper->handle = NULL;
    wrapper->size = 0;
    wrapper->is_bound = false;
    return true;
}

static bool Table__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Table__del__(void* self) {
    // Note: We don't own the table data, Max manages it
    // No cleanup needed
}

static bool Table__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    TableObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->is_bound && self->name) {
        snprintf(buf, sizeof(buf), "Table(name='%s', size=%ld)",
                 self->name->s_name, self->size);
    } else if (self->name) {
        snprintf(buf, sizeof(buf), "Table(name='%s', unbound)",
                 self->name->s_name);
    } else {
        snprintf(buf, sizeof(buf), "Table(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Method: bind(name)
// Bind to a named table object
static bool Table_bind(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    TableObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* name_str = py_tostr(py_arg(1));
    self->name = gensym(name_str);

    // Try to get the table
    short result = table_get(self->name, &self->handle, &self->size);

    if (result != 0) {
        // Table doesn't exist or error
        self->is_bound = false;
        self->handle = NULL;
        self->size = 0;
        py_newbool(py_retval(), false);
        return true;
    }

    self->is_bound = true;
    py_newbool(py_retval(), true);
    return true;
}

// Method: refresh()
// Refresh the table reference (call before accessing if table might have changed)
static bool Table_refresh(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    TableObject* self = py_touserdata(py_arg(0));

    if (!self->name) {
        return RuntimeError("No table name set - call bind() first");
    }

    // Re-get the table
    short result = table_get(self->name, &self->handle, &self->size);

    if (result != 0) {
        self->is_bound = false;
        self->handle = NULL;
        self->size = 0;
        py_newbool(py_retval(), false);
        return true;
    }

    self->is_bound = true;
    py_newbool(py_retval(), true);
    return true;
}

// Method: get(index)
// Get value at index
static bool Table_get(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    TableObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->is_bound || !self->handle) {
        return RuntimeError("Table not bound - call bind() first");
    }

    long index = (long)py_toint(py_arg(1));

    // Validate index
    if (index < 0 || index >= self->size) {
        return IndexError("Table index out of range (0 to %ld)", self->size - 1);
    }

    // Dereference handle to get array, then index into it
    long value = (*self->handle)[index];
    py_newint(py_retval(), value);
    return true;
}

// Method: set(index, value)
// Set value at index
static bool Table_set(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    TableObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_int);

    if (!self->is_bound || !self->handle) {
        return RuntimeError("Table not bound - call bind() first");
    }

    long index = (long)py_toint(py_arg(1));
    long value = (long)py_toint(py_arg(2));

    // Validate index
    if (index < 0 || index >= self->size) {
        return IndexError("Table index out of range (0 to %ld)", self->size - 1);
    }

    // Dereference handle to get array, then set value
    (*self->handle)[index] = value;

    // Mark table as dirty
    table_dirty(self->name);

    py_newnone(py_retval());
    return true;
}

// Method: __len__()
// Get table size
static bool Table__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    TableObject* self = py_touserdata(py_arg(0));

    if (!self->is_bound) {
        return RuntimeError("Table not bound - call bind() first");
    }

    py_newint(py_retval(), self->size);
    return true;
}

// Method: __getitem__(index)
// Python array access syntax
static bool Table__getitem__(int argc, py_Ref argv) {
    return Table_get(argc, argv);
}

// Method: __setitem__(index, value)
// Python array assignment syntax
static bool Table__setitem__(int argc, py_Ref argv) {
    return Table_set(argc, argv);
}

// Method: size()
// Get table size (explicit method)
static bool Table_size(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    TableObject* self = py_touserdata(py_arg(0));

    if (!self->is_bound) {
        py_newint(py_retval(), 0);
        return true;
    }

    py_newint(py_retval(), self->size);
    return true;
}

// Method: is_bound()
// Check if table is bound
static bool Table_is_bound(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    TableObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->is_bound);
    return true;
}

// Method: name()
// Get table name
static bool Table_name(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    TableObject* self = py_touserdata(py_arg(0));

    if (!self->name) {
        py_newnone(py_retval());
        return true;
    }

    py_newstr(py_retval(), self->name->s_name);
    return true;
}

// Method: to_list()
// Convert entire table to Python list
static bool Table_to_list(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    TableObject* self = py_touserdata(py_arg(0));

    if (!self->is_bound || !self->handle) {
        return RuntimeError("Table not bound - call bind() first");
    }

    // Create Python list
    py_newlistn(py_retval(), (int)self->size);

    // Copy values
    for (long i = 0; i < self->size; i++) {
        py_Ref item = py_list_getitem(py_retval(), (int)i);
        py_newint(item, (*self->handle)[i]);
    }

    return true;
}

// Method: from_list(values)
// Set table contents from Python list
static bool Table_from_list(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    TableObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_list);

    if (!self->is_bound || !self->handle) {
        return RuntimeError("Table not bound - call bind() first");
    }

    py_Ref list = py_arg(1);
    int list_len = py_list_len(list);

    // Copy as many values as will fit
    long copy_len = (list_len < self->size) ? list_len : self->size;

    for (long i = 0; i < copy_len; i++) {
        py_Ref item = py_list_getitem(list, (int)i);
        if (py_isint(item)) {
            (*self->handle)[i] = (long)py_toint(item);
        } else if (py_isfloat(item)) {
            (*self->handle)[i] = (long)py_tofloat(item);
        } else {
            // Skip non-numeric values
            continue;
        }
    }

    // Mark table as dirty
    table_dirty(self->name);

    py_newint(py_retval(), copy_len);
    return true;
}

// Method: fill(value)
// Fill entire table with a value
static bool Table_fill(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    TableObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->is_bound || !self->handle) {
        return RuntimeError("Table not bound - call bind() first");
    }

    long value = (long)py_toint(py_arg(1));

    for (long i = 0; i < self->size; i++) {
        (*self->handle)[i] = value;
    }

    // Mark table as dirty
    table_dirty(self->name);

    py_newnone(py_retval());
    return true;
}

// Method: copy_from(other_table, src_offset=0, dst_offset=0, count=-1)
// Copy data from another table
static bool Table_copy_from(int argc, py_Ref argv) {
    if (argc < 2 || argc > 5) {
        return TypeError("copy_from() takes 1-4 arguments, got %d", argc - 1);
    }

    TableObject* self = py_touserdata(py_arg(0));

    if (!self->is_bound || !self->handle) {
        return RuntimeError("Destination table not bound");
    }

    // Check if arg[1] is a Table
    if (py_typeof(py_arg(1)) != g_table_type) {
        return TypeError("First argument must be a Table");
    }

    TableObject* src = py_touserdata(py_arg(1));

    if (!src->is_bound || !src->handle) {
        return RuntimeError("Source table not bound");
    }

    // Parse optional arguments
    long src_offset = 0;
    long dst_offset = 0;
    long count = -1;

    if (argc >= 3) {
        if (!py_isint(py_arg(2))) {
            return TypeError("src_offset must be int");
        }
        src_offset = (long)py_toint(py_arg(2));
    }

    if (argc >= 4) {
        if (!py_isint(py_arg(3))) {
            return TypeError("dst_offset must be int");
        }
        dst_offset = (long)py_toint(py_arg(3));
    }

    if (argc >= 5) {
        if (!py_isint(py_arg(4))) {
            return TypeError("count must be int");
        }
        count = (long)py_toint(py_arg(4));
    }

    // Validate offsets
    if (src_offset < 0 || src_offset >= src->size) {
        return IndexError("Source offset out of range");
    }
    if (dst_offset < 0 || dst_offset >= self->size) {
        return IndexError("Destination offset out of range");
    }

    // Calculate actual copy count
    long max_src = src->size - src_offset;
    long max_dst = self->size - dst_offset;
    long actual_count = (max_src < max_dst) ? max_src : max_dst;

    if (count >= 0 && count < actual_count) {
        actual_count = count;
    }

    // Copy data
    for (long i = 0; i < actual_count; i++) {
        (*self->handle)[dst_offset + i] = (*src->handle)[src_offset + i];
    }

    // Mark destination table as dirty
    table_dirty(self->name);

    py_newint(py_retval(), actual_count);
    return true;
}

// Method: pointer()
// Get pointer to the handle (for advanced use)
static bool Table_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    TableObject* self = py_touserdata(py_arg(0));

    if (!self->is_bound || !self->handle) {
        py_newint(py_retval(), 0);
        return true;
    }

    py_newint(py_retval(), (py_i64)self->handle);
    return true;
}

#endif // API_TABLE_H
