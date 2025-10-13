# Implementation Notes: Max Type Wrappers

## Overview

This document describes the implementation of Python wrappers for Max/MSP's core types (`t_atom` and `t_symbol`) using pocketpy's C API.

## Architecture

### Type Wrapping Strategy

Both types use pocketpy's object system with user data:

```c
typedef struct {
    t_atom atom;
} AtomObject;

typedef struct {
    t_symbol* sym;
} SymbolObject;
```

- **AtomObject**: Contains actual `t_atom` struct (value semantics)
- **SymbolObject**: Contains pointer to Max's global symbol (reference semantics)

### Initialization Flow

1. `api_module_initialize()` called from `pktpy_init()` in pktpy.c:72
2. Creates `api` module with `py_newmodule()`
3. Registers types with `py_newtype()`
4. Binds methods/properties with `py_bindmethod()` and `py_bindproperty()`
5. Binds utility functions with `py_bindfunc()` and `py_bind()`

## Implementation Details

### Symbol Type

**File**: pktpy_api.h:168-244

#### Key Design Decisions

1. **No destructor needed**: Max symbols are globally interned and never freed
2. **Pointer comparison for equality**: Symbols with same name have same pointer
3. **String comparison fallback**: Allow comparing Symbol with Python strings
4. **Read-only name property**: Symbols are immutable in Max

#### Method Implementations

- `Symbol__new__`: Allocates `SymbolObject`, initializes with empty symbol
- `Symbol__init__`: Calls `gensym()` to get/create Max symbol
- `Symbol__str__`: Returns `s_name` field
- `Symbol__repr__`: Returns formatted string like `Symbol('name')`
- `Symbol__eq__`: Checks if other is Symbol (pointer compare) or string (strcmp)

### Atom Type

**File**: pktpy_api.h:246-402

#### Key Design Decisions

1. **Tagged union**: Atom type determined by `atom_gettype()`
2. **Automatic type conversion**: Setting `value` property changes type
3. **Type-safe methods**: Separate `is_long()`, `is_float()`, `is_symbol()` checkers
4. **Python-friendly API**: `.value` property returns appropriate Python type

#### Method Implementations

- `Atom__new__`: Allocates `AtomObject`, initializes as long(0)
- `Atom__init__`: Accepts int/float/str/Symbol, calls appropriate `atom_set*()` function
- `Atom__repr__`: Formatted string showing type and value
- `Atom_get_type`: Returns `"long"`, `"float"`, or `"symbol"` string
- `Atom_get_value`: Returns value as Python int/float/str based on type
- `Atom_set_value`: Accepts int/float/str, automatically sets correct atom type

### Utility Functions

**File**: pktpy_api.h:405-438

#### py_to_atom()

Converts Python value to Max atom:

```c
if (py_isint(py_val))
    atom_setlong(atom, py_toint(py_val));
else if (py_isfloat(py_val))
    atom_setfloat(atom, py_tofloat(py_val));
else if (py_isstr(py_val))
    atom_setsym(atom, gensym(py_tostr(py_val)));
```

Returns `false` if type not supported.

#### atom_to_py()

Converts Max atom to Python value via `py_retval()`:

```c
switch (atom_gettype(atom)) {
    case A_LONG:  py_newint(py_retval(), ...);
    case A_FLOAT: py_newfloat(py_retval(), ...);
    case A_SYM:   py_newstr(py_retval(), ...);
}
```

Returns `false` if atom type unknown.

### Max API Wrappers

**File**: pktpy_api.h:440-462

#### api_gensym()

Creates Symbol wrapper from string:

```c
const char* str = py_tostr(py_arg(0));
t_symbol* sym = gensym(str);
SymbolObject* obj = py_newobject(py_retval(), g_symbol_type, ...);
obj->sym = sym;
```

Requires global `g_symbol_type` variable (set during initialization).

#### Existing Functions

- `api_post()`: Wrapper for Max's `post()` function
- `api_error()`: Wrapper for Max's `error()` function

## Type Registration

**File**: pktpy_api.h:486-530

### Symbol Registration

```c
g_symbol_type = py_newtype("Symbol", tp_object, mod, NULL);
py_bindmethod(g_symbol_type, "__new__", Symbol__new__);
py_bindmethod(g_symbol_type, "__init__", Symbol__init__);
py_bindmethod(g_symbol_type, "__str__", Symbol__str__);
py_bindmethod(g_symbol_type, "__repr__", Symbol__repr__);
py_bindmethod(g_symbol_type, "__eq__", Symbol__eq__);
py_bindproperty(g_symbol_type, "name", Symbol_get_name, NULL);
```

Note: `g_symbol_type` stored globally for use by `api_gensym()`.

### Atom Registration

```c
py_Type atom_type = py_newtype("Atom", tp_object, mod, NULL);
py_bindmethod(atom_type, "__new__", Atom__new__);
py_bindmethod(atom_type, "__init__", Atom__init__);
py_bindmethod(atom_type, "__repr__", Atom__repr__);
py_bindproperty(atom_type, "type", Atom_get_type, NULL);
py_bindproperty(atom_type, "value", Atom_get_value, Atom_set_value);
py_bindmethod(atom_type, "is_long", Atom_is_long);
py_bindmethod(atom_type, "is_float", Atom_is_float);
py_bindmethod(atom_type, "is_symbol", Atom_is_symbol);
```

