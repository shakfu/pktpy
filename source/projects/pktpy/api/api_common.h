// api_common.h
// Common infrastructure for pktpy API wrappers

#ifndef API_COMMON_H
#define API_COMMON_H

// ----------------------------------------------------------------------------
// Max and MSP includes

#include "ext.h"
#include "ext_obex.h"
#include "ext_atomarray.h"
#include "ext_dictionary.h"
#include "jpatcher_api.h"
#include "ext_hashtab.h"
#include "ext_linklist.h"
#include "ext_buffer.h"

// ----------------------------------------------------------------------------
// PocketPy include

#include "pocketpy.h"

// ----------------------------------------------------------------------------
// Global type declarations

// Note: These are declared here as extern, but defined in the main pktpy_api.h
// without 'static' so they have external linkage
extern py_Type g_symbol_type;
extern py_Type g_atom_type;
extern py_Type g_atomarray_type;
extern py_Type g_dictionary_type;
extern py_Type g_object_type;
extern py_Type g_patcher_type;
extern py_Type g_box_type;
extern py_Type g_hashtab_type;
extern py_Type g_linklist_type;
extern py_Type g_buffer_type;
extern py_Type g_clock_type;
extern py_Type g_outlet_type;
extern py_Type g_patchline_type;

#endif // API_COMMON_H
