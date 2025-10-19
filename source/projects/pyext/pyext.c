// pyext.c
// Python external - write Max externals in Python

#include "pyext.h"
#include "pktpy_api.h"

// ----------------------------------------------------------------------------
// globals

t_class* pyext_class = NULL;
static bool pyext_py_initialized = false;

// ----------------------------------------------------------------------------
// PyextOutlet - Simple outlet wrapper for pyext
// We define our own instead of using pktpy's Outlet to avoid conflicts

typedef struct {
    void* outlet;  // Max outlet pointer
} PyextOutlet;

static py_Type g_pyext_outlet_type = 0;

// PyextOutlet.int(value)
static bool pyext_outlet_int(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);  // self + value
    PY_CHECK_ARG_TYPE(1, tp_int);

    PyextOutlet* self = py_touserdata(py_arg(0));  // self is first arg
    py_i64 value = py_toint(py_arg(1));  // value is second arg

    if (self->outlet) {
        outlet_int(self->outlet, (long)value);
    }

    py_newnone(py_retval());
    return true;
}

// PyextOutlet.float(value)
static bool pyext_outlet_float(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);  // self + value

    PyextOutlet* self = py_touserdata(py_arg(0));  // self is first arg
    double value = 0.0;

    if (py_isint(py_arg(1))) {
        value = (double)py_toint(py_arg(1));
    } else if (py_isfloat(py_arg(1))) {
        value = py_tofloat(py_arg(1));
    } else {
        return TypeError("expected int or float");
    }

    if (self->outlet) {
        outlet_float(self->outlet, value);
    }

    py_newnone(py_retval());
    return true;
}

// PyextOutlet.bang()
static bool pyext_outlet_bang(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);  // just self, no other args

    PyextOutlet* self = py_touserdata(py_arg(0));  // self is first arg

    if (self->outlet) {
        outlet_bang(self->outlet);
    }

    py_newnone(py_retval());
    return true;
}

// Register PyextOutlet type
static void pyext_register_outlet_type() {
    g_pyext_outlet_type = py_newtype("PyextOutlet", tp_object, NULL, NULL);

    py_bindmethod(g_pyext_outlet_type, "int", pyext_outlet_int);
    py_bindmethod(g_pyext_outlet_type, "float", pyext_outlet_float);
    py_bindmethod(g_pyext_outlet_type, "bang", pyext_outlet_bang);
}

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
    class_addmethod(c, (method)pyext_load,     "load",     A_DEFSYM, 0);

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

        // Register our PyextOutlet type
        pyext_register_outlet_type();

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
        snprintf(x->py_instance_name, sizeof(x->py_instance_name),
                "_pyext_inst_%p", (void*)x);
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

        // Inject outlet wrappers into Python instance
        if (x->py_instance_name[0] != '\0') {
            pyext_inject_outlets(x);
        }
    }

    return (x);
}

// ----------------------------------------------------------------------------
// pyext_free - cleanup

