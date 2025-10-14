// pyext.c
// Python external - write Max externals in Python

#include "pyext.h"
#include "pktpy_api.h"

// ----------------------------------------------------------------------------
// globals

t_class* pyext_class = NULL;
static bool pyext_py_initialized = false;

// ----------------------------------------------------------------------------
// ext_main - external setup

void ext_main(void* r)
{
    t_class* c = class_new("pyext", (method)pyext_new, (method)pyext_free,
                           (long)sizeof(t_pyext), 0L, A_GIMME, 0);

    // Standard message handlers
    class_addmethod(c, (method)pyext_bang,     "bang",     0);
    class_addmethod(c, (method)pyext_int,      "int",      A_LONG,   0);
    class_addmethod(c, (method)pyext_float,    "float",    A_FLOAT,  0);
    class_addmethod(c, (method)pyext_list,     "list",     A_GIMME,  0);
    class_addmethod(c, (method)pyext_anything, "anything", A_GIMME,  0);

    // Text editor
    class_addmethod(c, (method)pyext_dblclick, "dblclick", A_CANT,   0);
    class_addmethod(c, (method)pyext_edclose,  "edclose",  A_CANT,   0);
    class_addmethod(c, (method)pyext_edsave,   "edsave",   A_CANT,   0);
    class_addmethod(c, (method)pyext_okclose,  "okclose",  A_CANT,   0);
    class_addmethod(c, (method)pyext_read,     "read",     A_DEFSYM, 0);

    // Script management
    class_addmethod(c, (method)pyext_reload,   "reload",   0);

    class_register(CLASS_BOX, c);
    pyext_class = c;

    post("pyext: Python external object loaded");
}

// ----------------------------------------------------------------------------
// pyext_init - initialize pocketpy (once globally)

t_max_err pyext_init(t_pyext* x)
{
    if (!pyext_py_initialized) {
        // Initialize pocketpy
        py_initialize();

        // Redirect stdout to Max console
        py_Callbacks* callbacks = py_callbacks();
        callbacks->print = print_to_console;

        // Initialize API module
        api_module_initialize();

        pyext_py_initialized = true;
        post("pyext: pocketpy initialized");
    }
    return MAX_ERR_NONE;
}

// ----------------------------------------------------------------------------
// pyext_new - object instantiation

void* pyext_new(t_symbol* s, long argc, t_atom* argv)
{
    t_pyext* x = (t_pyext*)object_alloc(pyext_class);

    if (x) {
        // Initialize pocketpy if needed
        pyext_init(x);

        // Initialize instance variables
        x->py_instance = NULL;
        x->py_class_type = -1;
        x->script_name = gensym("");
        x->script_filename[0] = 0;
        x->script_pathname[0] = 0;
        x->script_path = 0;
        x->script_filetype = FOUR_CHAR_CODE('TEXT');
        x->script_outtype = 0;
        x->num_inlets = 1;
        x->num_outlets = 1;
        x->inlet_num = 0;

        // Initialize text editor
        x->code = sysmem_newhandle(0);
        x->code_size = 0;
        x->code_editor = NULL;
        x->run_on_save = 0;
        x->run_on_close = 1;

        // Initialize arrays
        for (int i = 0; i < PYEXT_MAX_INLETS; i++) {
            x->inlets[i] = NULL;
        }
        for (int i = 0; i < PYEXT_MAX_OUTLETS; i++) {
            x->outlets[i] = NULL;
        }

        // Parse arguments - expect script name as first argument
        if (argc > 0 && atom_gettype(argv) == A_SYM) {
            x->script_name = atom_getsym(argv);

            // Try to load the script
            if (pyext_load_script(x, x->script_name) != MAX_ERR_NONE) {
                object_error((t_object*)x, "failed to load script: %s",
                           x->script_name->s_name);
                // Don't fail - allow object to be created
            }
        } else {
            object_warn((t_object*)x, "no script specified");
        }

        // Setup inlets and outlets based on Python object configuration
        pyext_setup_inlets_outlets(x);
    }

    return (x);
}

// ----------------------------------------------------------------------------
// pyext_free - cleanup

void pyext_free(t_pyext* x)
{
    // Free Python instance
    if (x->py_instance != NULL) {
        // Python objects are managed by pocketpy's GC
        x->py_instance = NULL;
    }

    // Free text editor code buffer
    if (x->code) {
        sysmem_freehandle(x->code);
    }

    // Free inlet proxies
    for (int i = 1; i < x->num_inlets; i++) {
        if (x->inlets[i] != NULL) {
            object_free(x->inlets[i]);
        }
    }
}

// ----------------------------------------------------------------------------
// pyext_load_script - load and instantiate Python script