## Error Handling

All wrapper functions follow pocketpy conventions:

- Return `true` on success, `false` on error
- Use `PY_CHECK_ARGC(n)` to validate argument count
- Use `PY_CHECK_ARG_TYPE(idx, type)` to validate types
- Use `TypeError()` or `RuntimeError()` for custom errors

Example:

```c
static bool Atom__init__(int argc, py_Ref argv) {
    if (argc == 1) {
        // Valid
    } else if (argc == 2) {
        // Valid
    } else {
        return TypeError("Atom() takes 0 or 1 argument, got %d", argc - 1);
    }
    // ...
    return true;
}
```

## Memory Management

### Symbol Lifetime

- Max symbols are globally interned (never freed)
- `SymbolObject` only holds pointer, no ownership
- Python GC frees `SymbolObject` wrapper when no longer referenced
- Underlying `t_symbol*` remains valid forever

### Atom Lifetime

- `AtomObject` contains actual `t_atom` struct (16 bytes)
- Memory managed by Python GC
- When `AtomObject` is freed, `t_atom` is automatically freed with it
- Symbol atoms hold pointers to global symbol table (always valid)

## Performance Characteristics

### Symbol Operations

- Creation: O(log n) or O(1) with hash table (Max's gensym implementation)
- Comparison: O(1) pointer comparison
- String access: O(1) pointer dereference
- Memory: 8 bytes pointer + 16 bytes wrapper overhead

### Atom Operations

- Creation: O(1) stack allocation
- Type checking: O(1) enum comparison
- Value access: O(1) union access
- Type conversion: O(1) (no actual conversion, just different access)
- Memory: 16 bytes union + 16 bytes wrapper overhead

## Integration Points

### From Max to Python

Current implementation in pktpy.c converts Python return values to Max atoms:

- `pktpy_handle_output()` (line 254): Routes by type
- `pktpy_handle_long_output()` (line 264): Converts int to long
- `pktpy_handle_float_output()` (line 279): Converts float to float
- `pktpy_handle_string_output()` (line 294): Converts str to symbol
- `pktpy_handle_list_output()` (line 343): Converts list to atom array
- `pktpy_handle_tuple_output()` (line 447): Converts tuple to atom array

### From Python to Max (Future Work)

The `py_to_atom()` and `atom_to_py()` utilities enable future bidirectional conversion:

1. Accept Max atoms as Python function arguments
2. Return Python Atom objects from Max API functions
3. Convert Python lists to Max atom arrays
4. Build higher-level Max API wrappers

## Future Extensions

### Additional Max Types

Following the same pattern, could wrap:

- `t_object*`: Max object references
- `t_outlet*`: Outlet references for sending messages
- `t_clock*`: Timing/scheduling
- `t_buffer~*`: Audio buffer access
- `t_patcher*`: Patcher hierarchy access

### Additional Max API Functions

Console:
- `cpost()`, `error()` variants with formatted output
- `post_sym()`, `post_atom()`

Atoms:
- `atom_gettype_array()`: Batch type checking
- `atom_copy_array()`: Array operations
- Atom array constructors

Symbols:
- `symbol_unique()`: Generate unique symbols
- Symbol table iteration

Timing:
- `clock_new()`, `clock_delay()`
- `gettime()`, `setclock()`

### Python API Improvements

1. **Atom sequences**: Make Atom iterable for multi-value atoms
2. **Rich comparison**: Implement `<`, `>`, etc. for Atoms
3. **Atom arithmetic**: Support `+`, `*` etc. where sensible
4. **Type hinting**: Add `.pyi` stub file for IDE support
5. **Documentation strings**: Add `__doc__` attributes

## Testing Strategy

### Unit Tests

Test file: `tests/test_api_types.py`

Covers:
- Type construction (empty, with values)
- Property access (read/write)
- Type checking methods
- Type conversions
- Equality comparisons
- Cross-type interactions (Symbol with Atom)

### Integration Testing

Should test in Max environment:
1. Load pktpy external in Max
2. Send `execfile test_api_types.py` message
3. Verify console output matches expected
4. Test with actual Max objects and messages

### Performance Testing

Benchmark:
- Symbol creation rate (gensym calls/sec)
- Atom creation rate
- Type conversion overhead vs. native Python types
- Memory usage with large atom arrays

## Build Integration

- No additional build steps required
- Types automatically registered on external load
- Compiled into `pktpy.mxo` bundle
- File: `source/projects/pktpy/pktpy_api.h` (header-only)

## Comparison with v1

This implementation uses pocketpy v2's cleaner API:

**v1 (pocketpy 1.4.6)**:
- More verbose C++ style API
- Larger binary size (1.3MB)
- Complex template-based bindings

**v2 (pocketpy 2.0.8)**:
- Lua-like C API
- Smaller binary (572KB)
- Direct stack-based manipulation
- Simpler type registration

## References

- Max API: `ext.h`, `ext_obex.h` in max-sdk-base
- pocketpy API: `source/thirdparty/pocketpy/include/pocketpy/pocketpy.h`
- Atom functions: `atom_get*()`, `atom_set*()` in Max API
- Symbol functions: `gensym()` in Max API
- pocketpy examples: `source/projects/pktpy/tests/simple_struct/` (Vector2 example)
