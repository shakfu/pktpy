// pktpy_api.h

#ifndef PKTPY_API_H
#define PKTPY_API_H

// ----------------------------------------------------------------------------
// includes

// max api
#include "ext.h"
#include "ext_obex.h"

// pocketpy
#include "pocketpy.h"

// Max atomarray
#include "ext_atomarray.h"

// Max dictionary
#include "ext_dictionary.h"

// Max patcher
#include "jpatcher_api.h"

// Max hashtab
#include "ext_hashtab.h"

// Max linklist
#include "ext_linklist.h"

// MSP buffer
#include "ext_buffer.h"

// ----------------------------------------------------------------------------
// API wrappers
#include "api/api_symbol.h"
#include "api/api_atom.h"
#include "api/api_atomarray.h"
#include "api/api_object.h"
#include "api/api_box.h"
#include "api/api_dictionary.h"
#include "api/api_patcher.h"
#include "api/api_patchline.h"
#include "api/api_hashtab.h"
#include "api/api_linklist.h"
#include "api/api_clock.h"
#include "api/api_buffer.h"
#include "api/api_outlet.h"
#include "api/api_inlet.h"
#include "api/api_table.h"
#include "api/api_path.h"
#include "api/api_database.h"
#include "api/api_external.h"
#include "api/api_qelem.h"
#include "api/api_systhread.h"
#include "api/api_preset.h"
#include "api/api_time.h"
#include "api/api_message.h"

// ----------------------------------------------------------------------------
// custom pktpy2 functions

bool int_add(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_int);
    py_i64 a = py_toint(py_arg(0));
    py_i64 b = py_toint(py_arg(1));
    py_newint(py_retval(), a + b);
    return true;
}

void print_to_console(const char * content)
{
    post(content);
}

t_max_err demo(void) {
    // Hello world!
    bool ok = py_exec("print('Hello world!')", "<string>", EXEC_MODE, NULL);
    if(!ok) goto __ERROR;

    // Create a list: [1, 2, 3]
    py_Ref r0 = py_getreg(0);
    py_newlistn(r0, 3);
    py_newint(py_list_getitem(r0, 0), 1);
    py_newint(py_list_getitem(r0, 1), 2);
    py_newint(py_list_getitem(r0, 2), 3);

    // Eval the sum of the list
    py_Ref f_sum = py_getbuiltin(py_name("sum"));
    py_push(f_sum);
    py_pushnil();
    py_push(r0);
    ok = py_vectorcall(1, 0);
    if(!ok) goto __ERROR;

    post("Sum of the list: %d\n", (int)py_toint(py_retval()));  // 6

    // Bind native `int_add` as a global variable
    py_newnativefunc(r0, int_add);
    py_setglobal(py_name("add"), r0);

    // Call `add` in python
    ok = py_exec("add(3, 7)", "<string>", EVAL_MODE, NULL);
    if(!ok) goto __ERROR;

    py_i64 res = py_toint(py_retval());
    post("Sum of 2 variables: %d\n", (int)res);  // 10

    return MAX_ERR_NONE;

__ERROR:
    py_printexc();
    return MAX_ERR_GENERIC;
}

// ============================================================================
// api module

// ----------------------------------------------------------------------------
// functions


static bool api_post(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* cstr = py_tostr(py_arg(0));
    post(cstr);
    return true;
}

static bool api_error(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);
    const char* cstr = py_tostr(py_arg(0));
    error(cstr);
    return true;
}

// ----------------------------------------------------------------------------
// Person (demo code)


typedef struct t_person {
    int id;
    int age;
} t_person;

static void t_person__ctor(t_person* self, int id, int age) {
    self->id = id;
    self->age = age;
}

static bool Person__new__(int argc, py_Ref argv) {
    t_person* ptr = py_newobject(py_retval(), py_totype(argv), 0, sizeof(t_person));
    t_person__ctor(ptr, 0, 0); // init both to 0
    return true;
}

static bool Person__init__(int argc, py_Ref argv) {
    post("Person__init__ argc = %d", argc);
    if (argc == 1) {
        // do nothing
    }
    else if(argc == 3) {
        t_person* ptr = py_touserdata(py_arg(0));
        PY_CHECK_ARG_TYPE(1, tp_int);
        py_i64 id = py_toint(py_arg(1));
        PY_CHECK_ARG_TYPE(2, tp_int);
        py_i64 age = py_toint(py_arg(2));
        t_person__ctor(ptr, id, age);
    } else {
        return TypeError("Person__init__(): expected 0 or 2 arguments, got %d", argc - 1);
    }
    py_newnone(py_retval()); // return None
    return true;
}

static bool Person__id(int argc, py_Ref argv) {
    t_person* ptr = py_touserdata(py_arg(0));
    py_newint(py_retval(), ptr->id);
    return true;
}

static bool Person__age(int argc, py_Ref argv) {
    t_person* ptr = py_touserdata(py_arg(0));
    py_newint(py_retval(), ptr->age);
    return true;
}

static bool Person__set_id(int argc, py_Ref argv) {
    t_person* ptr = py_touserdata(py_arg(0));
    if (argc == 2) {
        ptr->id = py_toint(py_arg(1));
        return true;
    }
    return TypeError("Person__set_id(): expected 1 arguments, got %d", argc - 1);
}

static bool Person__set_age(int argc, py_Ref argv) {
    t_person* ptr = py_touserdata(py_arg(0));
    if (argc == 2) {
        ptr->age = py_toint(py_arg(1));
        return true;
    }
    return TypeError("Person__set_age(): expected 1 arguments, got %d", argc - 1);
}



// ----------------------------------------------------------------------------
// Forward declarations for global type variables

// Store type IDs globally so functions can access them
py_Type g_symbol_type = -1;
py_Type g_atom_type = -1;
py_Type g_atomarray_type = -1;
py_Type g_dictionary_type = -1;
py_Type g_object_type = -1;
py_Type g_patcher_type = -1;
py_Type g_patchline_type = -1;
py_Type g_box_type = -1;
py_Type g_hashtab_type = -1;
py_Type g_linklist_type = -1;
py_Type g_buffer_type = -1;
py_Type g_clock_type = -1;
py_Type g_outlet_type = -1;
py_Type g_inlet_type = -1;
py_Type g_table_type = -1;
py_Type g_path_type = -1;
py_Type g_database_type = -1;
py_Type g_dbresult_type = -1;
py_Type g_pyexternal_type = -1;
py_Type g_qelem_type = -1;
py_Type g_systhread_type = -1;
py_Type g_sysmutex_type = -1;
py_Type g_itm_type = -1;

// Forward declarations for utility functions
static bool py_to_atom(py_Ref py_val, t_atom* atom);
static bool atom_to_py(t_atom* atom);



// ----------------------------------------------------------------------------
// Utility Functions - Python to Max conversions

// Convert Python value to Atom
static bool py_to_atom(py_Ref py_val, t_atom* atom) {
    if (py_isint(py_val)) {
        atom_setlong(atom, py_toint(py_val));
        return true;
    } else if (py_isfloat(py_val)) {
        atom_setfloat(atom, py_tofloat(py_val));
        return true;
    } else if (py_isstr(py_val)) {
        atom_setsym(atom, gensym(py_tostr(py_val)));
        return true;
    }
    return false;
}

