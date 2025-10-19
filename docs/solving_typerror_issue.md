# Solving the TypeError Issue in pyext

## Problem Statement

When implementing `pyext`, a Max/MSP external that allows writing Max objects in Python using the embedded pocketpy interpreter, we encountered a persistent `TypeError: expected 1 arguments, got 2` error when calling methods on Python objects from C code.

The error manifested in two contexts:
1. Calling methods on the main Python `External` class instance (e.g., `bang()`, `int()`, `float()`)
2. Calling methods on `PyextOutlet` wrapper objects (e.g., `outlet.int(42)`, `outlet.float(1.0)`)

## Root Causes

The issue had multiple layers:

### 1. Instance Storage Problem

**Initial Mistake**: Storing Python instance references in temporary pocketpy registers.

```c
// WRONG - registers are temporary working storage
py_Ref instance_storage = py_getreg(10);
py_assign(instance_storage, py_retval());
x->py_instance = instance_storage;  // This becomes invalid!
```

**Why it failed**: Pocketpy registers (`py_Ref` from `py_getreg()`) are temporary scratch space that gets reused during operations. Storing a long-term reference to a register results in stale pointers and crashes.

**Solution**: Store instances in the Python global namespace with unique names.

```c
// CORRECT - use global namespace for persistence
typedef struct t_pyext {
    char py_instance_name[128];  // e.g., "_pyext_inst_0x123456"
    // ...
} t_pyext;

// Store in globals
snprintf(x->py_instance_name, sizeof(x->py_instance_name),
         "_pyext_inst_%p", (void*)x);
py_setglobal(py_name(x->py_instance_name), py_retval());

// Retrieve later
py_GlobalRef instance = py_getglobal(py_name(x->py_instance_name));
```

### 2. Method Calling Confusion

**Initial Mistake**: Trying to call unbound methods from the class with instance as first argument.

```c
// WRONG - overly complex, doesn't work with pocketpy's API
py_GlobalRef py_class = py_getglobal(py_name("External"));
py_getattr(py_class, py_name("method_name"));  // Get unbound method
py_GlobalRef unbound_method = py_retval();
py_push(unbound_method);
py_push(instance);  // Try to pass self explicitly
py_vectorcall(1, 0);  // Causes crashes
```

**Why it failed**: We were mixing calling conventions, mismanaging the stack, and not properly handling `py_retval()` which points to a register that gets reused.

**Solution**: Get the bound method from the instance and use `py_call()`.

```c
// CORRECT - get bound method from instance, use py_call()
py_GlobalRef instance = py_getglobal(py_name(x->py_instance_name));
if (!py_getattr(instance, py_name(method_name))) {
    return MAX_ERR_NONE;  // Method doesn't exist
}

// Save method immediately (py_retval() points to a reusable register)
py_Ref method_ref = py_getreg(10);
py_assign(method_ref, py_retval());

// Use py_call() - it handles stack management internally
bool ok = py_call(method_ref, 0, NULL);  // For no args
```

For methods with arguments:

```c
// Build arguments in high-numbered registers to avoid conflicts
for (long i = 0; i < argc; i++) {
    py_Ref arg_reg = py_getreg(11 + i);
    py_newint(arg_reg, value);  // or py_newfloat, py_newstr, etc.
}

// Call with argument array
py_Ref first_arg = argc > 0 ? py_getreg(11) : NULL;
bool ok = py_call(method_ref, (int)argc, first_arg);
```

### 3. The Critical py_bindmethod() Argument Convention

**The Root Cause of TypeError**: Misunderstanding how `py_bindmethod()` passes arguments.

When we defined outlet methods like this:

```c
// WRONG - assumes self is NOT counted in argc
static bool pyext_outlet_int(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);  // Expects just the int value
    PY_CHECK_ARG_TYPE(0, tp_int);

    PyextOutlet* self = py_touserdata(py_arg(-1));  // WRONG location
    py_i64 value = py_toint(py_arg(0));
    // ...
}
```

**What Actually Happens**: When pocketpy calls a method bound with `py_bindmethod()`:
- `argc` **INCLUDES** `self` in the count
- `argv[0]` **IS** `self` (the first argument)
- Explicit arguments follow: `argv[1]`, `argv[2]`, etc.

So when Python calls `outlet.int(42)`:
- `argc = 2` (not 1!)
- `argv[0]` = self (the PyextOutlet instance)
- `argv[1]` = 42 (the integer value)

**The Fix**:

```c
// CORRECT - self is the first argument, included in argc
static bool pyext_outlet_int(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);  // self + value = 2 arguments
    PY_CHECK_ARG_TYPE(1, tp_int);  // Check second arg (index 1)

    PyextOutlet* self = py_touserdata(py_arg(0));  // self is FIRST
    py_i64 value = py_toint(py_arg(1));            // value is SECOND

    if (self->outlet) {
        outlet_int(self->outlet, (long)value);
    }

    py_newnone(py_retval());
    return true;
}
```

## Evidence from pocketpy Source Code

Looking at pocketpy's own examples in `pktpy_api.h`:

```c
static bool Person__init__(int argc, py_Ref argv) {
    post("Person__init__ argc = %d", argc);
    if (argc == 1) {
        // Just self, no other args
    }
    else if(argc == 3) {
        t_person* ptr = py_touserdata(py_arg(0));  // self at index 0
        PY_CHECK_ARG_TYPE(1, tp_int);              // first arg at index 1
        py_i64 id = py_toint(py_arg(1));
        PY_CHECK_ARG_TYPE(2, tp_int);              // second arg at index 2
        py_i64 age = py_toint(py_arg(2));
        t_person__ctor(ptr, id, age);
    } else {
        return TypeError("Person__init__(): expected 0 or 2 arguments, got %d", argc - 1);
    }
    // ...
}
```

Notice:
- With 2 explicit args, it checks `argc == 3` (self + 2 args)
- `self` is accessed via `py_arg(0)`
- First explicit arg is at `py_arg(1)`
- Error message uses `argc - 1` to report user-visible arg count

## Complete Working Solution

### PyextOutlet Methods (pyext.c)

```c
// PyextOutlet.int(value)
static bool pyext_outlet_int(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);  // self + value
    PY_CHECK_ARG_TYPE(1, tp_int);

    PyextOutlet* self = py_touserdata(py_arg(0));
    py_i64 value = py_toint(py_arg(1));

    if (self->outlet) {
        outlet_int(self->outlet, (long)value);
    }

    py_newnone(py_retval());
    return true;
}

// PyextOutlet.float(value)
static bool pyext_outlet_float(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);  // self + value

    PyextOutlet* self = py_touserdata(py_arg(0));
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
    PY_CHECK_ARGC(1);  // just self

    PyextOutlet* self = py_touserdata(py_arg(0));

    if (self->outlet) {
        outlet_bang(self->outlet);
    }

    py_newnone(py_retval());
    return true;
}
```

### External Method Calling (pyext.c)

```c
t_max_err pyext_call_method_noargs(t_pyext* x, const char* method_name)
{
    // Get instance from globals
    py_GlobalRef instance = py_getglobal(py_name(x->py_instance_name));
    if (instance == NULL) {
        object_error((t_object*)x, "instance not found in globals");
        return MAX_ERR_GENERIC;
    }

    // Get bound method from instance
    if (!py_getattr(instance, py_name(method_name))) {
        return MAX_ERR_NONE;  // Method doesn't exist
    }

    // Call immediately using py_call (handles bound methods correctly)
    bool ok = py_call(py_retval(), 0, NULL);

    if (!ok) {
        py_printexc();
        object_error((t_object*)x, "error calling method: %s", method_name);
        return MAX_ERR_GENERIC;
    }

    return MAX_ERR_NONE;
}

t_max_err pyext_call_method(t_pyext* x, const char* method_name,
                           long argc, t_atom* argv)
{
    py_GlobalRef instance = py_getglobal(py_name(x->py_instance_name));
    if (instance == NULL) {
        object_error((t_object*)x, "instance not found in globals");
        return MAX_ERR_GENERIC;
    }

    if (!py_getattr(instance, py_name(method_name))) {
        return MAX_ERR_NONE;
    }

    // Save method to register 10
    py_Ref method_ref = py_getreg(10);
    py_assign(method_ref, py_retval());

    // Build arguments in registers 11+
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
                py_newnone(arg_reg);
                break;
        }
    }

    // Call with argument array
    py_Ref first_arg = argc > 0 ? py_getreg(11) : NULL;
    bool ok = py_call(method_ref, (int)argc, first_arg);

    if (!ok) {
        py_printexc();
        object_error((t_object*)x, "error calling method: %s", method_name);
        return MAX_ERR_GENERIC;
    }

    return MAX_ERR_NONE;
}
```

## Key Takeaways

1. **Never store long-term references to pocketpy registers** - they're temporary scratch space
2. **Use the Python global namespace** for persistent object storage
3. **Use `py_call()` not `py_vectorcall()`** for simpler, safer method calls
4. **Get bound methods from instances**, not unbound methods from classes
5. **CRITICAL**: When using `py_bindmethod()`:
   - `argc` includes `self`
   - `argv[0]` is `self`
   - Explicit arguments are `argv[1]`, `argv[2]`, etc.
6. **Save `py_retval()` immediately** - it points to a register that gets reused
7. **Use high-numbered registers** (10+) to avoid conflicts with internal operations

## Testing

The fix was verified by successfully:
- Creating a `pyext` object with `simple.py`
- Sending `bang` messages (no arguments)
- Sending `int` messages (one integer argument)
- Sending `float` messages (one float argument)
- Having Python code call `self._outlets[0].int(n)` and `self._outlets[1].float(f)`
- All operations completed without TypeError or crashes

## References

- pocketpy v2.0.8 source code: `source/thirdparty/pocketpy/`
- pocketpy C API header: `include/pocketpy/pocketpy.h`
- Working examples: `source/projects/pktpy/pktpy_api.h` (Person class)
- Max SDK documentation: `source/max-sdk-base/`