t_max_err pyext_load_script(t_pyext* x, t_symbol* script_name)
{
    t_max_err err;
    t_filehandle fh = NULL;
    t_handle code_handle = sysmem_newhandle(0);
    long code_size = 0;

    if (script_name == gensym("")) {
        object_error((t_object*)x, "empty script name");
        goto error;
    }

    // Locate the file
    strncpy_zero(x->script_filename, script_name->s_name, MAX_PATH_CHARS);

    if (locatefile_extended(x->script_filename, &x->script_path,
                           &x->script_outtype, &x->script_filetype, 1)) {
        object_error((t_object*)x, "can't find script: %s",
                   script_name->s_name);
        goto error;
    }

    // Get absolute path
    err = path_toabsolutesystempath(x->script_path, x->script_filename,
                                   x->script_pathname);
    if (err != MAX_ERR_NONE) {
        object_error((t_object*)x, "can't get absolute path for: %s",
                   script_name->s_name);
        goto error;
    }

    // Read the script file
    err = path_opensysfile(x->script_filename, x->script_path, &fh, READ_PERM);
    if (err != MAX_ERR_NONE) {
        object_error((t_object*)x, "can't open script: %s",
                   script_name->s_name);
        goto error;
    }

    sysfile_readtextfile(fh, code_handle, 0, TEXT_LB_UNIX | TEXT_NULL_TERMINATE);
    sysfile_close(fh);
    code_size = sysmem_handlesize(code_handle);

    if (code_size == 0) {
        object_error((t_object*)x, "empty script file: %s",
                   script_name->s_name);
        goto error;
    }

    // Execute the script to define the class
    bool ok = py_exec(*code_handle, x->script_pathname, EXEC_MODE, NULL);
    if (!ok) {
        py_printexc();
        object_error((t_object*)x, "error executing script: %s",
                   script_name->s_name);
        goto error;
    }

    // Look for a class called 'External' or first class defined
    // For now, try 'External' class
    py_GlobalRef external_class = py_getglobal(py_name("External"));
    if (external_class == NULL) {
        object_error((t_object*)x, "script must define 'External' class");
        goto error;
    }

    // Create instance of the class
    py_push(external_class);
    py_pushnil();
    ok = py_vectorcall(0, 0);
    if (!ok) {
        py_printexc();
        object_error((t_object*)x, "error creating instance of External class");
        goto error;
    }

    x->py_instance = py_retval();
    x->py_class_type = py_typeof(x->py_instance);

    // Query for inlets and outlets count
    bool has_inlets = py_getattr(x->py_instance, py_name("inlets"));
    if (has_inlets) {
        py_GlobalRef inlets_attr = py_retval();
        if (py_isint(inlets_attr)) {
            x->num_inlets = py_toint(inlets_attr);
            if (x->num_inlets < 1) x->num_inlets = 1;
            if (x->num_inlets > PYEXT_MAX_INLETS) x->num_inlets = PYEXT_MAX_INLETS;
        }
    }

    bool has_outlets = py_getattr(x->py_instance, py_name("outlets"));
    if (has_outlets) {
        py_GlobalRef outlets_attr = py_retval();
        if (py_isint(outlets_attr)) {
            x->num_outlets = py_toint(outlets_attr);
            if (x->num_outlets < 1) x->num_outlets = 1;
            if (x->num_outlets > PYEXT_MAX_OUTLETS) x->num_outlets = PYEXT_MAX_OUTLETS;
        }
    }

    sysmem_freehandle(code_handle);
    object_post((t_object*)x, "loaded script: %s", script_name->s_name);
    object_post((t_object*)x, "inlets: %ld, outlets: %ld",
               x->num_inlets, x->num_outlets);

    return MAX_ERR_NONE;

error:
    if (code_handle) sysmem_freehandle(code_handle);
    return MAX_ERR_GENERIC;
}

// ----------------------------------------------------------------------------
// pyext_reload - reload the Python script

t_max_err pyext_reload(t_pyext* x)
{
    if (x->script_name == gensym("")) {
        object_error((t_object*)x, "no script to reload");
        return MAX_ERR_GENERIC;
    }

    object_post((t_object*)x, "reloading script: %s", x->script_name->s_name);

    // Clear current instance
    x->py_instance = NULL;

    // Reload the script
    return pyext_load_script(x, x->script_name);
}

// ----------------------------------------------------------------------------
// pyext_setup_inlets_outlets - create inlets and outlets

