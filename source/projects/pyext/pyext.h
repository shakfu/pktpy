// pyext.h
// Python external - write Max externals in Python

#ifndef PYEXT_H
#define PYEXT_H

// ----------------------------------------------------------------------------
// includes

// max api
#include "ext.h"
#include "ext_obex.h"
#include "ext_path.h"

// pocketpy
#include "pocketpy.h"

// ----------------------------------------------------------------------------
// constants

#define PYEXT_MAX_INLETS 16
#define PYEXT_MAX_OUTLETS 16

// ----------------------------------------------------------------------------
// data structure

typedef struct t_pyext {
    t_object ob;                              // Max object header

    // Python instance
    py_GlobalRef py_instance;                 // Python object instance
    py_Type py_class_type;                    // Python class type

    // Script info
    t_symbol* script_name;                    // Script filename
    char script_filename[MAX_PATH_CHARS];     // Resolved filename
    char script_pathname[MAX_PATH_CHARS];     // Full path
    short script_path;                        // Max path ID
    t_fourcc script_filetype;                 // File type four char code
    t_fourcc script_outtype;                  // Output type four char code

    // Text editor
    t_object* code_editor;                    // Code editor object
    char** code;                              // Handle to code buffer
    long code_size;                           // Length of code buffer
    long run_on_save;                         // Reload script on editor save
    long run_on_close;                        // Reload script on editor close

    // Inlets and outlets
    long num_inlets;                          // Number of inlets
    long num_outlets;                         // Number of outlets
    void* inlets[PYEXT_MAX_INLETS];           // Inlet proxies
    void* outlets[PYEXT_MAX_OUTLETS];         // Outlet pointers
    long inlet_num;                           // Current inlet number

} t_pyext;

// ----------------------------------------------------------------------------
// function declarations

// lifecycle
void ext_main(void* r);
void* pyext_new(t_symbol* s, long argc, t_atom* argv);
void pyext_free(t_pyext* x);
t_max_err pyext_init(t_pyext* x);

// message handlers
void pyext_bang(t_pyext* x);
void pyext_int(t_pyext* x, long n);
void pyext_float(t_pyext* x, double f);
void pyext_list(t_pyext* x, t_symbol* s, long argc, t_atom* argv);
void pyext_anything(t_pyext* x, t_symbol* s, long argc, t_atom* argv);

// text editor
void pyext_dblclick(t_pyext* x);
void pyext_edclose(t_pyext* x, char** text, long size);
t_max_err pyext_edsave(t_pyext* x, char** text, long size);
void pyext_okclose(t_pyext* x, char* s, short* result);
void pyext_read(t_pyext* x, t_symbol* s);
void pyext_doread(t_pyext* x, t_symbol* s, long argc, t_atom* argv);

// script management
t_max_err pyext_load_script(t_pyext* x, t_symbol* script_name);
t_max_err pyext_reload(t_pyext* x);

// Python method routing
t_max_err pyext_call_method(t_pyext* x, const char* method_name, long argc, t_atom* argv);
t_max_err pyext_call_method_noargs(t_pyext* x, const char* method_name);

// utility
t_max_err pyext_setup_inlets_outlets(t_pyext* x);
void pyext_error(t_pyext* x, const char* fmt, ...);

#endif // PYEXT_H