// Convert Atom to Python value (returns via py_retval)
static bool atom_to_py(t_atom* atom) {
    switch (atom_gettype(atom)) {
        case A_LONG:
            py_newint(py_retval(), atom_getlong(atom));
            return true;
        case A_FLOAT:
            py_newfloat(py_retval(), atom_getfloat(atom));
            return true;
        case A_SYM:
            py_newstr(py_retval(), atom_getsym(atom)->s_name);
            return true;
        default:
            return false;
    }
}

// ----------------------------------------------------------------------------
// Max API Function Wrappers

static bool api_gensym(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    const char* str = py_tostr(py_arg(0));
    t_symbol* sym = gensym(str);

    // Create and return a SymbolObject using cached type
    if (g_symbol_type < 0) {
        return RuntimeError("Symbol type not initialized");
    }

    SymbolObject* obj = py_newobject(py_retval(), g_symbol_type, 0, sizeof(SymbolObject));
    obj->sym = sym;

    return true;
}

// Module-level atom conversion functions that take Atom objects
static bool api_atom_getlong(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);

    // Check if argument is an Atom
    if (g_atom_type < 0 || !py_checktype(py_arg(0), g_atom_type)) {
        return TypeError("atom_getlong() requires an Atom object");
    }

    AtomObject* atom_obj = py_touserdata(py_arg(0));
    t_atom_long val = atom_getlong(&atom_obj->atom);
    py_newint(py_retval(), val);
    return true;
}

static bool api_atom_getfloat(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);

    // Check if argument is an Atom
    if (g_atom_type < 0 || !py_checktype(py_arg(0), g_atom_type)) {
        return TypeError("atom_getfloat() requires an Atom object");
    }

    AtomObject* atom_obj = py_touserdata(py_arg(0));
    t_atom_float val = atom_getfloat(&atom_obj->atom);
    py_newfloat(py_retval(), val);
    return true;
}

static bool api_atom_getsym(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);

    // Check if argument is an Atom
    if (g_atom_type < 0 || !py_checktype(py_arg(0), g_atom_type)) {
        return TypeError("atom_getsym() requires an Atom object");
    }

    AtomObject* atom_obj = py_touserdata(py_arg(0));
    t_symbol* sym = atom_getsym(&atom_obj->atom);

    // Create and return a SymbolObject
    if (g_symbol_type < 0) {
        return RuntimeError("Symbol type not initialized");
    }

    SymbolObject* obj = py_newobject(py_retval(), g_symbol_type, 0, sizeof(SymbolObject));
    obj->sym = sym;

    return true;
}

// ----------------------------------------------------------------------------
// utils

static bool print_args(int argc, py_Ref argv) {
    py_ObjectRef tuple_item;
    py_i64 long_item;
    py_TValue* args = py_tuple_data(argv);
    int tuple_len = py_tuple_len(argv);
    post("tuple_len: %d", tuple_len);
    PY_CHECK_ARGC(1); // 1 arg: `*args` tuple
    PY_CHECK_ARG_TYPE(0, tp_tuple);
    for(int i = 0; i < tuple_len; i++) {
        tuple_item = py_tuple_getitem(py_arg(0), i);
        long_item = py_toint(tuple_item);
        post("%d: %d", i, (int)long_item);
    }
    py_newnone(py_retval());
    return true;
}



// ----------------------------------------------------------------------------
// Atom parsing functions

// Module-level parse function
static bool api_parse(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    const char* parsestr = py_tostr(py_arg(0));

    // Parse string into atoms
    t_atom* av = NULL;
    long ac = 0;
    t_max_err err = atom_setparse(&ac, &av, parsestr);

    if (err != MAX_ERR_NONE) {
        if (av) sysmem_freeptr(av);
        py_newstr(py_retval(), "Failed to parse string");
        return false;
    }

    // Create AtomArray to hold result
    py_newobject(py_retval(), g_atomarray_type, 0, sizeof(AtomArrayObject));
    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(py_retval());

    // Create atomarray and copy atoms
    arr_obj->atomarray = atomarray_new(ac, av);
    arr_obj->owns_atomarray = true;

    // Free temporary atoms
    if (av) sysmem_freeptr(av);

    return true;
}

// AtomArray.from_parse() class method
static bool AtomArray_from_parse(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    const char* parsestr = py_tostr(py_arg(0));

    // Parse string into atoms
    t_atom* av = NULL;
    long ac = 0;
    t_max_err err = atom_setparse(&ac, &av, parsestr);

    if (err != MAX_ERR_NONE) {
        if (av) sysmem_freeptr(av);
        py_newstr(py_retval(), "Failed to parse string");
        return false;
    }

    // Create AtomArray to hold result
    py_newobject(py_retval(), g_atomarray_type, 0, sizeof(AtomArrayObject));
    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(py_retval());

    // Create atomarray and copy atoms
    arr_obj->atomarray = atomarray_new(ac, av);
    arr_obj->owns_atomarray = true;

    // Free temporary atoms
    if (av) sysmem_freeptr(av);

    return true;
}


// ----------------------------------------------------------------------------
// Object registration and notification functions

// object_register() - Register object in namespace
static bool api_object_register(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);  // namespace
    PY_CHECK_ARG_TYPE(1, tp_str);  // name
    PY_CHECK_ARG_TYPE(2, tp_int);  // object pointer

    const char* ns_str = py_tostr(py_arg(0));
    const char* name_str = py_tostr(py_arg(1));
    py_i64 obj_ptr = py_toint(py_arg(2));

    t_symbol* ns = gensym(ns_str);
    t_symbol* name = gensym(name_str);
    void* obj = (void*)obj_ptr;

    void* registered = object_register(ns, name, obj);

    // Return pointer to registered object (may be different from input)
    py_newint(py_retval(), (py_i64)registered);
    return true;
}

// object_unregister() - Unregister object
static bool api_object_unregister(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer

    py_i64 obj_ptr = py_toint(py_arg(0));
    void* obj = (void*)obj_ptr;

    t_max_err err = object_unregister(obj);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to unregister object");
        return false;
    }

    py_newnone(py_retval());
    return true;
}

// object_findregistered() - Find registered object by namespace and name
static bool api_object_findregistered(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str);  // namespace
    PY_CHECK_ARG_TYPE(1, tp_str);  // name

    const char* ns_str = py_tostr(py_arg(0));
    const char* name_str = py_tostr(py_arg(1));

    t_symbol* ns = gensym(ns_str);
    t_symbol* name = gensym(name_str);

    void* obj = object_findregistered(ns, name);

    if (obj == NULL) {
        py_newnone(py_retval());
    } else {
        py_newint(py_retval(), (py_i64)obj);
    }

    return true;
}

