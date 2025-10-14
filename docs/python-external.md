# Writing Max Externals in Python via pocketpy

## Analysis

Based on examination of the pktpy codebase, here's what would be required to enable writing Max externals primarily in Python, similar to how the `js` object allows JavaScript externals.

## Current State

### What Already Exists

The codebase already has extensive Python bindings for the Max C API in `source/projects/pktpy/pktpy_api.h` and `source/projects/pktpy/api/*.h`:

- **Outlet/Inlet management** - Can create outlets/inlets and send messages (source/projects/pktpy/api/api_outlet.h:59-176)
- **Atom handling** - Full atom/atomarray conversion between Python and Max
- **Object/Patcher manipulation** - Query and modify patch structure
- **Clocks/Timers** - Schedule deferred execution (api_clock.h)
- **Buffers** - MSP buffer access (api_buffer.h)
- **Dictionaries, Hashtabs, Linklists** - Max data structures
- **Database, File I/O** - External data access
- **Threading** - SysThread and SysMutex wrappers
- **Message sending** - Can send messages between objects (api_message.h)

### Current Architecture

The `pktpy` external (source/projects/pktpy/pktpy.c:57-162) is a traditional Max external that:
1. Embeds pocketpy interpreter
2. Executes Python code on demand (via `eval`, `exec`, `execfile`)
3. Returns results through outlets
4. Has a code editor for live coding

## What's Missing for Full External Development in Python

To write Max externals primarily in Python (where Python defines the behavior), you need:

### 1. Dynamic Method Registration
- Max requires method registration at class creation time in `ext_main()` via `class_addmethod()`
- Current pktpy has fixed methods (bang, float, eval, exec, etc.) defined in C
- **Need:** Mechanism to scan a Python class for methods and dynamically register them with Max's method system

### 2. Instance Management
- Each Max object instance needs an associated Python object instance
- Current pktpy has one global Python namespace
- **Need:** Per-instance Python object storage and lifecycle management

### 3. Attribute System Integration
- Max attributes (visible in Inspector) are defined via `CLASS_ATTR_*` macros
- **Need:** Way to declare Python properties/variables as Max attributes

### 4. Inlet/Outlet Creation from Python
- Inlets/outlets must be created during object instantiation (`pktpy_new()`)
- **Need:** Python `__init__` method that can specify inlet/outlet configuration before Max finishes object creation

### 5. Message Routing Bridge
- **Need:** C bridge functions that receive Max messages and route them to Python methods with proper argument conversion

## Two Possible Approaches

### Approach 1: Generic "pyext" External (Recommended)

Create a single Max external similar to `js` that loads Python scripts and routes messages to Python objects.

**Architecture:**
```c
// C struct
typedef struct {
    t_object ob;
    py_GlobalRef py_instance;  // Python object instance
    void* outlets[MAX_OUTLETS];
    void* inlets[MAX_INLETS];
    t_symbol* script_name;
} t_pyext;
```

**Python Base Class:**
```python
# user_script.py
import api

class MyExternal:
    def __init__(self):
        self.inlets = 2
        self.outlets = 3
        self.value = 0

    def bang(self):
        """Responds to bang message"""
        api.post(f"Value is {self.value}")
        # outlet_bang called automatically on outlet 0

    def set_value(self, x):
        """Custom message handler"""
        self.value = x
```

**C Implementation Requirements:**

1. **In `ext_main()`:**
   - Register generic message handlers that forward to Python
   - Use `class_addmethod(c, (method)pyext_anything, "anything", A_GIMME, 0)` as catch-all

2. **In `pyext_new()`:**
   - Load Python script specified in object arguments
   - Instantiate Python class
   - Query Python object for `inlets` and `outlets` count
   - Create inlets/outlets accordingly
   - Store `py_GlobalRef` in struct

3. **In message handlers (e.g., `pyext_bang()`):**
   ```c
   void pyext_bang(t_pyext* x) {
       // Call Python method
       py_Ref py_inst = x->py_instance;
       py_Ref method = py_getattr(py_inst, py_name("bang"));
       if (method != NULL) {
           py_push(method);
           py_pushnil();
           py_vectorcall(0, 0);
       }
   }
   ```

4. **Generic `anything` handler:**
   ```c
   void pyext_anything(t_pyext* x, t_symbol* s, long argc, t_atom* argv) {
       // Look up method by name
       py_Ref method = py_getattr(x->py_instance, py_name(s->s_name));
       if (method != NULL) {
           // Convert atoms to Python args and call
           // Convert return value to atoms and output
       }
   }
   ```

**Advantages:**
- No compilation needed - just edit Python files
- Similar workflow to `js` object
- Dynamic - can reload scripts
- Good for rapid prototyping

**Challenges:**
- Method discovery is runtime, not compile-time
- Attributes harder to expose to Inspector (would need custom implementation)
- Slightly less performant than compiled C

### Approach 2: Code Generation + Compilation

Write Python code that generates C source for a proper Max external.

**Python DSL:**
```python
# define_external.py
from maxgen import External, inlet, outlet, method, attribute

class MyExternal(External):
    name = "myext"

    @attribute(type='float', default=0.5)
    def gain(self, value):
        self._gain = value

    @inlet(0)
    @method
    def bang(self):
        self.outlets[0].float(self.gain * 440.0)

    @method
    def set_frequency(self, freq: float):
        self.outlets[0].float(freq * self.gain)
```

Generator produces C code with proper `ext_main()`, class registration, etc.

**Advantages:**
- Native performance
- Full Max integration (attributes, help files, etc.)
- Type checking possible

**Challenges:**
- Requires build system
- Less dynamic than Approach 1
- More complex tooling needed

## Recommendation: Start with Approach 1

### Immediate Next Steps to Implement pyext

1. **Create new external `pyext.c`** based on `pktpy.c` but with:
   - Argument parsing to get script filename
   - Python class instantiation per Max object
   - Generic message routing to Python methods

2. **Define Python base class API** in a module (e.g., `maxext.py`):
   ```python
   class MaxObject:
       def __init__(self):
           self.inlets = 1
           self.outlets = 1
   ```

3. **Implement message routing** for common messages:
   - bang, int, float, list, anything
   - Map to Python methods with same names

4. **Implement attribute system** (Phase 2):
   - Scan Python class for decorated methods/properties
   - Register as Max attributes in `ext_main()`

5. **Add introspection helpers** in Python:
   - `self.get_patcher()` - returns Patcher object
   - `self.get_box()` - returns Box object
   - Already have outlet methods in api_outlet.h

## Key Technical Considerations

### Memory Management
- Python objects must stay alive for Max object lifetime
- Use `py_GlobalRef` stored in C struct
- Free in `pyext_free()`

### Thread Safety
- Max is not thread-safe
- Python code runs in main thread
- Already have SysThread wrappers if needed

### Error Handling
- Python exceptions must be caught and converted to Max errors
- Use `py_printexc()` to show in Max console

### Reload Capability
- Could implement hot-reload by tracking file modification times
- Re-instantiate Python objects when script changes

## Conclusion

This is definitely feasible and would be similar in architecture to how Max's `js` object works with V8. The extensive Max API wrappers already built provide most of the necessary infrastructure.