void pyext_free(t_pyext* x)
{
    // Remove Python instance from global namespace
    if (x->py_instance_name[0] != '\0') {
        py_Ref r0 = py_getreg(0);
        py_newnone(r0);
        py_setglobal(py_name(x->py_instance_name), r0);
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
// pyext_find_external_class - find class marked as external
//
// Searches for external class in this priority order:
// 1. Check global registry __pyext_external_class__ (set by @api.external decorator)
// 2. Class named "External" (backward compatibility)
//
// Returns: py_GlobalRef to the class, or NULL if not found

py_GlobalRef pyext_find_external_class(t_pyext* x)
{
    // First priority: Check if a class was registered via @api.external decorator
    py_GlobalRef registered_class = py_getglobal(py_name("__pyext_external_class__"));
    if (registered_class != NULL) {
        object_post((t_object*)x, "found registered external class via @api.external");
        return registered_class;
    }

    // Second priority: Try to find class named "External" (backward compatibility)
    py_GlobalRef external_class = py_getglobal(py_name("External"));
    if (external_class != NULL) {
        object_post((t_object*)x, "found External class (legacy)");
        return external_class;
    }

    // No external class found
    return NULL;
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

    // Find the external class using flexible detection
    py_GlobalRef external_class = pyext_find_external_class(x);
    if (external_class == NULL) {
        object_error((t_object*)x,
                   "script must define an external class:\n"
                   "  1. Use @api.external decorator on any class, OR\n"
                   "  2. Name your class 'External' (legacy)");
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

    // Store instance in global namespace with unique name
    py_setglobal(py_name(x->py_instance_name), py_retval());
    x->py_class_type = py_typeof(py_retval());

    object_post((t_object*)x, "stored instance as '%s', type=%d",
                x->py_instance_name, x->py_class_type);

    // Query for inlets and outlets count using py_getdict (direct __dict__ access)
    // This bypasses property lookups and gets the actual instance variable
    py_GlobalRef instance = py_getglobal(py_name(x->py_instance_name));

    // Try to get 'inlets' from instance __dict__
    py_ItemRef inlets_item = py_getdict(instance, py_name("inlets"));
    if (inlets_item != NULL && py_isint(inlets_item)) {
        x->num_inlets = py_toint(inlets_item);
        object_post((t_object*)x, "read inlets from __dict__ = %ld", x->num_inlets);
        if (x->num_inlets < 1) x->num_inlets = 1;
        if (x->num_inlets > PYEXT_MAX_INLETS) x->num_inlets = PYEXT_MAX_INLETS;
    }

    // Try to get 'outlets' from instance __dict__
    py_ItemRef outlets_item = py_getdict(instance, py_name("outlets"));
    if (outlets_item != NULL && py_isint(outlets_item)) {
        py_i64 raw_outlets = py_toint(outlets_item);
        x->num_outlets = raw_outlets;
        object_post((t_object*)x, "read outlets from __dict__ = %lld", raw_outlets);
        if (x->num_outlets < 1) x->num_outlets = 1;
        if (x->num_outlets > PYEXT_MAX_OUTLETS) x->num_outlets = PYEXT_MAX_OUTLETS;
    }

    sysmem_freehandle(code_handle);
    object_post((t_object*)x, "loaded script: %s", script_name->s_name);
    object_post((t_object*)x, "inlets: %ld, outlets: %ld",
               x->num_inlets, x->num_outlets);

    // Note: Outlets will be created in pyext_setup_inlets_outlets() after this returns
    // We'll inject outlet wrappers after that in pyext_new()

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

    // Clear current instance from globals
    if (x->py_instance_name[0] != '\0') {
        py_Ref r0 = py_getreg(0);
        py_newnone(r0);
        py_setglobal(py_name(x->py_instance_name), r0);
    }

    // Clean up old inlets/outlets before recreating them
    // Free inlet proxies (skip first inlet which is the main inlet)
    for (int i = 1; i < x->num_inlets; i++) {
        if (x->inlets[i] != NULL) {
            object_free(x->inlets[i]);
            x->inlets[i] = NULL;
        }
    }

    // Free outlets
    for (int i = 0; i < x->num_outlets; i++) {
        if (x->outlets[i] != NULL) {
            outlet_delete(x->outlets[i]);
            x->outlets[i] = NULL;
        }
    }

    // Reload the script - this will update num_inlets and num_outlets
    t_max_err err = pyext_load_script(x, x->script_name);
    if (err != MAX_ERR_NONE) {
        return err;
    }

    // Recreate inlets and outlets with potentially new counts
    pyext_setup_inlets_outlets(x);

    // Inject outlet wrappers into the new Python instance
    pyext_inject_outlets(x);

    object_post((t_object*)x, "script reloaded successfully");
    return MAX_ERR_NONE;
}

// ----------------------------------------------------------------------------
// pyext_load - load a new Python script

t_max_err pyext_load(t_pyext* x, t_symbol* s)
{
    if (s == gensym("") || s == NULL) {
        object_error((t_object*)x, "load requires a filename argument");
        return MAX_ERR_GENERIC;
    }

    object_post((t_object*)x, "loading script: %s", s->s_name);

    // Clear current instance from globals
    if (x->py_instance_name[0] != '\0') {
        py_Ref r0 = py_getreg(0);
        py_newnone(r0);
        py_setglobal(py_name(x->py_instance_name), r0);
    }

    // Clean up old inlets/outlets before recreating them
    // Free inlet proxies (skip first inlet which is the main inlet)
    for (int i = 1; i < x->num_inlets; i++) {
        if (x->inlets[i] != NULL) {
            object_free(x->inlets[i]);
            x->inlets[i] = NULL;
        }
    }

    // Free outlets
    for (int i = 0; i < x->num_outlets; i++) {
        if (x->outlets[i] != NULL) {
            outlet_delete(x->outlets[i]);
            x->outlets[i] = NULL;
        }
    }

    // Update script name and load the new script
    x->script_name = s;
    t_max_err err = pyext_load_script(x, s);
    if (err != MAX_ERR_NONE) {
        return err;
    }

    // Recreate inlets and outlets with potentially new counts
    pyext_setup_inlets_outlets(x);

    // Inject outlet wrappers into the new Python instance
    pyext_inject_outlets(x);

    object_post((t_object*)x, "script loaded successfully");
    return MAX_ERR_NONE;
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
// pyext_inject_outlets - inject outlet wrapper objects into Python instance

t_max_err pyext_inject_outlets(t_pyext* x)
{
    if (x->py_instance_name[0] == '\0') {
        return MAX_ERR_GENERIC;
    }

    // Get the instance from globals
    py_GlobalRef instance = py_getglobal(py_name(x->py_instance_name));
    if (instance == NULL) {
        object_error((t_object*)x, "instance not found in globals");
        return MAX_ERR_GENERIC;
    }

    // Create a list of Outlet wrapper objects
    py_Ref outlets_list = py_getreg(0);
    py_newlistn(outlets_list, x->num_outlets);

    // Populate the list with PyextOutlet objects
    for (long i = 0; i < x->num_outlets; i++) {
        py_Ref outlet_item = py_list_getitem(outlets_list, i);

        // Create PyextOutlet object with userdata
        PyextOutlet* outlet_obj = py_newobject(outlet_item, g_pyext_outlet_type, 0, sizeof(PyextOutlet));
        if (outlet_obj) {
            outlet_obj->outlet = x->outlets[i];
        } else {
            object_error((t_object*)x, "failed to allocate PyextOutlet for outlet %ld", i);
        }
    }

    // Set the outlets_list attribute on the Python instance
    py_setattr(instance, py_name("_outlets"), outlets_list);

    object_post((t_object*)x, "injected %ld outlet(s) into Python instance", x->num_outlets);

    return MAX_ERR_NONE;
}

// ----------------------------------------------------------------------------
// pyext_call_method_noargs - call Python method with no arguments

t_max_err pyext_call_method_noargs(t_pyext* x, const char* method_name)
{
    if (x->py_instance_name[0] == '\0') {
        object_error((t_object*)x, "no Python instance");
        return MAX_ERR_GENERIC;
    }

    // Get the instance from globals
    py_GlobalRef instance = py_getglobal(py_name(x->py_instance_name));
    if (instance == NULL) {
        object_error((t_object*)x, "instance not found in globals");
        return MAX_ERR_GENERIC;
    }

    // Try to get the method attribute
    if (!py_getattr(instance, py_name(method_name))) {
        // Method doesn't exist - not an error, just silently return
        return MAX_ERR_NONE;
    }

    // The method is now in py_retval()
    // Use py_call which is safer for bound methods
    bool ok = py_call(py_retval(), 0, NULL);

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
    if (x->py_instance_name[0] == '\0') {
        object_error((t_object*)x, "no Python instance");
        return MAX_ERR_GENERIC;
    }

    // Get the instance from globals
    py_GlobalRef instance = py_getglobal(py_name(x->py_instance_name));
    if (instance == NULL) {
        object_error((t_object*)x, "instance not found in globals");
        return MAX_ERR_GENERIC;
    }

    // Try to get the method attribute
    if (!py_getattr(instance, py_name(method_name))) {
        // Method doesn't exist - not an error, just silently return
        return MAX_ERR_NONE;
    }

    // Save the method immediately
    py_Ref method_ref = py_getreg(10);
    py_assign(method_ref, py_retval());

    // Build argument array in consecutive registers
    // Start at register 11 to avoid conflicts
    if (argc > 16) {
        object_error((t_object*)x, "too many arguments (max 16)");
        return MAX_ERR_GENERIC;
    }

    for (long i = 0; i < argc; i++) {
        py_Ref arg_reg = py_getreg(11 + i);
        switch (atom_gettype(argv + i)) {
            case A_LONG:
                py_newint(arg_reg, atom_getlong(argv + i));
                break;
            case A_FLOAT:
                py_newfloat(arg_reg, atom_getfloat(argv + i));
                break;
            case A_SYM:
                py_newstr(arg_reg, atom_getsym(argv + i)->s_name);
                break;
            default:
                object_warn((t_object*)x, "unsupported atom type");
                py_newnone(arg_reg);
                break;
        }
    }

    // Call using py_call - pass pointer to first argument register
    py_Ref first_arg = argc > 0 ? py_getreg(11) : NULL;
    bool ok = py_call(method_ref, (int)argc, first_arg);

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