// object_findregisteredbyptr() - Find namespace and name by object pointer
static bool api_object_findregisteredbyptr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer

    py_i64 obj_ptr = py_toint(py_arg(0));
    void* obj = (void*)obj_ptr;

    t_symbol* ns = NULL;
    t_symbol* name = NULL;

    t_max_err err = object_findregisteredbyptr(&ns, &name, obj);

    if (err != MAX_ERR_NONE || ns == NULL || name == NULL) {
        py_newnone(py_retval());
        return true;
    }

    // Return tuple (namespace, name)
    py_newlistn(py_retval(), 2);
    py_Ref item0 = py_list_getitem(py_retval(), 0);
    py_Ref item1 = py_list_getitem(py_retval(), 1);
    py_newstr(item0, ns->s_name);
    py_newstr(item1, name->s_name);

    return true;
}

// object_attach() - Attach client to registered object
static bool api_object_attach(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);  // namespace
    PY_CHECK_ARG_TYPE(1, tp_str);  // name
    PY_CHECK_ARG_TYPE(2, tp_int);  // client object pointer

    const char* ns_str = py_tostr(py_arg(0));
    const char* name_str = py_tostr(py_arg(1));
    py_i64 client_ptr = py_toint(py_arg(2));

    t_symbol* ns = gensym(ns_str);
    t_symbol* name = gensym(name_str);
    void* client = (void*)client_ptr;

    void* registered_obj = object_attach(ns, name, client);

    if (registered_obj == NULL) {
        py_newnone(py_retval());
    } else {
        py_newint(py_retval(), (py_i64)registered_obj);
    }

    return true;
}

// object_detach() - Detach client from registered object
static bool api_object_detach(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);  // namespace
    PY_CHECK_ARG_TYPE(1, tp_str);  // name
    PY_CHECK_ARG_TYPE(2, tp_int);  // client object pointer

    const char* ns_str = py_tostr(py_arg(0));
    const char* name_str = py_tostr(py_arg(1));
    py_i64 client_ptr = py_toint(py_arg(2));

    t_symbol* ns = gensym(ns_str);
    t_symbol* name = gensym(name_str);
    void* client = (void*)client_ptr;

    t_max_err err = object_detach(ns, name, client);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to detach from object");
        return false;
    }

    py_newnone(py_retval());
    return true;
}

// object_attach_byptr() - Attach to registered object by pointer
static bool api_object_attach_byptr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // client object pointer
    PY_CHECK_ARG_TYPE(1, tp_int);  // registered object pointer

    py_i64 client_ptr = py_toint(py_arg(0));
    py_i64 registered_ptr = py_toint(py_arg(1));

    void* client = (void*)client_ptr;
    void* registered_obj = (void*)registered_ptr;

    t_max_err err = object_attach_byptr(client, registered_obj);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to attach by pointer");
        return false;
    }

    py_newnone(py_retval());
    return true;
}

// object_detach_byptr() - Detach from registered object by pointer
static bool api_object_detach_byptr(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // client object pointer
    PY_CHECK_ARG_TYPE(1, tp_int);  // registered object pointer

    py_i64 client_ptr = py_toint(py_arg(0));
    py_i64 registered_ptr = py_toint(py_arg(1));

    void* client = (void*)client_ptr;
    void* registered_obj = (void*)registered_ptr;

    t_max_err err = object_detach_byptr(client, registered_obj);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to detach by pointer");
        return false;
    }

    py_newnone(py_retval());
    return true;
}

// object_notify() - Send notification to attached clients
static bool api_object_notify(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_str);  // message symbol
    PY_CHECK_ARG_TYPE(2, tp_int);  // data pointer (can be NULL/0)

    py_i64 obj_ptr = py_toint(py_arg(0));
    const char* msg_str = py_tostr(py_arg(1));
    py_i64 data_ptr = py_toint(py_arg(2));

    void* obj = (void*)obj_ptr;
    t_symbol* msg = gensym(msg_str);
    void* data = (void*)data_ptr;

    t_max_err err = object_notify(obj, msg, data);

    if (err != MAX_ERR_NONE) {
        py_newstr(py_retval(), "Failed to notify");
        return false;
    }

    py_newnone(py_retval());
    return true;
}


// ----------------------------------------------------------------------------
// Defer functions (module-level)

// Defer callback storage
typedef struct {
    py_Ref callback;
    py_Ref symbol;
    py_Ref atomarray;
} DeferData;

// Bridge function for defer
static void defer_callback_bridge(DeferData* data, t_symbol* s, short argc, t_atom* argv) {
    if (data == NULL || data->callback == NULL) return;

    // Call Python callback with symbol and atomarray
    py_push(data->callback);
    py_pushnil();
    py_push(data->symbol);
    py_push(data->atomarray);
    bool ok = py_vectorcall(2, 0);

    if (!ok) {
        py_printexc();
    }

    // Free defer data
    sysmem_freeptr(data);
}

// api.defer(owner_ptr, callback, symbol, atomarray)
static bool api_defer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_int);   // owner pointer
    // argv[1] is callback
    PY_CHECK_ARG_TYPE(2, tp_str);   // symbol
    // argv[3] is atomarray

    py_i64 owner_ptr = py_toint(py_arg(0));
    const char* sym_str = py_tostr(py_arg(2));
    t_symbol* sym = gensym(sym_str);

    // Get atomarray
    py_Ref arr_ref = py_arg(3);
    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(arr_ref);
    long ac;
    t_atom* av;
    atomarray_getatoms(arr_obj->atomarray, &ac, &av);

    // Create defer data (will be freed in bridge)
    DeferData* data = (DeferData*)sysmem_newptr(sizeof(DeferData));
    data->callback = py_arg(1);
    data->symbol = py_arg(2);
    data->atomarray = py_arg(3);

    // Keep references
    py_setslot(py_arg(0), 0, data->callback);
    py_setslot(py_arg(0), 1, data->symbol);
    py_setslot(py_arg(0), 2, data->atomarray);

    defer((void*)owner_ptr, (method)defer_callback_bridge, sym, (short)ac, av);

    py_newnone(py_retval());
    return true;
}

// api.defer_low(owner_ptr, callback, symbol, atomarray)
static bool api_defer_low(int argc, py_Ref argv) {
    PY_CHECK_ARGC(4);
    PY_CHECK_ARG_TYPE(0, tp_int);   // owner pointer
    // argv[1] is callback
    PY_CHECK_ARG_TYPE(2, tp_str);   // symbol
    // argv[3] is atomarray

    py_i64 owner_ptr = py_toint(py_arg(0));
    const char* sym_str = py_tostr(py_arg(2));
    t_symbol* sym = gensym(sym_str);

    // Get atomarray
    py_Ref arr_ref = py_arg(3);
    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(arr_ref);
    long ac;
    t_atom* av;
    atomarray_getatoms(arr_obj->atomarray, &ac, &av);

    // Create defer data (will be freed in bridge)
    DeferData* data = (DeferData*)sysmem_newptr(sizeof(DeferData));
    data->callback = py_arg(1);
    data->symbol = py_arg(2);
    data->atomarray = py_arg(3);

    // Keep references
    py_setslot(py_arg(0), 0, data->callback);
    py_setslot(py_arg(0), 1, data->symbol);
    py_setslot(py_arg(0), 2, data->atomarray);

    defer_low((void*)owner_ptr, (method)defer_callback_bridge, sym, (short)ac, av);

    py_newnone(py_retval());
    return true;
}


