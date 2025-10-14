// api_preset.h
// Preset and Pattr wrapper for pktpy API
//
// State management integration with Max's preset system

#ifndef API_PRESET_H
#define API_PRESET_H

#include "api_common.h"

// ----------------------------------------------------------------------------
// Preset module functions (not a class, just module-level functions)

// Store preset data
static bool preset_store_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_str);

    const char* fmt = py_tostr(py_arg(0));
    preset_store((char*)fmt);

    py_newnone(py_retval());
    return true;
}

// Set preset value for an object
static bool preset_set_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_int);  // value

    py_i64 obj_ptr = py_toint(py_arg(0));
    py_i64 val = py_toint(py_arg(1));

    t_object* obj = (t_object*)obj_ptr;
    preset_set(obj, (t_atom_long)val);

    py_newnone(py_retval());
    return true;
}

// Send preset int to object
static bool preset_int_func(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);  // object pointer
    PY_CHECK_ARG_TYPE(1, tp_int);  // value

    py_i64 obj_ptr = py_toint(py_arg(0));
    py_i64 n = py_toint(py_arg(1));

    t_object* obj = (t_object*)obj_ptr;
    preset_int(obj, (t_atom_long)n);

    py_newnone(py_retval());
    return true;
}

// Get preset data symbol
static bool preset_get_data_symbol(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);

    // Return the preset_data symbol name
    py_newstr(py_retval(), "preset_data");
    return true;
}

#endif // API_PRESET_H
