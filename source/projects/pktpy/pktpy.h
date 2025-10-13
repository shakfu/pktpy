// pktpy.h

#ifndef PKTPY_H
#define PKTPY_H


// ----------------------------------------------------------------------------
// Includes

// max api
#include "ext.h"
#include "ext_obex.h"

// pocketpy
#include "pocketpy.h"

// ----------------------------------------------------------------------------
// Constants

#define PY_MAX_ELEMS 1024
#define ITER_SUCCESS 1
#define ITER_STOP 0
#define ITER_FAILURE (-1)

// ----------------------------------------------------------------------------
// Macros

#define py_checklist(self) py_checktype(self, tp_list)
#define py_checktuple(self) py_checktype(self, tp_tuple)
#define py_checkdict(self) py_checktype(self, tp_dict)

// ----------------------------------------------------------------------------
// Globals

t_class* pktpy_class; // global pointer to object class

// ----------------------------------------------------------------------------
// Datastructures

typedef struct t_pktpy t_pktpy;

// ----------------------------------------------------------------------------
// Object creation and destruction Methods

void* pktpy_new(t_symbol* s, long argc, t_atom* argv);
void pktpy_free(t_pktpy* x);
void pktpy_init(t_pktpy* x);

// ----------------------------------------------------------------------------
// Attribute Getters / Setters and Helpers

t_max_err pktpy_name_get(t_pktpy *x, t_object *attr, long *argc, t_atom **argv);
t_max_err pktpy_name_set(t_pktpy *x, t_object *attr, long argc, t_atom *argv);

// ----------------------------------------------------------------------------
// Helpers

void pktpy_float(t_pktpy *x, double f);

// ----------------------------------------------------------------------------
// Path helpers

t_max_err pktpy_locate_path_from_symbol(t_pktpy* x, t_symbol* s);

// ----------------------------------------------------------------------------
// Side-effect helpers

void pktpy_bang(t_pktpy* x);
void pktpy_bang_success(t_pktpy* x);
void pktpy_bang_failure(t_pktpy* x);

// ----------------------------------------------------------------------------
// Common handlers

t_max_err pktpy_handle_float_output(t_pktpy* x, py_GlobalRef pfloat);
t_max_err pktpy_handle_long_output(t_pktpy* x, py_GlobalRef plong);
t_max_err pktpy_handle_string_output(t_pktpy* x, py_GlobalRef pstring);
t_max_err pktpy_handle_bool_output(t_pktpy* x, py_GlobalRef pbool);
t_max_err pktpy_handle_list_output(t_pktpy* x, py_GlobalRef plist);
t_max_err pktpy_handle_tuple_output(t_pktpy* x, py_GlobalRef ptuple);
t_max_err pktpy_handle_output(t_pktpy* x, py_GlobalRef retval);

// ----------------------------------------------------------------------------
// Core Python Methods

t_max_err pktpy_import(t_pktpy* x, t_symbol* s);
t_max_err pktpy_exec(t_pktpy* x, t_symbol* s, long argc, t_atom* argv);
t_max_err pktpy_eval(t_pktpy* x, t_symbol* s, long argc, t_atom* argv);
t_max_err pktpy_execfile(t_pktpy* x, t_symbol* s);

// ----------------------------------------------------------------------------
// Extra Python Methods

// t_max_err pktpy_anything(t_pktpy* x, t_symbol* s, long argc, t_atom* argv);

// ----------------------------------------------------------------------------
// Information Methods

// void pktpy_count(t_pktpy* x);
// void pktpy_metadata(t_pktpy* x);
// void pktpy_assist(t_pktpy* x, void* b, long m, long a, char* s);

// ----------------------------------------------------------------------------
// Time-based Methods

// t_max_err pktpy_task(t_pktpy* x);
// t_max_err pktpy_sched(t_pktpy* x, t_symbol* s, long argc, t_atom* argv);

// ----------------------------------------------------------------------------
// Interobject Methods

// t_max_err pktpy_send(t_pktpy* x, t_symbol* s, long argc, t_atom* argv);
// void pktpy_scan(t_pktpy* x);
// long pktpy_scan_callback(t_pktpy* x, t_object* obj);

// ----------------------------------------------------------------------------
// Code editor Methods

void pktpy_read(t_pktpy* x, t_symbol* s);
void pktpy_doread(t_pktpy* x, t_symbol* s, long argc, t_atom* argv);
void pktpy_load(t_pktpy* x, t_symbol* s); // read(f) -> execfile(f)
void pktpy_dblclick(t_pktpy* x);
void pktpy_run(t_pktpy* x);
void pktpy_edclose(t_pktpy* x, char** text, long size);
t_max_err pktpy_edsave(t_pktpy* x, char** text, long size);
void pktpy_okclose(t_pktpy* x, char *s, short *result);

// ----------------------------------------------------------------------------
// max datastructure support methods

// ----------------------------------------------------------------------------



#endif // PKTPY_H