t_max_err pyext_setup_inlets_outlets(t_pyext* x)
{
    // Create outlets (right to left)
    for (long i = x->num_outlets - 1; i >= 0; i--) {
        x->outlets[i] = outlet_new((t_object*)x, NULL);
    }

    // Create inlet proxies (right to left, skip first inlet)
    for (long i = x->num_inlets - 1; i >= 1; i--) {
        x->inlets[i] = proxy_new((t_object*)x, i, &x->inlet_num);
    }

    return MAX_ERR_NONE;
}

// ----------------------------------------------------------------------------
// pyext_call_method_noargs - call Python method with no arguments

t_max_err pyext_call_method_noargs(t_pyext* x, const char* method_name)
{
    if (x->py_instance == NULL) {
        object_error((t_object*)x, "no Python instance");
        return MAX_ERR_GENERIC;
    }

    // Look up method
    bool has_method = py_getattr(x->py_instance, py_name(method_name));
    if (!has_method) {
        // Method doesn't exist - not an error, just silently return
        return MAX_ERR_NONE;
    }

    py_GlobalRef method = py_retval();
    if (!py_callable(method)) {
        // Not callable - not an error
        return MAX_ERR_NONE;
    }

    // Call the method
    py_push(method);
    py_push(x->py_instance);
    bool ok = py_vectorcall(0, 0);

    if (!ok) {
        py_printexc();
        object_error((t_object*)x, "error calling method: %s", method_name);
        return MAX_ERR_GENERIC;
    }

    return MAX_ERR_NONE;
}

// ----------------------------------------------------------------------------
// pyext_call_method - call Python method with arguments

t_max_err pyext_call_method(t_pyext* x, const char* method_name,
                           long argc, t_atom* argv)
{
    if (x->py_instance == NULL) {
        object_error((t_object*)x, "no Python instance");
        return MAX_ERR_GENERIC;
    }

    // Look up method
    bool has_method = py_getattr(x->py_instance, py_name(method_name));
    if (!has_method) {
        // Method doesn't exist - not an error, just silently return
        return MAX_ERR_NONE;
    }

    py_GlobalRef method = py_retval();
    if (!py_callable(method)) {
        // Not callable - not an error
        return MAX_ERR_NONE;
    }

    // Setup call
    py_push(method);
    py_push(x->py_instance);

    // Convert arguments
    for (long i = 0; i < argc; i++) {
        py_Ref r0 = py_getreg(0);
        switch (atom_gettype(argv + i)) {
            case A_LONG:
                py_newint(r0, atom_getlong(argv + i));
                py_push(r0);
                break;
            case A_FLOAT:
                py_newfloat(r0, atom_getfloat(argv + i));
                py_push(r0);
                break;
            case A_SYM:
                py_newstr(r0, atom_getsym(argv + i)->s_name);
                py_push(r0);
                break;
            default:
                object_warn((t_object*)x, "unsupported atom type");
                py_newnone(r0);
                py_push(r0);
                break;
        }
    }

    // Call the method
    bool ok = py_vectorcall(argc, 0);

    if (!ok) {
        py_printexc();
        object_error((t_object*)x, "error calling method: %s", method_name);
        return MAX_ERR_GENERIC;
    }

    return MAX_ERR_NONE;
}

// ----------------------------------------------------------------------------
// Message handlers

void pyext_bang(t_pyext* x)
{
    long inlet = proxy_getinlet((t_object*)x);
    pyext_call_method_noargs(x, "bang");
}

void pyext_int(t_pyext* x, long n)
{
    long inlet = proxy_getinlet((t_object*)x);
    t_atom a;
    atom_setlong(&a, n);
    pyext_call_method(x, "int", 1, &a);
}

void pyext_float(t_pyext* x, double f)
{
    long inlet = proxy_getinlet((t_object*)x);
    t_atom a;
    atom_setfloat(&a, f);
    pyext_call_method(x, "float", 1, &a);
}

void pyext_list(t_pyext* x, t_symbol* s, long argc, t_atom* argv)
{
    long inlet = proxy_getinlet((t_object*)x);
    pyext_call_method(x, "list", argc, argv);
}

void pyext_anything(t_pyext* x, t_symbol* s, long argc, t_atom* argv)
{
    long inlet = proxy_getinlet((t_object*)x);

    // Try to call method with the message name
    pyext_call_method(x, s->s_name, argc, argv);
}

// ----------------------------------------------------------------------------
// Text Editor Methods

/**
 * @brief Double-click handler - opens code editor
 */
