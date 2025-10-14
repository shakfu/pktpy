# api_external.h - PyExternal Wrapper

## Overview

The `api_external.h` file provides a Python wrapper (`PyExternal`) for the pktpy Max/MSP external object. This allows Python code running inside pktpy to access and interact with the parent external object.

## Purpose

The PyExternal wrapper enables Python scripts to:
- Access the external's outlets
- Get the parent patcher
- Post messages to the Max console
- Send output through the external's outlets
- Access the external as a Max object

## Architecture

```
┌─────────────────────────────────────┐
│  Python Code (running in pktpy)    │
│                                     │
│  ext = api.PyExternal()             │
│  ext.post("Hello")                  │
│  ext.out(42)                        │
└──────────────┬──────────────────────┘
               │
               ↓
        ┌──────────────┐
        │ PyExternal   │  (api_external.h)
        │   Wrapper    │
        └──────┬───────┘
               │
               ↓
        ┌──────────────┐
        │   t_pktpy    │  (pktpy.c)
        │  Max Object  │
        └──────────────┘
```

## Structure

### PyExternalObject

```c
typedef struct {
    struct t_pktpy* external;  // Pointer to the pktpy external instance
} PyExternalObject;
```

The wrapper stores a pointer to the actual `t_pktpy` struct defined in `pktpy.c`.

## Methods

### Constructor

**`PyExternal()`**
- Creates an empty PyExternal object (external pointer is NULL)

**`PyExternal(ptr)`**
- Creates PyExternal from integer pointer value
- For internal use when passing external reference to Python

### Representation

**`__str__()`, `__repr__()`**
- Returns string representation: `"PyExternal(<address>)"`
- Shows `"PyExternal(None)"` if external is NULL

### Utility Methods

**`is_valid()`**
- Returns `bool` indicating if external pointer is not NULL
- Check before calling methods that require valid external

**`get_pointer()`**
- Returns external pointer as integer
- For internal/advanced use

### Console Output

**`post(message)`**
- Posts message to Max console
- Uses Max's `post()` function
- Parameters:
  - `message`: String to post

### Outlet Access

**`get_outlet_left()`**
- Returns pointer to left outlet (main output)
- Returns integer pointer value

**`get_outlet_middle()`**
- Returns pointer to middle outlet (error/status)
- Returns integer pointer value

**`get_outlet_right()`**
- Returns pointer to right outlet (success/completion)
- Returns integer pointer value

### Object Access

**`get_owner()`**
- Returns pointer to external as `t_object*`
- Returns integer pointer value
- Can be wrapped in Object type

**`get_patcher()`**
- Returns pointer to parent patcher
- Returns integer pointer value or None
- Can be wrapped in Patcher type

**`get_name()`**
- Returns name of external
- Currently returns static "pktpy"

### Output Methods

**`bang_left()`**
- Sends bang to left outlet
- No parameters

**`out(value)`**
- Sends value to left outlet
- Handles int, float, and string types
- Parameters:
  - `value`: Python value to output

## Usage Examples

### Basic Usage

```python
import api

# Create PyExternal instance
ext = api.PyExternal()

# Check if valid
if ext.is_valid():
    api.post("External is valid")

# Post to console
ext.post("Hello from Python!")
```

### Outlet Usage

```python
import api

ext = api.PyExternal()

# Send bang
ext.bang_left()

# Send values
ext.out(42)
ext.out(3.14)
ext.out("message")
```

### Accessing Patcher

```python
import api

ext = api.PyExternal()

# Get patcher pointer
patcher_ptr = ext.get_patcher()

if patcher_ptr:
    # Wrap in Patcher object
    # patcher = api.Patcher(patcher_ptr)
    # patcher.method()
    pass
```

## Integration

### Files Modified

1. **api/api_external.h** (NEW)
   - Defines PyExternalObject struct
   - Implements all PyExternal methods

2. **api/api_common.h**
   - Added `extern py_Type g_pyexternal_type;`

3. **pktpy_api.h**
   - Included `api/api_external.h`
   - Added `py_Type g_pyexternal_type = -1;`
   - Registered PyExternal in `api_module_initialize()`

## Design Decisions

### Forward Declaration
Uses `struct t_pktpy` forward declaration to avoid circular dependencies between api_external.h and pktpy.c.

### Pointer as Integer
Outlets and object pointers are returned as integers rather than wrapped objects. This allows flexibility in how they're used and avoids circular dependencies.

### Limited Name Access
Currently `get_name()` returns static "pktpy" because accessing the full `t_pktpy` struct requires including pktpy.h, which would create circular dependency. Can be improved with proper header organization.

## Future Enhancements

1. **Property Access**
   - Expose `param0`, `param1`, `param2` as properties
   - Access to code editor state

2. **Direct Outlet Objects**
   - Return wrapped Outlet objects instead of raw pointers
   - Requires refactoring to avoid circular dependencies

3. **Inlet Access**
   - Methods to query and access inlets
   - Proxy inlet management

4. **Attribute Access**
   - Get/set custom attributes
   - Access instance name

5. **Better Integration**
   - Auto-initialization with current external context
   - Thread-safe access patterns

## Testing

See `examples/tests/test_pyexternal.py` for test examples.

## Related Files

- `source/projects/pktpy/pktpy.c` - Main external implementation
- `source/projects/pktpy/pktpy.h` - External header
- `source/projects/pktpy/api/api_object.h` - Generic Object wrapper
- `source/projects/pktpy/api/api_outlet.h` - Outlet wrapper
- `source/projects/pktpy/api/api_patcher.h` - Patcher wrapper