// ----------------------------------------------------------------------------
// Additional atom utility functions

// atom_gettext() - Convert atoms to text string
static bool api_atom_gettext(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);

    // Get AtomArray
    py_Ref arr_ref = py_arg(0);
    if (py_typeof(arr_ref) != g_atomarray_type) {
        py_newstr(py_retval(), "Argument must be AtomArray");
        return false;
    }

    AtomArrayObject* arr_obj = (AtomArrayObject*)py_touserdata(arr_ref);
    if (arr_obj->atomarray == NULL) {
        py_newstr(py_retval(), "AtomArray is null");
        return false;
    }

    long ac;
    t_atom* av;
    atomarray_getatoms(arr_obj->atomarray, &ac, &av);

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

// ----------------------------------------------------------------------------
// initialize

bool api_module_initialize(void) {
    py_GlobalRef mod = py_newmodule("api");

    // Basic Max console functions
    py_bindfunc(mod, "post", api_post);
    py_bindfunc(mod, "error", api_error);

    // Utility functions
    py_bind(mod, "print_args(*args)", print_args);

    // Symbol type
    g_symbol_type = py_newtype("Symbol", tp_object, mod, NULL);
    py_bindmethod(g_symbol_type, "__new__", Symbol__new__);
    py_bindmethod(g_symbol_type, "__init__", Symbol__init__);
    py_bindmethod(g_symbol_type, "__str__", Symbol__str__);
    py_bindmethod(g_symbol_type, "__repr__", Symbol__repr__);
    py_bindmethod(g_symbol_type, "__eq__", Symbol__eq__);
    py_bindproperty(g_symbol_type, "name", Symbol_get_name, NULL);

    // Now bind gensym after Symbol type is created
    py_bindfunc(mod, "gensym", api_gensym);

    // Atom type
    g_atom_type = py_newtype("Atom", tp_object, mod, NULL);
    py_bindmethod(g_atom_type, "__new__", Atom__new__);
    py_bindmethod(g_atom_type, "__init__", Atom__init__);
    py_bindmethod(g_atom_type, "__repr__", Atom__repr__);
    py_bindmethod(g_atom_type, "__str__", Atom__str__);
    py_bindmethod(g_atom_type, "__int__", Atom__int__);
    py_bindmethod(g_atom_type, "__float__", Atom__float__);
    py_bindproperty(g_atom_type, "type", Atom_get_type, NULL);
    py_bindproperty(g_atom_type, "value", Atom_get_value, Atom_set_value);
    py_bindmethod(g_atom_type, "is_long", Atom_is_long);
    py_bindmethod(g_atom_type, "is_float", Atom_is_float);
    py_bindmethod(g_atom_type, "is_symbol", Atom_is_symbol);
    // Max atom conversion methods
    py_bindmethod(g_atom_type, "getlong", Atom_getlong);
    py_bindmethod(g_atom_type, "getfloat", Atom_getfloat);
    py_bindmethod(g_atom_type, "getsym", Atom_getsym);

    // Max atom conversion functions (module-level)
    py_bindfunc(mod, "atom_getlong", api_atom_getlong);
    py_bindfunc(mod, "atom_getfloat", api_atom_getfloat);
    py_bindfunc(mod, "atom_getsym", api_atom_getsym);

    // Atom parsing functions (module-level)
    py_bindfunc(mod, "parse", api_parse);

    // Atom utility functions (module-level)
    py_bindfunc(mod, "atom_gettext", api_atom_gettext);

    // Object registration and notification functions (module-level)
    py_bindfunc(mod, "object_register", api_object_register);
    py_bindfunc(mod, "object_unregister", api_object_unregister);
    py_bindfunc(mod, "object_findregistered", api_object_findregistered);
    py_bindfunc(mod, "object_findregisteredbyptr", api_object_findregisteredbyptr);
    py_bindfunc(mod, "object_attach", api_object_attach);
    py_bindfunc(mod, "object_detach", api_object_detach);
    py_bindfunc(mod, "object_attach_byptr", api_object_attach_byptr);
    py_bindfunc(mod, "object_detach_byptr", api_object_detach_byptr);
    py_bindfunc(mod, "object_notify", api_object_notify);

    // Scheduling functions (module-level)
    py_bindfunc(mod, "defer", api_defer);
    py_bindfunc(mod, "defer_low", api_defer_low);

    // Clock type
    g_clock_type = py_newtype("Clock", tp_object, mod, (py_Dtor)Clock__del__);
    py_bindmethod(g_clock_type, "__new__", Clock__new__);
    py_bindmethod(g_clock_type, "__init__", Clock__init__);
    py_bindmethod(g_clock_type, "__repr__", Clock__repr__);
    py_bindmethod(g_clock_type, "delay", Clock_delay);
    py_bindmethod(g_clock_type, "fdelay", Clock_fdelay);
    py_bindmethod(g_clock_type, "unset", Clock_unset);
    py_bindmethod(g_clock_type, "pointer", Clock_pointer);

    // Outlet type
    g_outlet_type = py_newtype("Outlet", tp_object, mod, (py_Dtor)Outlet__del__);
    py_bindmethod(g_outlet_type, "__new__", Outlet__new__);
    py_bindmethod(g_outlet_type, "__init__", Outlet__init__);
    py_bindmethod(g_outlet_type, "__repr__", Outlet__repr__);
    py_bindmethod(g_outlet_type, "bang", Outlet_bang);
    py_bindmethod(g_outlet_type, "int", Outlet_int);
    py_bindmethod(g_outlet_type, "float", Outlet_float);
    py_bindmethod(g_outlet_type, "list", Outlet_list);
    py_bindmethod(g_outlet_type, "anything", Outlet_anything);
    py_bindmethod(g_outlet_type, "pointer", Outlet_pointer);

    // Inlet type
    g_inlet_type = py_newtype("Inlet", tp_object, mod, (py_Dtor)Inlet__del__);
    py_bindmethod(g_inlet_type, "__new__", Inlet__new__);
    py_bindmethod(g_inlet_type, "__init__", Inlet__init__);
    py_bindmethod(g_inlet_type, "__repr__", Inlet__repr__);
    py_bindmethod(g_inlet_type, "delete", Inlet_delete);
    py_bindmethod(g_inlet_type, "pointer", Inlet_pointer);
    py_bindmethod(g_inlet_type, "get_num", Inlet_get_num);
    py_bindmethod(g_inlet_type, "is_proxy", Inlet_is_proxy);
    py_bindmethod(g_inlet_type, "is_null", Inlet_is_null);

    // Inlet module-level functions
    py_bindfunc(mod, "inlet_new", api_inlet_new);
    py_bindfunc(mod, "intin", api_intin);
    py_bindfunc(mod, "floatin", api_floatin);
    py_bindfunc(mod, "proxy_new", api_proxy_new);
    py_bindfunc(mod, "proxy_getinlet", api_proxy_getinlet);
    py_bindfunc(mod, "inlet_count", api_inlet_count);
    py_bindfunc(mod, "inlet_nth", api_inlet_nth);

    // Buffer type
    g_buffer_type = py_newtype("Buffer", tp_object, mod, (py_Dtor)Buffer__del__);
    py_bindmethod(g_buffer_type, "__new__", Buffer__new__);
    py_bindmethod(g_buffer_type, "__init__", Buffer__init__);
    py_bindmethod(g_buffer_type, "__repr__", Buffer__repr__);
    py_bindmethod(g_buffer_type, "ref_new", Buffer_ref_new);
    py_bindmethod(g_buffer_type, "ref_set", Buffer_ref_set);
    py_bindmethod(g_buffer_type, "exists", Buffer_exists);
    py_bindmethod(g_buffer_type, "getobject", Buffer_getobject);
    py_bindmethod(g_buffer_type, "getinfo", Buffer_getinfo);
    py_bindmethod(g_buffer_type, "lock", Buffer_lock);
    py_bindmethod(g_buffer_type, "unlock", Buffer_unlock);
    py_bindmethod(g_buffer_type, "locksamples", Buffer_locksamples);
    py_bindmethod(g_buffer_type, "unlocksamples", Buffer_unlocksamples);
    py_bindmethod(g_buffer_type, "getchannelcount", Buffer_getchannelcount);
    py_bindmethod(g_buffer_type, "getframecount", Buffer_getframecount);
    py_bindmethod(g_buffer_type, "getsamplerate", Buffer_getsamplerate);
    py_bindmethod(g_buffer_type, "setdirty", Buffer_setdirty);
    py_bindmethod(g_buffer_type, "getfilename", Buffer_getfilename);
    py_bindmethod(g_buffer_type, "peek", Buffer_peek);
    py_bindmethod(g_buffer_type, "poke", Buffer_poke);
    py_bindmethod(g_buffer_type, "is_null", Buffer_is_null);
    py_bindmethod(g_buffer_type, "pointer", Buffer_pointer);

    // AtomArray type
    g_atomarray_type = py_newtype("AtomArray", tp_object, mod, (py_Dtor)AtomArray__del__);
    py_bindmethod(g_atomarray_type, "__new__", AtomArray__new__);
    py_bindmethod(g_atomarray_type, "__init__", AtomArray__init__);
    py_bindmethod(g_atomarray_type, "__repr__", AtomArray__repr__);
    py_bindmethod(g_atomarray_type, "__len__", AtomArray__len__);
    py_bindmethod(g_atomarray_type, "__getitem__", AtomArray__getitem__);
    py_bindmethod(g_atomarray_type, "__setitem__", AtomArray__setitem__);
    py_bindmethod(g_atomarray_type, "getsize", AtomArray_getsize);
    py_bindmethod(g_atomarray_type, "append", AtomArray_append);
    py_bindmethod(g_atomarray_type, "clear", AtomArray_clear);
    py_bindmethod(g_atomarray_type, "to_list", AtomArray_to_list);
    py_bindmethod(g_atomarray_type, "duplicate", AtomArray_duplicate);
    py_bindmethod(g_atomarray_type, "from_parse", AtomArray_from_parse);
    py_bindmethod(g_atomarray_type, "to_ints", AtomArray_to_ints);
    py_bindmethod(g_atomarray_type, "to_floats", AtomArray_to_floats);
    py_bindmethod(g_atomarray_type, "to_symbols", AtomArray_to_symbols);
    py_bindmethod(g_atomarray_type, "to_text", AtomArray_to_text);

    // Dictionary type
    g_dictionary_type = py_newtype("Dictionary", tp_object, mod, (py_Dtor)Dictionary__del__);
    py_bindmethod(g_dictionary_type, "__new__", Dictionary__new__);
    py_bindmethod(g_dictionary_type, "__init__", Dictionary__init__);
    py_bindmethod(g_dictionary_type, "__repr__", Dictionary__repr__);
    py_bindmethod(g_dictionary_type, "__len__", Dictionary__len__);
    py_bindmethod(g_dictionary_type, "__getitem__", Dictionary__getitem__);
    py_bindmethod(g_dictionary_type, "__setitem__", Dictionary__setitem__);
    py_bindmethod(g_dictionary_type, "__contains__", Dictionary__contains__);
    py_bindmethod(g_dictionary_type, "get", Dictionary_get);
    py_bindmethod(g_dictionary_type, "keys", Dictionary_keys);
    py_bindmethod(g_dictionary_type, "has_key", Dictionary_has_key);
    py_bindmethod(g_dictionary_type, "clear", Dictionary_clear);
    py_bindmethod(g_dictionary_type, "delete", Dictionary_delete);
    py_bindmethod(g_dictionary_type, "getlong", Dictionary_getlong);
    py_bindmethod(g_dictionary_type, "getfloat", Dictionary_getfloat);
    py_bindmethod(g_dictionary_type, "getstring", Dictionary_getstring);
    py_bindmethod(g_dictionary_type, "read", Dictionary_read);
    py_bindmethod(g_dictionary_type, "write", Dictionary_write);
    py_bindmethod(g_dictionary_type, "dump", Dictionary_dump);

    // Object type
    g_object_type = py_newtype("Object", tp_object, mod, (py_Dtor)Object__del__);
    py_bindmethod(g_object_type, "__new__", Object__new__);
    py_bindmethod(g_object_type, "__init__", Object__init__);
    py_bindmethod(g_object_type, "__repr__", Object__repr__);
    py_bindmethod(g_object_type, "create", Object_create);
    py_bindmethod(g_object_type, "wrap", Object_wrap);
    py_bindmethod(g_object_type, "free", Object_free_method);
    py_bindmethod(g_object_type, "is_null", Object_is_null);
    py_bindmethod(g_object_type, "classname", Object_classname);
    py_bindmethod(g_object_type, "method", Object_method);
    py_bindmethod(g_object_type, "getattr", Object_getattr);
    py_bindmethod(g_object_type, "setattr", Object_setattr);
    py_bindmethod(g_object_type, "attrnames", Object_attrnames);
    py_bindmethod(g_object_type, "pointer", Object_pointer);

    // Box type
    g_box_type = py_newtype("Box", tp_object, mod, (py_Dtor)Box__del__);
    py_bindmethod(g_box_type, "__new__", Box__new__);
    py_bindmethod(g_box_type, "__init__", Box__init__);
    py_bindmethod(g_box_type, "__repr__", Box__repr__);
    py_bindmethod(g_box_type, "wrap", Box_wrap);
    py_bindmethod(g_box_type, "is_null", Box_is_null);
    py_bindmethod(g_box_type, "classname", Box_classname);
    py_bindmethod(g_box_type, "get_object", Box_get_object);
    py_bindmethod(g_box_type, "get_rect", Box_get_rect);
    py_bindmethod(g_box_type, "set_rect", Box_set_rect);
    py_bindmethod(g_box_type, "pointer", Box_pointer);

    // Patcher type
    g_patcher_type = py_newtype("Patcher", tp_object, mod, (py_Dtor)Patcher__del__);
    py_bindmethod(g_patcher_type, "__new__", Patcher__new__);
    py_bindmethod(g_patcher_type, "__init__", Patcher__init__);
    py_bindmethod(g_patcher_type, "__repr__", Patcher__repr__);
    py_bindmethod(g_patcher_type, "wrap", Patcher_wrap);
    py_bindmethod(g_patcher_type, "is_null", Patcher_is_null);
    py_bindmethod(g_patcher_type, "get_firstobject", Patcher_get_firstobject);
    py_bindmethod(g_patcher_type, "get_lastobject", Patcher_get_lastobject);
    py_bindmethod(g_patcher_type, "newobject", Patcher_newobject);
    py_bindmethod(g_patcher_type, "deleteobj", Patcher_deleteobj);
    py_bindmethod(g_patcher_type, "set_locked", Patcher_set_locked);
    py_bindmethod(g_patcher_type, "get_title", Patcher_get_title);
    py_bindmethod(g_patcher_type, "set_title", Patcher_set_title);
    py_bindmethod(g_patcher_type, "get_rect", Patcher_get_rect);
    py_bindmethod(g_patcher_type, "set_rect", Patcher_set_rect);
    py_bindmethod(g_patcher_type, "get_parentpatcher", Patcher_get_parentpatcher);
    py_bindmethod(g_patcher_type, "get_toppatcher", Patcher_get_toppatcher);
    py_bindmethod(g_patcher_type, "set_dirty", Patcher_set_dirty);
    py_bindmethod(g_patcher_type, "count", Patcher_count);
    py_bindmethod(g_patcher_type, "pointer", Patcher_pointer);
    py_bindmethod(g_patcher_type, "get_firstline", Patcher_get_firstline);
    py_bindmethod(g_patcher_type, "get_name", Patcher_get_name);
    py_bindmethod(g_patcher_type, "get_filepath", Patcher_get_filepath);
    py_bindmethod(g_patcher_type, "get_filename", Patcher_get_filename);
    py_bindmethod(g_patcher_type, "get_boxes", Patcher_get_boxes);
    py_bindmethod(g_patcher_type, "get_lines", Patcher_get_lines);

    // Patchline type
    g_patchline_type = py_newtype("Patchline", tp_object, mod, (py_Dtor)Patchline__del__);
    py_bindmethod(g_patchline_type, "__new__", Patchline__new__);
    py_bindmethod(g_patchline_type, "__init__", Patchline__init__);
    py_bindmethod(g_patchline_type, "__repr__", Patchline__repr__);
    py_bindmethod(g_patchline_type, "wrap", Patchline_wrap);
    py_bindmethod(g_patchline_type, "is_null", Patchline_is_null);
    py_bindmethod(g_patchline_type, "get_box1", Patchline_get_box1);
    py_bindmethod(g_patchline_type, "get_box2", Patchline_get_box2);
    py_bindmethod(g_patchline_type, "get_outletnum", Patchline_get_outletnum);
    py_bindmethod(g_patchline_type, "get_inletnum", Patchline_get_inletnum);
    py_bindmethod(g_patchline_type, "get_startpoint", Patchline_get_startpoint);
    py_bindmethod(g_patchline_type, "get_endpoint", Patchline_get_endpoint);
    py_bindmethod(g_patchline_type, "get_hidden", Patchline_get_hidden);
    py_bindmethod(g_patchline_type, "set_hidden", Patchline_set_hidden);
    py_bindmethod(g_patchline_type, "get_nextline", Patchline_get_nextline);
    py_bindmethod(g_patchline_type, "pointer", Patchline_pointer);

    // Hashtab type
    g_hashtab_type = py_newtype("Hashtab", tp_object, mod, (py_Dtor)Hashtab__del__);
    py_bindmethod(g_hashtab_type, "__new__", Hashtab__new__);
    py_bindmethod(g_hashtab_type, "__init__", Hashtab__init__);
    py_bindmethod(g_hashtab_type, "__repr__", Hashtab__repr__);
    py_bindmethod(g_hashtab_type, "__len__", Hashtab__len__);
    py_bindmethod(g_hashtab_type, "__contains__", Hashtab__contains__);
    py_bindmethod(g_hashtab_type, "__getitem__", Hashtab__getitem__);
    py_bindmethod(g_hashtab_type, "__setitem__", Hashtab__setitem__);
    py_bindmethod(g_hashtab_type, "wrap", Hashtab_wrap);
    py_bindmethod(g_hashtab_type, "is_null", Hashtab_is_null);
    py_bindmethod(g_hashtab_type, "store", Hashtab_store);
    py_bindmethod(g_hashtab_type, "lookup", Hashtab_lookup);
    py_bindmethod(g_hashtab_type, "delete", Hashtab_delete);
    py_bindmethod(g_hashtab_type, "clear", Hashtab_clear);
    py_bindmethod(g_hashtab_type, "keys", Hashtab_keys);
    py_bindmethod(g_hashtab_type, "has_key", Hashtab_has_key);
    py_bindmethod(g_hashtab_type, "getsize", Hashtab_getsize);
    py_bindmethod(g_hashtab_type, "pointer", Hashtab_pointer);

    // Linklist type
    g_linklist_type = py_newtype("Linklist", tp_object, mod, (py_Dtor)Linklist__del__);
    py_bindmethod(g_linklist_type, "__new__", Linklist__new__);
    py_bindmethod(g_linklist_type, "__init__", Linklist__init__);
    py_bindmethod(g_linklist_type, "__repr__", Linklist__repr__);
    py_bindmethod(g_linklist_type, "__len__", Linklist__len__);
    py_bindmethod(g_linklist_type, "__getitem__", Linklist__getitem__);
    py_bindmethod(g_linklist_type, "wrap", Linklist_wrap);
    py_bindmethod(g_linklist_type, "is_null", Linklist_is_null);
    py_bindmethod(g_linklist_type, "append", Linklist_append);
    py_bindmethod(g_linklist_type, "insertindex", Linklist_insertindex);
    py_bindmethod(g_linklist_type, "getindex", Linklist_getindex);
    py_bindmethod(g_linklist_type, "chuckindex", Linklist_chuckindex);
    py_bindmethod(g_linklist_type, "deleteindex", Linklist_deleteindex);
    py_bindmethod(g_linklist_type, "clear", Linklist_clear);
    py_bindmethod(g_linklist_type, "getsize", Linklist_getsize);
    py_bindmethod(g_linklist_type, "reverse", Linklist_reverse);
    py_bindmethod(g_linklist_type, "rotate", Linklist_rotate);
    py_bindmethod(g_linklist_type, "shuffle", Linklist_shuffle);
    py_bindmethod(g_linklist_type, "swap", Linklist_swap);
    py_bindmethod(g_linklist_type, "pointer", Linklist_pointer);

    // Table type
    g_table_type = py_newtype("Table", tp_object, mod, (py_Dtor)Table__del__);
    py_bindmethod(g_table_type, "__new__", Table__new__);
    py_bindmethod(g_table_type, "__init__", Table__init__);
    py_bindmethod(g_table_type, "__repr__", Table__repr__);
    py_bindmethod(g_table_type, "__len__", Table__len__);
    py_bindmethod(g_table_type, "__getitem__", Table__getitem__);
    py_bindmethod(g_table_type, "__setitem__", Table__setitem__);
    py_bindmethod(g_table_type, "bind", Table_bind);
    py_bindmethod(g_table_type, "refresh", Table_refresh);
    py_bindmethod(g_table_type, "get", Table_get);
    py_bindmethod(g_table_type, "set", Table_set);
    py_bindmethod(g_table_type, "size", Table_size);
    py_bindmethod(g_table_type, "is_bound", Table_is_bound);
    py_bindmethod(g_table_type, "name", Table_name);
    py_bindmethod(g_table_type, "to_list", Table_to_list);
    py_bindmethod(g_table_type, "from_list", Table_from_list);
    py_bindmethod(g_table_type, "fill", Table_fill);
    py_bindmethod(g_table_type, "copy_from", Table_copy_from);
    py_bindmethod(g_table_type, "pointer", Table_pointer);

    // Path type
    g_path_type = py_newtype("Path", tp_object, mod, (py_Dtor)Path__del__);
    py_bindmethod(g_path_type, "__new__", Path__new__);
    py_bindmethod(g_path_type, "__init__", Path__init__);
    py_bindmethod(g_path_type, "__repr__", Path__repr__);
    py_bindmethod(g_path_type, "set_from_id", Path_set_from_id);
    py_bindmethod(g_path_type, "get_id", Path_get_id);
    py_bindmethod(g_path_type, "get_path", Path_get_path);
    py_bindmethod(g_path_type, "is_set", Path_is_set);

    // Path/file module-level functions
    py_bindfunc(mod, "path_getdefault", api_path_getdefault);
    py_bindfunc(mod, "path_setdefault", api_path_setdefault);
    py_bindfunc(mod, "path_getapppath", api_path_getapppath);
    py_bindfunc(mod, "locatefile_extended", api_locatefile_extended);
    py_bindfunc(mod, "path_toabsolutesystempath", api_path_toabsolutesystempath);
    py_bindfunc(mod, "path_nameconform", api_path_nameconform);
    py_bindfunc(mod, "path_opensysfile", api_path_opensysfile);
    py_bindfunc(mod, "path_createsysfile", api_path_createsysfile);
    py_bindfunc(mod, "path_closesysfile", api_path_closesysfile);
    py_bindfunc(mod, "sysfile_read", api_sysfile_read);
    py_bindfunc(mod, "sysfile_write", api_sysfile_write);
    py_bindfunc(mod, "sysfile_geteof", api_sysfile_geteof);
    py_bindfunc(mod, "sysfile_seteof", api_sysfile_seteof);
    py_bindfunc(mod, "sysfile_getpos", api_sysfile_getpos);
    py_bindfunc(mod, "sysfile_setpos", api_sysfile_setpos);
    py_bindfunc(mod, "sysfile_readtextfile", api_sysfile_readtextfile);
    py_bindfunc(mod, "path_deletefile", api_path_deletefile);

    // Path constants
    py_Ref r0 = py_getreg(0);

    // Path styles
    py_newint(r0, PATH_STYLE_MAX);
    py_setglobal(py_name("PATH_STYLE_MAX"), r0);
    py_newint(r0, PATH_STYLE_NATIVE);
    py_setglobal(py_name("PATH_STYLE_NATIVE"), r0);
    py_newint(r0, PATH_STYLE_SLASH);
    py_setglobal(py_name("PATH_STYLE_SLASH"), r0);

    // Path types
    py_newint(r0, PATH_TYPE_ABSOLUTE);
    py_setglobal(py_name("PATH_TYPE_ABSOLUTE"), r0);
    py_newint(r0, PATH_TYPE_RELATIVE);
    py_setglobal(py_name("PATH_TYPE_RELATIVE"), r0);
    py_newint(r0, PATH_TYPE_BOOT);
    py_setglobal(py_name("PATH_TYPE_BOOT"), r0);
    py_newint(r0, PATH_TYPE_PATH);
    py_setglobal(py_name("PATH_TYPE_PATH"), r0);

    // File permissions
    py_newint(r0, PATH_READ_PERM);
    py_setglobal(py_name("PATH_READ_PERM"), r0);
    py_newint(r0, PATH_WRITE_PERM);
    py_setglobal(py_name("PATH_WRITE_PERM"), r0);
    py_newint(r0, PATH_RW_PERM);
    py_setglobal(py_name("PATH_RW_PERM"), r0);

    // File position modes
    py_newint(r0, SYSFILE_ATMARK);
    py_setglobal(py_name("SYSFILE_ATMARK"), r0);
    py_newint(r0, SYSFILE_FROMSTART);
    py_setglobal(py_name("SYSFILE_FROMSTART"), r0);
    py_newint(r0, SYSFILE_FROMLEOF);
    py_setglobal(py_name("SYSFILE_FROMLEOF"), r0);

    // Database type
    g_database_type = py_newtype("Database", tp_object, mod, (py_Dtor)Database__del__);
    py_bindmethod(g_database_type, "__new__", Database__new__);
    py_bindmethod(g_database_type, "__init__", Database__init__);
    py_bindmethod(g_database_type, "__repr__", Database__repr__);
    py_bindmethod(g_database_type, "open", Database_open);
    py_bindmethod(g_database_type, "close", Database_close);
    py_bindmethod(g_database_type, "query", Database_query);
    py_bindmethod(g_database_type, "transaction_start", Database_transaction_start);
    py_bindmethod(g_database_type, "transaction_end", Database_transaction_end);
    py_bindmethod(g_database_type, "transaction_flush", Database_transaction_flush);
    py_bindmethod(g_database_type, "get_last_insert_id", Database_get_last_insert_id);
    py_bindmethod(g_database_type, "create_table", Database_create_table);
    py_bindmethod(g_database_type, "add_column", Database_add_column);
    py_bindmethod(g_database_type, "is_open", Database_is_open);
    py_bindmethod(g_database_type, "pointer", Database_pointer);

    // DBResult type
    g_dbresult_type = py_newtype("DBResult", tp_object, mod, (py_Dtor)DBResult__del__);
    py_bindmethod(g_dbresult_type, "__new__", DBResult__new__);
    py_bindmethod(g_dbresult_type, "__init__", DBResult__init__);
    py_bindmethod(g_dbresult_type, "__repr__", DBResult__repr__);
    py_bindmethod(g_dbresult_type, "__len__", DBResult__len__);
    py_bindmethod(g_dbresult_type, "numrecords", DBResult_numrecords);
    py_bindmethod(g_dbresult_type, "numfields", DBResult_numfields);
    py_bindmethod(g_dbresult_type, "fieldname", DBResult_fieldname);
    py_bindmethod(g_dbresult_type, "get_string", DBResult_get_string);
    py_bindmethod(g_dbresult_type, "get_long", DBResult_get_long);
    py_bindmethod(g_dbresult_type, "get_float", DBResult_get_float);
    py_bindmethod(g_dbresult_type, "get_record", DBResult_get_record);
    py_bindmethod(g_dbresult_type, "to_list", DBResult_to_list);
    py_bindmethod(g_dbresult_type, "reset", DBResult_reset);
    py_bindmethod(g_dbresult_type, "clear", DBResult_clear);

    // External type - wrapper for pktpy external object
    g_pyexternal_type = py_newtype("External", tp_object, mod, NULL);
    py_bindmethod(g_pyexternal_type, "__new__", External__new__);
    py_bindmethod(g_pyexternal_type, "__init__", External__init__);
    py_bindmethod(g_pyexternal_type, "__str__", External__str__);
    py_bindmethod(g_pyexternal_type, "__repr__", External__repr__);
    py_bindmethod(g_pyexternal_type, "get_pointer", External_get_pointer);
    py_bindmethod(g_pyexternal_type, "is_valid", External_is_valid);
    py_bindmethod(g_pyexternal_type, "get_outlet_left", External_get_outlet_left);
    py_bindmethod(g_pyexternal_type, "get_outlet_middle", External_get_outlet_middle);
    py_bindmethod(g_pyexternal_type, "get_outlet_right", External_get_outlet_right);
    py_bindmethod(g_pyexternal_type, "get_owner", External_get_owner);
    py_bindmethod(g_pyexternal_type, "get_patcher", External_get_patcher);
    py_bindmethod(g_pyexternal_type, "get_name", External_get_name);
    py_bindmethod(g_pyexternal_type, "post", External_post);
    py_bindmethod(g_pyexternal_type, "bang_left", External_bang_left);
    py_bindmethod(g_pyexternal_type, "out", External_out);

    // Qelem type - queue-based defer
    g_qelem_type = py_newtype("Qelem", tp_object, mod, (py_Dtor)Qelem__del__);
    py_bindmethod(g_qelem_type, "__new__", Qelem__new__);
    py_bindmethod(g_qelem_type, "__init__", Qelem__init__);
    py_bindmethod(g_qelem_type, "__repr__", Qelem__repr__);
    py_bindmethod(g_qelem_type, "set", Qelem_set);
    py_bindmethod(g_qelem_type, "unset", Qelem_unset);
    py_bindmethod(g_qelem_type, "is_set", Qelem_is_set);
    py_bindmethod(g_qelem_type, "is_null", Qelem_is_null);
    py_bindmethod(g_qelem_type, "pointer", Qelem_pointer);
    py_bindmethod(g_qelem_type, "front", Qelem_front);

    // SysThread type - thread management
    g_systhread_type = py_newtype("SysThread", tp_object, mod, (py_Dtor)SysThread__del__);
    py_bindmethod(g_systhread_type, "__new__", SysThread__new__);
    py_bindmethod(g_systhread_type, "__init__", SysThread__init__);
    py_bindmethod(g_systhread_type, "__repr__", SysThread__repr__);
    py_bindmethod(g_systhread_type, "start", SysThread_start);
    py_bindmethod(g_systhread_type, "join", SysThread_join);
    py_bindmethod(g_systhread_type, "is_running", SysThread_is_running);
    py_bindmethod(g_systhread_type, "get_result", SysThread_get_result);
    py_bindmethod(g_systhread_type, "sleep", SysThread_sleep);

    // SysMutex type - mutex/lock management
    g_sysmutex_type = py_newtype("SysMutex", tp_object, mod, (py_Dtor)SysMutex__del__);
    py_bindmethod(g_sysmutex_type, "__new__", SysMutex__new__);
    py_bindmethod(g_sysmutex_type, "__init__", SysMutex__init__);
    py_bindmethod(g_sysmutex_type, "__repr__", SysMutex__repr__);
    py_bindmethod(g_sysmutex_type, "lock", SysMutex_lock);
    py_bindmethod(g_sysmutex_type, "unlock", SysMutex_unlock);
    py_bindmethod(g_sysmutex_type, "trylock", SysMutex_trylock);
    py_bindmethod(g_sysmutex_type, "is_locked", SysMutex_is_locked);
    py_bindmethod(g_sysmutex_type, "pointer", SysMutex_pointer);

    // ITM (Time) type
    g_itm_type = py_newtype("ITM", tp_object, mod, (py_Dtor)ITM__del__);
    py_bindmethod(g_itm_type, "__new__", ITM__new__);
    py_bindmethod(g_itm_type, "__init__", ITM__init__);
    py_bindmethod(g_itm_type, "__repr__", ITM__repr__);
    py_bindmethod(g_itm_type, "getticks", ITM_getticks);
    py_bindmethod(g_itm_type, "gettime", ITM_gettime);
    py_bindmethod(g_itm_type, "getstate", ITM_getstate);
    py_bindmethod(g_itm_type, "tickstoms", ITM_tickstoms);
    py_bindmethod(g_itm_type, "mstoticks", ITM_mstoticks);
    py_bindmethod(g_itm_type, "mstosamps", ITM_mstosamps);
    py_bindmethod(g_itm_type, "sampstoms", ITM_sampstoms);
    py_bindmethod(g_itm_type, "bbutoticsk", ITM_bbutoticsk);
    py_bindmethod(g_itm_type, "tickstobbu", ITM_tickstobbu);
    py_bindmethod(g_itm_type, "pause", ITM_pause);
    py_bindmethod(g_itm_type, "resume", ITM_resume);
    py_bindmethod(g_itm_type, "seek", ITM_seek);
    py_bindmethod(g_itm_type, "settimesignature", ITM_settimesignature);
    py_bindmethod(g_itm_type, "gettimesignature", ITM_gettimesignature);
    py_bindmethod(g_itm_type, "dump", ITM_dump);
    py_bindmethod(g_itm_type, "sync", ITM_sync);
    py_bindmethod(g_itm_type, "pointer", ITM_pointer);
    py_bindmethod(g_itm_type, "is_valid", ITM_is_valid);

    // ITM module-level functions
    py_bindfunc(mod, "itm_getglobal", itm_getglobal_func);
    py_bindfunc(mod, "itm_setresolution", itm_setresolution_func);
    py_bindfunc(mod, "itm_getresolution", itm_getresolution_func);

    // Preset module-level functions
    py_bindfunc(mod, "preset_store", preset_store_func);
    py_bindfunc(mod, "preset_set", preset_set_func);
    py_bindfunc(mod, "preset_int", preset_int_func);
    py_bindfunc(mod, "preset_get_data_symbol", preset_get_data_symbol);

    // Message sending module-level functions
    py_bindfunc(mod, "typedmess", typedmess_func);
    py_bindfunc(mod, "send_message", send_message_func);
    py_bindfunc(mod, "send_bang", send_bang_func);
    py_bindfunc(mod, "send_int", send_int_func);
    py_bindfunc(mod, "send_float", send_float_func);
    py_bindfunc(mod, "send_symbol", send_symbol_func);
    py_bindfunc(mod, "send_list", send_list_func);
    py_bindfunc(mod, "send_anything", send_anything_func);

    // Person type (demo code)
    py_Type person_type = py_newtype("Person", tp_object, mod, NULL);
    py_bindmethod(person_type, "__new__", Person__new__);
    py_bindmethod(person_type, "__init__", Person__init__);
    py_bindproperty(person_type, "id", Person__id, Person__set_id);
    py_bindproperty(person_type, "age", Person__age, Person__set_age);

    return true;
}




// ----------------------------------------------------------------------------

#endif // PKTPY_API_H