void pyext_dblclick(t_pyext* x)
{
    if (x->code_editor) {
        // Editor already exists, make it visible
        object_attr_setchar(x->code_editor, gensym("visible"), 1);
    } else {
        // Create new editor
        x->code_editor = object_new(CLASS_NOBOX, gensym("jed"), x, 0);

        // Load the script file content if we have a loaded script
        if (x->script_name != gensym("") && x->script_pathname[0] != 0) {
            // Read the file
            pyext_doread(x, x->script_name, 0, NULL);
            // Set the text in editor
            object_method(x->code_editor, gensym("settext"), *x->code, gensym("utf-8"));
        }

        object_attr_setchar(x->code_editor, gensym("scratch"), 1);
        char title[256];
        snprintf(title, sizeof(title), "pyext: %s", x->script_name->s_name);
        object_attr_setsym(x->code_editor, gensym("title"), gensym(title));
    }
}

/**
 * @brief Editor close handler - preserves text and optionally reloads
 */
void pyext_edclose(t_pyext* x, char** text, long size)
{
    // Free old code buffer
    if (x->code) {
        sysmem_freehandle(x->code);
    }

    // Save new text
    x->code = sysmem_newhandleclear(size + 1);
    sysmem_copyptr((char*)*text, *x->code, size);
    x->code_size = size + 1;
    x->code_editor = NULL;

    // Optionally reload on close
    if (x->run_on_close && x->code_size > 2) {
        // Write back to file
        if (x->script_pathname[0] != 0) {
            t_filehandle fh;
            t_max_err err = path_createsysfile(x->script_filename, x->script_path,
                                              'TEXT', &fh);
            if (err == MAX_ERR_NONE) {
                t_ptr_size write_size = (t_ptr_size)size;
                sysfile_write(fh, &write_size, *x->code);
                sysfile_close(fh);
            }
        }
        // Reload the script
        pyext_reload(x);
    }
}

/**
 * @brief Editor save handler - optionally reloads on save
 */
t_max_err pyext_edsave(t_pyext* x, char** text, long size)
{
    if (x->run_on_save) {
        object_post((t_object*)x, "run-on-save: reloading script");

        // Save text to code buffer
        if (x->code) {
            sysmem_freehandle(x->code);
        }
        x->code = sysmem_newhandleclear(size + 1);
        sysmem_copyptr((char*)*text, *x->code, size);
        x->code_size = size + 1;

        // Write back to file
        if (x->script_pathname[0] != 0) {
            t_filehandle fh;
            t_max_err err = path_createsysfile(x->script_filename, x->script_path,
                                              'TEXT', &fh);
            if (err == MAX_ERR_NONE) {
                t_ptr_size write_size = (t_ptr_size)size;
                sysfile_write(fh, &write_size, *x->code);
                sysfile_close(fh);
                // Reload the script
                pyext_reload(x);
            } else {
                object_error((t_object*)x, "failed to save script");
                return MAX_ERR_GENERIC;
            }
        }
    }
    return MAX_ERR_NONE;
}

/**
 * @brief Configure editor close behavior
 */
void pyext_okclose(t_pyext* x, char* s, short* result)
{
    *result = 3; // Don't put up a dialog
}

/**
 * @brief Read script file into editor
 */
void pyext_read(t_pyext* x, t_symbol* s)
{
    defer((t_object*)x, (method)pyext_doread, s, 0, NULL);
}

/**
 * @brief Deferred read function
 */
void pyext_doread(t_pyext* x, t_symbol* s, long argc, t_atom* argv)
{
    t_max_err err;
    t_filehandle fh;
    char filename[MAX_PATH_CHARS];
    short path;

    // Use provided symbol or current script
    if (s == gensym("") || s == NULL) {
        if (x->script_pathname[0] == 0) {
            object_error((t_object*)x, "no script loaded");
            return;
        }
        strncpy_zero(filename, x->script_filename, MAX_PATH_CHARS);
        path = x->script_path;
    } else {
        t_fourcc filetype = FOUR_CHAR_CODE('TEXT');
        t_fourcc outtype = 0;
        strncpy_zero(filename, s->s_name, MAX_PATH_CHARS);
        if (locatefile_extended(filename, &path, &outtype, &filetype, 1)) {
            object_error((t_object*)x, "can't find file: %s", s->s_name);
            return;
        }
    }

    // Read the file
    err = path_opensysfile(filename, path, &fh, READ_PERM);
    if (err == MAX_ERR_NONE) {
        sysfile_readtextfile(fh, x->code, 0, TEXT_LB_UNIX | TEXT_NULL_TERMINATE);
        sysfile_close(fh);
        x->code_size = sysmem_handlesize(x->code);
    } else {
        object_error((t_object*)x, "error reading file: %s", filename);
    }
}

// ----------------------------------------------------------------------------
// pyext_error - formatted error message

void pyext_error(t_pyext* x, const char* fmt, ...)
{
    char buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    object_error((t_object*)x, "%s", buf);
}
